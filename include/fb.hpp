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

#include <windows.h>
#include "alca.fb.hpp"

void MarshalFileEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::FileAction action,
	std::wstring fileName,
	std::wstring newFileName,
	std::wstring opcodeName,
	bool fileIsDir
);

void MarshalProcessEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::ProcessAction action,
	std::wstring opcodeName,
	DWORD dwProcessId,
	DWORD dwSelfProcessId,
	std::string processName,
	std::wstring commandLine,
	std::wstring imageName
);

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
);

void MarshalRegistryEvent(
	flatbuffers::FlatBufferBuilder& builder,
	Sensor::RegistryAction action,
	std::wstring opcodeName,
	std::wstring keyPath,
	std::wstring valueName
);