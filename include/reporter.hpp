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

// reporter class for sending events back to alca client

#include <WinSock2.h>
#include <mutex>
#include "alca.fb.hpp"

#include "alca.h"
#include "log.hpp"

// Table of prefixes used when sending events.
// To add more event types, add a record to the table, which
// should include the module name (unique) and version.
// Use your prefix as the first argument to reporter::report_event.

#define GV_EVENT_FILE_PREFIX "file"    , AC_VERSION(0, 0, 1)
#define GV_EVENT_PROC_PREFIX "process" , AC_VERSION(0, 0, 1)
#define GV_EVENT_NET_PREFIX  "network" , AC_VERSION(0, 0, 1)
#define GV_EVENT_REG_PREFIX  "registry", AC_VERSION(0, 0, 1)

/*
	+------------+
	| version :4 | module version
	+------------+
	| etypelen:4 | event type length
	+------------+
	| typename:? | event type
	+------------+
	| evntdata:? | event data
	+------------+
*/

namespace governor {

	class reporter
	{
	public:
		reporter(SOCKET clientSocket, const int mode = AC_PACKET_LOCAL);
		int report_trace_status(int status, const uint8_t* data = nullptr, uint32_t data_size = 0);
		inline int report_event(
			const char* event_type,
			uint32_t event_version,
			const uint8_t* event_data,
			const int data_size);
		 inline void insert_int(std::vector<uint8_t>& buffer, uint32_t integer);
		~reporter() {}

	private:
		const SOCKET s;
		const int mode;
		std::mutex mu;
	};

	inline reporter::reporter(SOCKET cs, const int m)
		: s(cs)
		, mode(m)
	{}

	/// <summary>
	/// Report status of trace. Status can be one of AC_PACKET_DATA_TRACE_START,
	/// AC_PACKET_DATA_TRACE_STOP, or AC_PACKET_DTA_SUBMIT_ERROR.
	/// </summary>
	/// <param name="status">= status code</param>
	/// <returns>0 if successful</returns>
	inline int reporter::report_trace_status(int status, const uint8_t* data, uint32_t data_size)
	{
		std::lock_guard<std::mutex> lg(mu);
		LogVerbose(
			STATUS_SEVERITY_INFORMATIONAL,
			REPORTING_CATEGORY,
			TEXT("[REP] Reporting trace status (status = %d; size = %d)"), status, data_size
		);

		ac_packet_handle hPacket;
		if (ac_packet_create(mode, status, &hPacket) < 0)
		{
			return -1;
		}

		uint32_t packet_size = data_size;
		if (data_size > 0)
		{
			ac_packet_set_data(hPacket, data, data_size, AC_PACKET_SEQUENCE_LAST);
		}
		uint8_t* packet = ac_packet_serialize(hPacket, &packet_size);
		packet_size = netint(packet_size);
		if (send(s, reinterpret_cast<const char*>(&packet_size), sizeof(packet_size), 0) == SOCKET_ERROR)
		{
			ac_packet_destroy(hPacket);
			return WSAGetLastError();
		}
		if (send(s, reinterpret_cast<const char*>(packet), packet_size, 0) == SOCKET_ERROR)
		{
			ac_packet_destroy(hPacket);
			return WSAGetLastError();
		}
		ac_packet_destroy(hPacket);
		return 0;
	}

	inline void reporter::insert_int(std::vector<uint8_t>& buffer, uint32_t integer)
	{
		buffer.push_back( integer & 0xFF );
		buffer.push_back( (integer >> 8) & 0xFF );
		buffer.push_back( (integer >> 16) & 0xFF );
		buffer.push_back( (integer >> 24) & 0xFF );
	}

	/// <summary>
	/// Sends a serialised event to alca.
	/// </summary>
	/// <param name="event_type">= type of event</param>
	/// <param name="event_version">= event version</param>
	/// <param name="event_data">= event data serialised</param>
	/// <param name="data_size">= size of event data</param>
	/// <returns>0 if successful</returns>
	inline int reporter::report_event(
		const char *event_type,
		uint32_t event_version,
		const uint8_t* event_data,
		const int data_size)
	{
		std::lock_guard<std::mutex> lg(mu);
		LogVerbose(
			STATUS_SEVERITY_INFORMATIONAL,
			REPORTING_CATEGORY,
			TEXT("[REP] Reporting event (size = %d)"), data_size
		);
		ac_packet_handle hPacket;
		if (ac_packet_create(mode, AC_PACKET_DATA_EVENT, &hPacket) < 0)
		{
			return -1;
		}

		std::vector<uint8_t> v;
		uint32_t packet_size = 0;
		uint32_t event_type_version = netint(event_version);
		uint32_t event_type_len = netint((uint32_t)strlen(event_type) + 1);

		insert_int(v, event_type_version);
		insert_int(v, event_type_len);
		v.insert(v.end(), event_type, event_type + event_type_len);
		v.insert(v.end(), event_data, event_data + data_size);

		if (ac_packet_set_data(
			hPacket,
			v.data(),
			(uint32_t)v.size(),
			AC_PACKET_SEQUENCE_LAST
		) != 0)
		{
			ac_packet_destroy(hPacket);
			return -1;
		}

		const uint8_t* packet = ac_packet_serialize(hPacket, &packet_size);
		
		packet_size = netint(packet_size);
		if (send(s, reinterpret_cast<const char*>(&packet_size), sizeof(packet_size), 0) == SOCKET_ERROR)
		{
			ac_packet_destroy(hPacket);
			return WSAGetLastError();
		}
		if (send(s, reinterpret_cast<const char*>(packet), packet_size, 0) == SOCKET_ERROR)
		{
			ac_packet_destroy(hPacket);
			return WSAGetLastError();
		}
		ac_packet_destroy(hPacket);
		return 0;
	}
}
