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
#include "handler.hpp"

#define SETUP_PROVIDER(trace, provider, callback) \
	krabs::kernel::provider provider; \
	krabs::event_filter provider##_filter(krabs::predicates::process_id_is(TraceContext->pi.dwProcessId)); \
	provider##_filter.add_on_event_callback(callback); \
	provider.add_filter(provider##_filter); \
	provider.add_on_error_callback(ErrorCallback); \
	trace->enable(provider); \

DWORD GvTraceStart(LPVOID lpParameter)
{
	auto TraceContext = static_cast<PGV_TRACE_CONTEXT>(lpParameter);

	/* disk_io_provider must be enabled for EVENT_TRACE_FLAG_DISK_FILE_IO events.
	 https://learn.microsoft.com/en-us/windows/win32/etw/fileio#remarks.
	 We don't need a callback to handle its events though.*/
	krabs::kernel::disk_io_provider disk_io_provider;

	EVENT_TRACE_PROPERTIES properties{ 0 };
	properties.BufferSize = 64;
	properties.MinimumBuffers = 8;
	properties.MaximumBuffers = 32;
	properties.FlushTimer = 1;

	TraceContext->Trace->set_trace_properties(&properties);

	TraceContext->Trace->enable(disk_io_provider);

	// why these geniuses wouldn't add a trace-wide filtering mechanism is beyond me
	SETUP_PROVIDER(TraceContext->Trace, process_provider, ProcessTraceCallback);
	SETUP_PROVIDER(TraceContext->Trace, thread_provider, ProcessTraceCallback);
	SETUP_PROVIDER(TraceContext->Trace, image_load_provider, ProcessTraceCallback);
	SETUP_PROVIDER(TraceContext->Trace, virtual_alloc_provider, ProcessTraceCallback);

	SETUP_PROVIDER(TraceContext->Trace, file_io_provider, FileTraceCallback);
	SETUP_PROVIDER(TraceContext->Trace, file_init_io_provider, FileTraceCallback);
	SETUP_PROVIDER(TraceContext->Trace, disk_file_io_provider, FileTraceCallback);

	SETUP_PROVIDER(TraceContext->Trace, network_tcpip_provider, NetworkTraceCallback);

	SETUP_PROVIDER(TraceContext->Trace, registry_provider, RegistryTraceCallback);

	if (ResumeThread(TraceContext->pi.hThread) == -1)
	{
		return GetLastError();
	}
	TraceContext->Trace->start();
	return ERROR_SUCCESS;
}
