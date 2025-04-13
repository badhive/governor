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

#include "krabs.hpp"
#include "win32.h"

// FILEIO_CREATE is for NtCreateFile calls. It's not necessarily for new files.
// This means that when this event is received, we don't actually send any event data to
// alca, but we update fileKeyNameMap for future name lookups in other events.
// https://learn.microsoft.com/en-us/windows/win32/etw/fileio-create

// FileTraceCallback
#define FILEIO_CREATE 64
#define FILEIO_CLEANUP 65
#define FILEIO_READ 67
#define FILEIO_WRITE 68
#define FILEIO_DELETE 70
#define FILEIO_RENAME 71
#define FILEIO_OPEND 76

// ProcessTraceCallback
#define PROCESS_START 1
#define PROCESS_END 2
#define MEM_VALLOC 98

// ThreadTraceCallback
#define THREAD_START 1
#define THREAD_END 2

// ImageTraceCallback
#define IMAGE_LOAD 10
#define IMAGE_UNLOAD 2

// NetworkTraceCallback
#define SEND_IPV6 26
#define RECV_IPV6 27
#define CONNECT_IPV6 28
#define DISCONNECT_IPV6 29
#define ACCEPT_IPV6 31

void FileTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void ProcessTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void ThreadTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void ImageTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void NetworkTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void RegistryTraceCallback(const EVENT_RECORD&, const krabs::trace_context&);

void ErrorCallback(const EVENT_RECORD&, const std::string&);
