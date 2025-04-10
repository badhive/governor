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

#include <filesystem>

#include "alca.fb.hpp"
#include "log.hpp"

flatbuffers::FlatBufferBuilder MarshalFileEvent(
	Sensor::FileAction action,
	std::wstring fileName,
	std::wstring newFileName,
	std::wstring opcodeName
)
{
	std::string newFileNameA(newFileName.begin(), newFileName.end());
	auto fsPath = std::filesystem::path(fileName);
	std::string name = fsPath.filename().string();
	std::string dir = fsPath.parent_path().string();
	std::string ext = fsPath.extension().string();

	flatbuffers::FlatBufferBuilder builder(1024);
	flatbuffers::Offset<flatbuffers::String> fbPath = builder.CreateString(fsPath.string());
	flatbuffers::Offset<flatbuffers::String> fbName = builder.CreateString(name);
	flatbuffers::Offset<flatbuffers::String> fbDir = builder.CreateString(dir);
	flatbuffers::Offset<flatbuffers::String> fbExt = builder.CreateString(ext);
	flatbuffers::Offset<flatbuffers::String> fbNewName = builder.CreateString(newFileNameA);

	LogWrite(
		STATUS_SEVERITY_INFORMATIONAL,
		FILE_CATEGORY,
		TEXT("Received event details:\n")
		TEXT("	operation = %ls\n")
		TEXT("	action    = %d\n")
		TEXT("	path      = %ls\n")
		TEXT("	new_name  = %ls\n"),
		opcodeName.c_str(),
		action,
		fileName.c_str(),
		newFileName.c_str()
	);

	flatbuffers::Offset<Sensor::FileEvent> evt =
		Sensor::CreateFileEvent(
			builder,
			action,
			fbName,
			fbPath,
			fbDir,
			fbExt,
			0,
			0,
			0,
			0,
			0,
			0,
			fbNewName
		);

	builder.Finish(evt);
	return builder;
}
