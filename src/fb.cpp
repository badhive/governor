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

#include "fb.hpp"
#include "log.hpp"
#include "utils.hpp"

void MarshalFileEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::FileAction action,
	std::wstring fileName,
	std::wstring newFileName,
	std::wstring opcodeName,
	bool fileIsDir
)
{
	std::string newFileNameA(newFileName.begin(), newFileName.end());
	auto fsPath = std::filesystem::path(fileName);
	std::string name = fsPath.filename().string();
	std::string dir = fsPath.parent_path().string();
	std::string ext = fsPath.extension().string();

	flatbuffers::Offset<flatbuffers::String> fbPath = builder.CreateString(fsPath.string());
	flatbuffers::Offset<flatbuffers::String> fbName = builder.CreateString(name);
	flatbuffers::Offset<flatbuffers::String> fbDir = builder.CreateString(dir);
	flatbuffers::Offset<flatbuffers::String> fbExt = builder.CreateString(ext);
	flatbuffers::Offset<flatbuffers::String> fbNewName = builder.CreateString(newFileNameA);

	bool fbIsDir = fileIsDir;

	LogWrite(
		STATUS_SEVERITY_INFORMATIONAL,
		FILE_CATEGORY,
		TEXT("Received file event details:\n")
		TEXT("	operation = %ls\n")
		TEXT("	action    = %d\n")
		TEXT("	path      = %ls\n")
		TEXT("	new_name  = %ls\n")
		TEXT("	is_dir    = %ls\n"),
		opcodeName.c_str(),
		action,
		fileName.c_str(),
		newFileName.c_str(),
		fbIsDir ? L"yes" : L"no"
	);
	
	flatbuffers::Offset<Sensor::FileEvent> evt =
		Sensor::CreateFileEvent(
			builder,
			action,
			fbName,
			fbPath,
			fbDir,
			fbExt,
			fbNewName,
			fbIsDir
		);

	builder.Finish(evt);
	return;
}

void MarshalProcessEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::ProcessAction action,
	std::wstring opcodeName,
	DWORD dwProcessId,
	DWORD dwSelfProcessId,
	std::string processName,
	std::wstring commandLine,
	std::wstring imageName
)
{
	std::string commandLineA(commandLine.begin(), commandLine.end());
	std::string imageNameA(imageName.begin(), imageName.end());
	
	flatbuffers::Offset<flatbuffers::String> fbName = builder.CreateString(processName);
	flatbuffers::Offset<flatbuffers::String> fbCommandLine = builder.CreateString(commandLineA);
	flatbuffers::Offset<flatbuffers::String> fbImageName = builder.CreateString(imageNameA);

	std::wstring processNameW(processName.begin(), processName.end());

	LPCTSTR format = TEXT("Received process event details:\n")
		TEXT("	operation = %ls\n")
		TEXT("	action    = %d\n")
		TEXT("	name      = %ls\n")
		TEXT("	cmdline   = %ls\n")
		TEXT("	imageName = %ls\n")
		TEXT("	pid       = %d\n");

	if (action == Sensor::ProcessAction_AllocLocal
		|| action == Sensor::ProcessAction_CreateLocalThread
		|| action == Sensor::ProcessAction_TerminateLocalThread
		|| action == Sensor::ProcessAction_ImageLoad
		|| action == Sensor::ProcessAction_ImageUnload) // all are noisy, only log in verbose mode
	{
		LogVerbose(
			STATUS_SEVERITY_INFORMATIONAL,
			FILE_CATEGORY,
			format,
			opcodeName.c_str(),
			action,
			processNameW.c_str(),
			commandLine.c_str(),
			imageName.c_str(),
			dwProcessId
		);
	}
	else
	{
		LogWrite(
			STATUS_SEVERITY_INFORMATIONAL,
			FILE_CATEGORY,
			format,
			opcodeName.c_str(),
			action,
			processNameW.c_str(),
			commandLine.c_str(),
			imageName.c_str(),
			dwProcessId
		);
	}

	flatbuffers::Offset<Sensor::ProcessEvent> evt =
		Sensor::CreateProcessEvent(
			builder,
			action,
			dwProcessId,
			fbName,
			dwSelfProcessId,
			fbCommandLine,
			fbImageName
		);
	builder.Finish(evt);
	return;
}

void MarshalNetworkEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::NetworkAction action,
	std::wstring opcodeName,
	bool tcp,
	bool udp,
	bool ipv6,
	std::string localAddr,
	u_short localPort,
	std::string remoteAddr,
	u_short remotePort,
	uint32_t size
)
{
	flatbuffers::Offset<flatbuffers::String> fbLocalAddr = builder.CreateString(localAddr);
	flatbuffers::Offset<flatbuffers::String> fbRemoteAddr = builder.CreateString(remoteAddr);

	std::wstring localAddrW(localAddr.begin(), localAddr.end());
	std::wstring remoteAddrW(remoteAddr.begin(), remoteAddr.end());

	LogWrite(
		STATUS_SEVERITY_INFORMATIONAL,
		FILE_CATEGORY,
		TEXT("Received network event details:\n")
		TEXT("	operation    = %ls\n")
		TEXT("	action       = %d\n")
		TEXT("	local_addr   = %ls\n")
		TEXT("	local_port   = %d\n")
		TEXT("	remote_addr  = %ls\n")
		TEXT("	remote_port  = %d\n")
		TEXT("	size         = %d\n"),
		opcodeName.c_str(),
		action,
		localAddrW.c_str(),
		localPort,
		remoteAddrW.c_str(),
		remotePort,
		size
	);

	flatbuffers::Offset<Sensor::NetworkEvent> evt =
		Sensor::CreateNetworkEvent(
			builder,
			action,
			tcp,
			udp,
			ipv6,
			fbLocalAddr,
			localPort,
			fbRemoteAddr,
			remotePort,
			size
		);
	builder.Finish(evt);
	return;
}

void MarshalRegistryEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::RegistryAction action,
	std::wstring opcodeName,
	std::wstring keyPath,
	std::wstring valueName
)
{
	std::string keyPathA(keyPath.begin(), keyPath.end());
	std::string valueNameA(valueName.begin(), valueName.end());

	flatbuffers::Offset<flatbuffers::String> fbKeyPath = builder.CreateString(keyPathA);
	flatbuffers::Offset<flatbuffers::String> fbValueName = builder.CreateString(valueNameA);

	LogWrite(
		STATUS_SEVERITY_INFORMATIONAL,
		FILE_CATEGORY,
		TEXT("Received registry event details:\n")
		TEXT("	operation    = %ls\n")
		TEXT("	action       = %d\n")
		TEXT("	keyPath      = %ls\n")
		TEXT("	valueName    = %ls\n"),
		opcodeName.c_str(),
		action,
		keyPath.c_str(),
		valueName.c_str()
	);

	flatbuffers::Offset<Sensor::RegistryEvent> evt =
		Sensor::CreateRegistryEvent(
			builder,
			action,
			fbKeyPath,
			fbValueName
		);
	builder.Finish(evt);
	return;
}
