/*
 * Copyright (c) 2025 pygrum.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define NOMINMAX

#include <filesystem>
#include <tchar.h>

#include "fb.hpp"
#include "handler.hpp"
#include "log.hpp"
#include "reporter.hpp"
#include "trace.hpp"

#define WSTROP(op) L#op

/*
Helpful references (since MSDN does not do its job):

https://stackoverflow.com/questions/38127255/get-created-modified-deleted-files-by-a-specific-process-from-an-event-tracing
*/

void FileTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto *userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	std::shared_mutex& mapLock = userContext->mapLock;
	governor::reporter* reporter = userContext->reporter;

	auto& fileContext = userContext->File;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);
	
	std::wstring operation;
	int opcode = schema.event_opcode();
	std::wstring filePath, newFilePath;
	bool isDir = false;
	Sensor::FileAction action = Sensor::FileAction_MIN;

	try
	{
		switch (opcode)
		{
		case FILEIO_CREATE:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			TRACE_FILE_MAP_ENTRY createRequest{ };

			createRequest.filePath = parser.parse<std::wstring>(L"OpenPath");
			createRequest.fileObject = parser.parse<krabs::pointer>(L"FileObject");
			createRequest.irp = parser.parse<krabs::pointer>(L"IrpPtr");
			uint32_t createOptions = parser.parse<uint32_t>(L"CreateOptions");
			createRequest.isDir = createOptions & FILE_DIRECTORY_FILE;

			if (fileContext.CreateRequestMap.count(createRequest.irp.address) > 0)
			{
				return; // shouldn't handle duplicate CreateFile requests
			}
			TRACE_FILE_INFO_ENTRY ie{};
			ie.path = createRequest.filePath;
			ie.isDir = createRequest.isDir;

			fileContext.ObjectInfoMap[createRequest.fileObject.address] = ie;
			fileContext.CreateRequestMap[createRequest.irp.address] = createRequest;
			return;
		}
		break;

		case FILEIO_OPEND:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			krabs::pointer irp = parser.parse<krabs::pointer>(L"IrpPtr");
			ULONG_PTR createStatus = parser.parse<krabs::pointer>(L"ExtraInfo").address;

			// check if it was createFile request
			const TRACE_FILE_MAP_ENTRY& createRequest = fileContext.CreateRequestMap.at(irp.address);
			if (createStatus == FILE_CREATED)
			{
				action = Sensor::FileAction_FileCreate;
				operation = WSTROP(Sensor::FileAction_FileCreate);
				filePath = createRequest.filePath;
				isDir = createRequest.isDir;
			}
			// statuses indicating that file was truncated
			else if (createStatus == FILE_SUPERSEDED
				|| createStatus == FILE_OVERWRITTEN
				)
			{
				action = Sensor::FileAction_FileModify;
				operation = WSTROP(Sensor::FileAction_FileModify);
				filePath = createRequest.filePath;
				isDir = createRequest.isDir;
			}
			// status indicating that file existed
			else if (createStatus == FILE_OPENED)
			{
				// only save these files to detect future modifications.
				// delete once CLEANUP or DELETE is received for that file.
				fileContext.PreExistingFileMap[createRequest.fileObject.address] = createRequest;
			}
			else {
				fileContext.CreateRequestMap.erase(irp.address);
				return;
			}
			
			// if any of the above statuses were returned then file was successfully renamed (if rename request
			// exists for the IRP).
			if (fileContext.RenameRequestMap.count(irp.address) > 0)
			{
				const TRACE_FILE_MAP_ENTRY& entry = fileContext.RenameRequestMap.at(irp.address);
				// validate that it was a rename, the names would differ
				if (createRequest.filePath != entry.filePath)
				{
					action = Sensor::FileAction_FileRename;
					operation = WSTROP(Sensor::FileAction_FileRename);
			
					filePath = entry.filePath;
					newFilePath = createRequest.filePath;
					isDir = createRequest.isDir;
				}
				fileContext.RenameRequestMap.erase(irp.address);
			}
			fileContext.CreateRequestMap.erase(irp.address);
		}
		break;

		case FILEIO_DELETE:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			krabs::pointer fileObject = parser.parse<krabs::pointer>(L"FileObject");

			const auto& ie = fileContext.ObjectInfoMap.at(fileObject.address);
			const std::wstring& fileName = ie.path;
			action = Sensor::FileAction_FileDelete;
			operation = WSTROP(Sensor::FileAction_FileDelete);
			filePath = fileName;
			isDir = ie.isDir;

			// if it has been deleted then it doesn't need to be in either
			fileContext.ModifiedFileMap.erase(fileObject.address);
			fileContext.PreExistingFileMap.erase(fileObject.address);
		}
		break;

		// we only care about file writes in the context of modifications to existing files.
		// all existing file opens are handled initially by FILEIO_CREATE. If not then file is not marked as modified
		case FILEIO_WRITE:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			krabs::pointer fileObject = parser.parse<krabs::pointer>(L"FileObject");
			krabs::pointer irp = parser.parse<krabs::pointer>(L"IrpPtr");

			// mark file as written
			TRACE_FILE_MAP_ENTRY openedFile = fileContext.PreExistingFileMap.at(fileObject.address);
			openedFile.irp = irp;
			fileContext.ModifiedFileMap[fileObject.address] = openedFile;
		}
		break;

		// should be sent after a FILEIO_WRITE event.
		case FILEIO_CLEANUP:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			krabs::pointer fileObject = parser.parse<krabs::pointer>(L"FileObject");
			krabs::pointer irp = parser.parse<krabs::pointer>(L"IrpPtr");

			const TRACE_FILE_MAP_ENTRY& openedFile = fileContext.ModifiedFileMap.at(fileObject.address);
			// if request was part of a modify request
			if (openedFile.irp.address == irp.address)
			{
				action = Sensor::FileAction_FileModify;
				operation = WSTROP(Sensor::FileAction_FileModify);
				filePath = openedFile.filePath;
				isDir = openedFile.isDir;
			}
			fileContext.ModifiedFileMap.erase(fileObject.address);
			fileContext.PreExistingFileMap.erase(fileObject.address);
		}
		break;

		// files created as part of rename requests share the same IRP.
		// in FILEIO_OPEND, we can look for files with that Irp in the RenameRequestMap
		// to determine whether it was created off of a rename.
		case FILEIO_RENAME:
		{
			std::lock_guard<std::shared_mutex> lock(mapLock);

			krabs::pointer fileObject = parser.parse<krabs::pointer>(L"FileObject");
			krabs::pointer irp = parser.parse<krabs::pointer>(L"IrpPtr");

			TRACE_FILE_MAP_ENTRY entry{};
			const auto& ie = fileContext.ObjectInfoMap.at(fileObject.address);
			entry.filePath = ie.path;
			entry.fileObject = fileObject;
			entry.irp = irp;
			entry.isDir = ie.isDir;

			fileContext.RenameRequestMap[irp.address] = entry;
			return;
		}
		break;
		default: return;
		}
	}
	catch (const std::out_of_range&)
	{
		return;
	}
	if (filePath.empty())
		return;

	flatbuffers::FlatBufferBuilder builder = MarshalFileEvent(
		action,
		filePath,
		newFilePath,
		operation,
		isDir);

	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_FILE_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(FILE_CATEGORY, __FUNCTIONT__, TEXT("Failed to report event: %d"), rc);
	}
}

void ProcessTraceCallback(const EVENT_RECORD&, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;
}

void NetworkTraceCallback(const EVENT_RECORD&, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;
}

void RegistryTraceCallback(const EVENT_RECORD&, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;
}

void ErrorCallback(const EVENT_RECORD& EventRecord, const std::string& strerr)
{
	std::wstring szProvider = std::to_wstring(EventRecord.EventHeader.ProviderId);
	std::wstring szError(strerr.begin(), strerr.end());

	LogError(MAIN_CATEGORY, __FUNCTIONT__, TEXT("Provider = %ls: %ls"), szProvider.c_str(), szError.c_str());
}
