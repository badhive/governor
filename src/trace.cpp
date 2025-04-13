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
	provider##_filter.add_on_error_callback(ErrorCallback); \
	provider.add_filter(provider##_filter); \
	trace->enable(provider); \

VOID GvTraceSetupNetworkProvider(PGV_TRACE_CONTEXT TraceContext, krabs::kernel::network_tcpip_provider& provider)
{
	// TCP-IP provider needs to be set up separately: a lot of work is done in a separate thread so we
	// can't filter by PID. We need to get the PID by parsing the event. Most TCP-IP events have a PID member.
	provider.add_on_event_callback([](const EVENT_RECORD& record, const krabs::trace_context& trace_context) {
		krabs::schema schema(record, trace_context.schema_locator);
		auto* userContext = (PGV_EVENT_CONTEXT)trace_context.user_context;
		if (schema.event_opcode() != 17) // TcpIp_Fail doesn't have PID member, we don't care about it regardless
		{
			krabs::parser parser(schema);
			uint32_t pid = parser.parse<uint32_t>(L"PID");
			if (pid == userContext->pi.dwProcessId)
				NetworkTraceCallback(record, trace_context);
		}
		});
	TraceContext->trace->enable(provider);
}

DWORD GvTraceStart(LPVOID lpParameter)
{
	auto TraceContext = static_cast<PGV_TRACE_CONTEXT>(lpParameter);

	/* disk_io_provider must be enabled for EVENT_TRACE_FLAG_DISK_FILE_IO events.
	 https://learn.microsoft.com/en-us/windows/win32/etw/fileio#remarks.
	 We don't need a callback to handle its events though.*/
	krabs::kernel::disk_io_provider disk_io_provider;
	krabs::kernel::network_tcpip_provider network_tcpip_provider;

	EVENT_TRACE_PROPERTIES properties{ 0 };
	properties.BufferSize = 64;
	properties.MinimumBuffers = 8;
	properties.MaximumBuffers = 32;
	properties.FlushTimer = 1;

	TraceContext->trace->set_trace_properties(&properties);

	TraceContext->trace->enable(disk_io_provider);
	GvTraceSetupNetworkProvider(TraceContext, network_tcpip_provider);

	// why these geniuses wouldn't add a trace-wide filtering mechanism is beyond me
	SETUP_PROVIDER(TraceContext->trace, process_provider, ProcessTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, virtual_alloc_provider, ProcessTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, thread_provider, ThreadTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, image_load_provider, ImageTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, file_io_provider, FileTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, file_init_io_provider, FileTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, disk_file_io_provider, FileTraceCallback);
	SETUP_PROVIDER(TraceContext->trace, registry_provider, RegistryTraceCallback);

	if (ResumeThread(TraceContext->pi.hThread) == -1)
	{
		return GetLastError();
	}
	TraceContext->trace->start();
	return ERROR_SUCCESS;
}
