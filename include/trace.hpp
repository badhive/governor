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

#pragma once

#include <iostream>
#include <shared_mutex>
#include "krabs.hpp"
#include "reporter.hpp"

typedef struct _TRACE_FILE_MAP_ENTRY
{
	std::wstring filePath;
	krabs::pointer fileObject;
	krabs::pointer irp;
	bool isDir;
} TRACE_FILE_MAP_ENTRY, * PTRACE_FILE_MAP_ENTRY;

typedef struct _TRACE_FILE_INFO_ENTRY
{
	std::wstring path;
	bool isDir;
} TRACE_FILE_INFO_ENTRY, * PTRACE_FILE_INFO_ENTRY;

typedef struct _GV_EVENT_CONTEXT
{
	governor::reporter *reporter;
	const PROCESS_INFORMATION& pi;
	std::shared_mutex mapLock;
	struct {
		std::map<ULONG_PTR, TRACE_FILE_MAP_ENTRY> CreateRequestMap;
		std::map<ULONG_PTR, TRACE_FILE_MAP_ENTRY> RenameRequestMap;
		std::map<ULONG_PTR, TRACE_FILE_MAP_ENTRY> PreExistingFileMap;
		std::map<ULONG_PTR, TRACE_FILE_MAP_ENTRY> ModifiedFileMap;
		std::map<ULONG_PTR, TRACE_FILE_INFO_ENTRY> ObjectInfoMap;
	} File;
	struct {
		std::map<ULONG_PTR, std::wstring> RegistryHandleKeyMap;
	} Registry;
} GV_EVENT_CONTEXT, * PGV_EVENT_CONTEXT;

typedef struct _GV_TRACE_CONTEXT
{
	const PROCESS_INFORMATION& pi;
	SOCKET client_socket;
	krabs::kernel_trace *trace;
} GV_TRACE_CONTEXT, * PGV_TRACE_CONTEXT;

DWORD GvTraceStart(LPVOID);
