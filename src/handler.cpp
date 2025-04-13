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

#include "trace.hpp"
#include <filesystem>
#include <tchar.h>
#include <psapi.h>

#include "fb.hpp"
#include "handler.hpp"
#include "log.hpp"
#include "reporter.hpp"
#include "utils.hpp"

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
	
	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalFileEvent(
		builder,
		action,
		filePath,
		newFilePath,
		operation,
		isDir);

	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_FILE_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(FILE_CATEGORY, __FUNCTIONT__, TEXT("Failed to report file event: %d"), rc);
	}
}

void ProcessTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);

	std::wstring operation;
	int opcode = schema.event_opcode();

	DWORD selfPid = userContext->pi.dwProcessId;
	DWORD pid = 0;
	std::wstring cmdLine;
	std::string processName;
	std::wstring imageName; // empty
	std::wstring opcodeName;

	Sensor::ProcessAction action = Sensor::ProcessAction_MIN;
	switch (opcode)
	{
	case PROCESS_START:
	{
		action = Sensor::ProcessAction_ProcessStart;
		opcodeName = WSTROP(Sensor::ProcessAction_ProcessStart);
		pid = parser.parse<uint32_t>(L"ProcessId");
		processName = parser.parse<std::string>(L"ImageFileName");
		cmdLine = parser.parse<std::wstring>(L"CommandLine");
	}
	break;
	case PROCESS_END:
	{
		action = Sensor::ProcessAction_ProcessEnd;
		opcodeName = WSTROP(Sensor::ProcessAction_ProcessEnd);
		pid = parser.parse<uint32_t>(L"ProcessId");
		processName = parser.parse<std::string>(L"ImageFileName");
		cmdLine = parser.parse<std::wstring>(L"CommandLine");
	}
	break;
	case MEM_VALLOC:
	{
		action = Sensor::ProcessAction_AllocLocal;
		opcodeName = WSTROP(Sensor::ProcessAction_AllocLocal);
		pid = parser.parse<uint32_t>(L"ProcessId");
		if (pid != selfPid)
		{
			action = Sensor::ProcessAction_AllocRemote;
			opcodeName = WSTROP(Sensor::ProcessAction_AllocRemote);
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
			if (hProcess)
			{
				std::wstring imageNameW;
				WCHAR ProcessImageName[18 + MAX_PATH]; // device path format
				if (GetProcessImageFileNameW(hProcess, ProcessImageName, 18 + MAX_PATH) != 0)
				{
					imageNameW = ProcessImageName;
					processName.assign(imageNameW.begin(), imageNameW.end());
				}
				PEB peb{};
				DWORD status = GetProcessPEB(hProcess, &peb);
				if (status == 0)
				{
					if (peb.ProcessParameters != NULL)
						cmdLine = peb.ProcessParameters->CommandLine.Buffer;
				}
				else
				{
					LogError(
						PROCESS_CATEGORY,
						__FUNCTIONT__,
						TEXT("failed to get PEB from %ls: %d"),
						imageNameW.c_str(),
						status);
				}
				CloseHandle(hProcess);
			}
			else
			{
				DWORD status = GetLastError();
				// only if process has ended before we tried to query it
				if (status != ERROR_INVALID_PARAMETER)
					LogSystemError(PROCESS_CATEGORY, __FUNCTIONT__, status);
			}
		}	
		break;
	}
	default: return;
	}
	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalProcessEvent(
		builder,
		action,
		opcodeName,
		pid,
		selfPid,
		processName,
		cmdLine,
		imageName
	);
	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_PROC_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(PROCESS_CATEGORY, __FUNCTIONT__, TEXT("Failed to report process event: %d"), rc);
	}
}

void ThreadTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);

	std::wstring operation;
	int opcode = schema.event_opcode();

	DWORD selfPid = userContext->pi.dwProcessId;
	DWORD pid = 0;
	std::wstring cmdLine;
	std::string processName;
	std::wstring imageName; // empty
	std::wstring opcodeName;
	Sensor::ProcessAction action = Sensor::ProcessAction_MIN;

	switch (opcode)
	{
	case THREAD_START:
	{
		pid = parser.parse<uint32_t>(L"ProcessId");
		if (pid != selfPid)
		{
			action = Sensor::ProcessAction_CreateRemoteThread;
			opcodeName = WSTROP(Sensor::ProcessAction_CreateRemoteThread);
		}
		else
		{
			action = Sensor::ProcessAction_CreateLocalThread;
			opcodeName = WSTROP(Sensor::ProcessAction_CreateLocalThread);
		}
	}
	break;
	case THREAD_END:
	{
		pid = parser.parse<uint32_t>(L"ProcessId");
		if (pid != selfPid)
		{
			action = Sensor::ProcessAction_TerminateRemoteThread;
			opcodeName = WSTROP(Sensor::ProcessAction_TerminateRemoteThread);
		}
		else
		{
			action = Sensor::ProcessAction_TerminateLocalThread;
			opcodeName = WSTROP(Sensor::ProcessAction_TerminateLocalThread);
		}
	}
	break;
	default: return;
	}
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		WCHAR ProcessImageName[18 + MAX_PATH]; // device path format
		if (GetProcessImageFileNameW(hProcess, ProcessImageName, 18 + MAX_PATH) != 0)
		{
			std::wstring imageNameW = ProcessImageName;
			processName.assign(imageNameW.begin(), imageNameW.end());
		}
		PEB peb{};
		DWORD status = GetProcessPEB(hProcess, &peb);
		if (status == 0)
		{
			if (peb.ProcessParameters != NULL)
				cmdLine = peb.ProcessParameters->CommandLine.Buffer;
		}
		CloseHandle(hProcess);
	}

	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalProcessEvent(
		builder,
		action,
		opcodeName,
		pid,
		selfPid,
		processName,
		cmdLine,
		imageName
	);
	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_PROC_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(PROCESS_CATEGORY, __FUNCTIONT__, TEXT("Failed to report process[thread] event: %d"), rc);
	}
}

void ImageTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);

	std::wstring operation;
	int opcode = schema.event_opcode();

	DWORD selfPid = userContext->pi.dwProcessId;
	DWORD pid = 0;
	std::wstring cmdLine;
	std::string processName;
	std::wstring imageName;
	std::wstring opcodeName;
	Sensor::ProcessAction action = Sensor::ProcessAction_MIN;

	switch (opcode)
	{
	case IMAGE_LOAD:
	{
		pid = parser.parse<uint32_t>(L"ProcessId");
		imageName = parser.parse<std::wstring>(L"FileName");
		action = Sensor::ProcessAction_ImageLoad;
		opcodeName = WSTROP(Sensor::ProcessAction_ImageLoad);
	}
	break;
	case IMAGE_UNLOAD:
	{
		pid = parser.parse<uint32_t>(L"ProcessId");
		imageName = parser.parse<std::wstring>(L"FileName");
		action = Sensor::ProcessAction_ImageUnload;
		opcodeName = WSTROP(Sensor::ProcessAction_ImageUnload);
	}
	break;
	default: return;
	}
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (hProcess)
	{
		WCHAR ProcessImageName[18 + MAX_PATH]; // device path format
		if (GetProcessImageFileNameW(hProcess, ProcessImageName, 18 + MAX_PATH) != 0)
		{
			std::wstring imageNameW = ProcessImageName;
			processName.assign(imageNameW.begin(), imageNameW.end());
		}
		PEB peb{};
		DWORD status = GetProcessPEB(hProcess, &peb);
		if (status == 0)
		{
			if (peb.ProcessParameters != NULL)
				cmdLine = peb.ProcessParameters->CommandLine.Buffer;
		}
		CloseHandle(hProcess);
	}

	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalProcessEvent(
		builder,
		action,
		opcodeName,
		pid,
		selfPid,
		processName,
		cmdLine,
		imageName
	);
	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_PROC_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(PROCESS_CATEGORY, __FUNCTIONT__, TEXT("Failed to report process[image] event: %d"), rc);
	}
}

void NetworkTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);

	std::wstring operation;
	int opcode = schema.event_opcode();
	
	Sensor::NetworkAction action = Sensor::NetworkAction_MIN;
	bool tcp = true;
	bool udp = false;
	bool ipv6 = false;
	std::string sAddr;
	krabs::ip_address sAddrStruct;
	u_short sPort;

	std::string dAddr;
	krabs::ip_address dAddrStruct;
	u_short dPort;

	uint32_t size = 0;

	switch (opcode)
	{
	case ACCEPT_IPV6:
		ipv6 = true;
		[[fallthrough]];
	case EVENT_TRACE_TYPE_ACCEPT:
	{
		action = Sensor::NetworkAction_NetAccept;
		operation = WSTROP(Sensor::NetworkAction_NetAccept);
		dAddrStruct = parser.parse<krabs::ip_address>(L"daddr");
		dPort = parser.parse<u_short>(L"dport");

		sAddrStruct = parser.parse<krabs::ip_address>(L"saddr");
		sPort = parser.parse<u_short>(L"sport");
	}
	break;
	case CONNECT_IPV6:
		ipv6 = true;
		[[fallthrough]];
	case EVENT_TRACE_TYPE_CONNECT:
	{
		action = Sensor::NetworkAction_NetConnect;
		operation = WSTROP(Sensor::NetworkAction_NetConnect);
		dAddrStruct = parser.parse<krabs::ip_address>(L"daddr");
		dPort = parser.parse<u_short>(L"dport");

		sAddrStruct = parser.parse<krabs::ip_address>(L"saddr");
		sPort = parser.parse<u_short>(L"sport");
	}
	break;
	case DISCONNECT_IPV6:
		ipv6 = true;
		[[fallthrough]];
	case EVENT_TRACE_TYPE_DISCONNECT:
	{
		action = Sensor::NetworkAction_NetDisconnect;
		operation = WSTROP(Sensor::NetworkAction_NetDisconnect);
		dAddrStruct = parser.parse<krabs::ip_address>(L"daddr");
		dPort = parser.parse<u_short>(L"dport");

		sAddrStruct = parser.parse<krabs::ip_address>(L"saddr");
		sPort = parser.parse<u_short>(L"sport");
	}
	break;
	case SEND_IPV6:
		ipv6 = true;
		[[fallthrough]];
	case EVENT_TRACE_TYPE_SEND:
	{
		action = Sensor::NetworkAction_NetSend;
		operation = WSTROP(Sensor::NetworkAction_NetSend);
		dAddrStruct = parser.parse<krabs::ip_address>(L"daddr");
		dPort = parser.parse<u_short>(L"dport");

		sAddrStruct = parser.parse<krabs::ip_address>(L"saddr");
		sPort = parser.parse<u_short>(L"sport");
		size = parser.parse<uint32_t>(L"size");
	}
	break;
	case RECV_IPV6:
		ipv6 = true;
		[[fallthrough]];
	case EVENT_TRACE_TYPE_RECEIVE:
	{
		action = Sensor::NetworkAction_NetReceive;
		operation = WSTROP(Sensor::NetworkAction_NetReceive);
		dAddrStruct = parser.parse<krabs::ip_address>(L"daddr");
		dPort = parser.parse<u_short>(L"dport");

		sAddrStruct = parser.parse<krabs::ip_address>(L"saddr");
		sPort = parser.parse<u_short>(L"sport");
		size = parser.parse<uint32_t>(L"size");
	}
	break;
	default: return;
	}
	if (ipv6)
	{
		IN6_ADDR addr{ 0 };
		char addrBuf[46];
		memcpy(addr.u.Byte, sAddrStruct.v6, sizeof(addr.u.Byte));
		sAddr = inet_ntop(AF_INET6, &addr, addrBuf, sizeof(addrBuf));

		memcpy(addr.u.Byte, dAddrStruct.v6, sizeof(addr.u.Byte));
		dAddr = inet_ntop(AF_INET6, &addr, addrBuf, sizeof(addrBuf));
	}
	else
	{
		IN_ADDR addr{ 0 };
		char addrBuf[16];
		addr.S_un.S_addr = sAddrStruct.v4;
		sAddr = inet_ntop(AF_INET, &addr, addrBuf, sizeof(addrBuf));

		addr.S_un.S_addr = dAddrStruct.v4;
		dAddr = inet_ntop(AF_INET, &addr, addrBuf, sizeof(addrBuf));
	}

	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalNetworkEvent(
		builder,
		action,
		operation,
		tcp,
		udp,
		ipv6,
		sAddr,
		sPort,
		dAddr,
		dPort,
		size
	);
	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_NET_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(PROCESS_CATEGORY, __FUNCTIONT__, TEXT("Failed to report network event: %d"), rc);
	}
}

void RegistryTraceCallback(const EVENT_RECORD& EventRecord, const krabs::trace_context& ctx)
{
	auto* userContext = (GV_EVENT_CONTEXT*)(ctx.user_context);
	auto& mapLock = userContext->mapLock;
	auto* reporter = userContext->reporter;
	auto& regContext = userContext->Registry;

	krabs::schema schema(EventRecord, ctx.schema_locator);
	krabs::parser parser(schema);

	std::wstring operation;
	int opcode = schema.event_opcode();
	std::lock_guard<std::shared_mutex> lock(mapLock);

	Sensor::RegistryAction action = Sensor::RegistryAction_MIN;

	std::wstring keyPath;
	std::wstring valueName;
	ULONG_PTR keyHandle;

	std::wstring keyName = parser.parse<std::wstring>(L"KeyName");

	switch (opcode)
	{
	// creating subkey event
	case EVENT_TRACE_TYPE_REGCREATE:
	{
		action = Sensor::RegistryAction_RegOpenKey;
		operation = WSTROP(Sensor::RegistryAction_RegOpenKey);
		keyPath = keyName;
	}
	break;
	case EVENT_TRACE_TYPE_REGKCBCREATE:
	{
		action = Sensor::RegistryAction_RegCreateKey;
		operation = WSTROP(Sensor::RegistryAction_RegCreateKey);
		keyPath = keyName;
	}
	break;
	case EVENT_TRACE_TYPE_REGKCBDELETE:
	{
		action = Sensor::RegistryAction_RegDeleteKey;
		operation = WSTROP(Sensor::RegistryAction_RegDeleteKey);
		keyPath = keyName;
	}
	break;
	case EVENT_TRACE_TYPE_REGSETVALUE:
	{
		action = Sensor::RegistryAction_RegSetValue;
		operation = WSTROP(Sensor::RegistryAction_RegSetValue);
		valueName = keyName;
	}
	break;
	case EVENT_TRACE_TYPE_REGDELETEVALUE:
	{
		action = Sensor::RegistryAction_RegDeleteValue;
		operation = WSTROP(Sensor::RegistryAction_RegDeleteValue);
		valueName = keyName;
	}
	break;
	case EVENT_TRACE_TYPE_REGFLUSH:
	{
		break;
	}
	default: return;
	}
	
	flatbuffers::FlatBufferBuilder builder(1024);
	MarshalRegistryEvent(builder, action, operation, keyPath, valueName);
	const uint8_t* event_data = builder.GetBufferPointer();
	int rc;
	if ((rc = reporter->report_event(GV_EVENT_REG_PREFIX, event_data, builder.GetSize())) != 0)
	{
		LogError(PROCESS_CATEGORY, __FUNCTIONT__, TEXT("Failed to report registry event: %d"), rc);
	}
}

void ErrorCallback(const EVENT_RECORD& EventRecord, const std::string& strerr)
{
	std::wstring szError(strerr.begin(), strerr.end());

	// errors here usually aren't serious so we only log as verbose
	LogVerbose(STATUS_SEVERITY_ERROR, MAIN_CATEGORY, TEXT("%ls"), szError.c_str());
}
