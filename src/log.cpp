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

#include <vector>

#include <stdarg.h>
#include <tchar.h>

#include "log.hpp"
#include "utils.hpp"

HANDLE g_hEventLog;
BOOL g_bConsole;
BOOL g_bVerbose = FALSE;

BOOL LogInit(BOOL isConsole)
{
	g_bConsole = isConsole;
	if (g_hEventLog)
	{
		return TRUE;
	}
	g_hEventLog = RegisterEventSource(NULL, GV_SVCNAME);
	if (!g_hEventLog)
	{
		return FALSE;
	}
	return TRUE;
}

VOID LogSetVerbose(BOOL bVerbose)
{
	g_bVerbose = bVerbose;
}

VOID LogSystemError(WORD wCategory, LPCTSTR szFunction, DWORD dwError)
{
	LPTSTR lpMessageBuffer = NULL;
	DWORD dwMessageSize;
	LPCTSTR pInsertStrings[2] = { NULL, NULL };
	if (!dwError)
		return;

	if (g_hEventLog)
	{
		dwMessageSize = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			0,
			(LPTSTR) & lpMessageBuffer,
			0,
			NULL
		);
		if (dwMessageSize == 0)
			return;
		
		if (g_bConsole)
		{
			_tprintf(TEXT("[SYS] error (%s): %s\n"), szFunction, lpMessageBuffer);
			LocalFree(lpMessageBuffer);
			return;
		}
		pInsertStrings[0] = szFunction;
		pInsertStrings[1] = lpMessageBuffer;
		ReportEvent(
			g_hEventLog,
			EVENTLOG_ERROR_TYPE,
			wCategory,
			MSG_SYSTEM_ERROR,
			NULL,
			2,
			0,
			(LPCTSTR*)pInsertStrings,
			NULL
		);
		LocalFree(lpMessageBuffer);
	}
}

static BOOL LogCreateMessage(LPCTSTR szFormat, va_list args, LPTSTR *szMessage)
{
	LPTSTR lpMessageBuffer;
	DWORD dwMessageSize;
	va_list args2;
	va_copy(args2, args);
	dwMessageSize = _vsctprintf(
		szFormat,
		args
	) + 1;

	lpMessageBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwMessageSize * sizeof(TCHAR));
	if (!lpMessageBuffer)
	{
		return FALSE;
	}
	_vsntprintf_s(
		lpMessageBuffer,
		dwMessageSize,
		_TRUNCATE,
		szFormat,
		args2
	);
	va_end(args2);

	*szMessage = lpMessageBuffer;
	return TRUE;
}

static VOID LogFreeMessage(LPTSTR szMessage)
{
	if (szMessage)
		HeapFree(GetProcessHeap(), 0, szMessage);
}

VOID LogError(WORD wCategory, LPCTSTR szFunction, LPCTSTR szFormat, ...)
{
	LPTSTR lpMessageBuffer = NULL;
	LPCTSTR pInsertStrings[2] = { NULL, NULL };
	if (g_hEventLog)
	{
		va_list args;
		va_start(args, szFormat);
		if (!LogCreateMessage(szFormat, args, &lpMessageBuffer))
		{
			va_end(args);
			return;
		}
		va_end(args);

		if (lpMessageBuffer == NULL)
			return;

		if (g_bConsole)
		{
			_tprintf(TEXT("[APP] error (%s): %s\n"), szFunction, lpMessageBuffer);
			LogFreeMessage(lpMessageBuffer);
			return;
		}
		pInsertStrings[0] = szFunction;
		pInsertStrings[1] = lpMessageBuffer;
		ReportEvent(
			g_hEventLog,
			EVENTLOG_ERROR_TYPE,
			wCategory,
			MSG_RUNTIME_ERROR,
			NULL,
			2,
			0,
			(LPCTSTR*)pInsertStrings,
			NULL
		);
		LogFreeMessage(lpMessageBuffer);
	}
}

VOID LogWarning(WORD wCategory, LPCTSTR szFunction, LPCTSTR szFormat, ...)
{
	LPTSTR lpMessageBuffer = NULL;
	LPCTSTR pInsertStrings[2] = { NULL, NULL };
	if (g_hEventLog)
	{
		va_list args;
		va_start(args, szFormat);
		if (!LogCreateMessage(szFormat, args, &lpMessageBuffer))
		{
			va_end(args);
			return;
		}
		va_end(args);

		if (lpMessageBuffer == NULL)
			return;

		if (g_bConsole)
		{
			_tprintf(TEXT("[APP] warning (%s): %s\n"), szFunction, lpMessageBuffer);
			LogFreeMessage(lpMessageBuffer);
			return;
		}

		pInsertStrings[0] = szFunction;
		pInsertStrings[1] = lpMessageBuffer;
		ReportEvent(
			g_hEventLog,
			EVENTLOG_WARNING_TYPE,
			wCategory,
			MSG_RUNTIME_WARN,
			NULL,
			2,
			0,
			(LPCTSTR*)pInsertStrings,
			NULL
		);
		LogFreeMessage(lpMessageBuffer);
	}
}

VOID LogWrite(DWORD dwSeverity, WORD wCategory, LPCTSTR szFormat, ...)
{
	LPTSTR lpMessageBuffer = NULL;
	if (g_hEventLog)
	{
		va_list args;
		va_start(args, szFormat);
		if (!LogCreateMessage(szFormat, args, &lpMessageBuffer))
		{
			va_end(args);
			return;
		}
		va_end(args);

		if (lpMessageBuffer == NULL)
			return;

		if (dwSeverity != STATUS_SEVERITY_SUCCESS &&
			dwSeverity != STATUS_SEVERITY_INFORMATIONAL)
			return;

		if (g_bConsole)
		{
			_tprintf(TEXT("[APP] info: %s\n"), lpMessageBuffer);
			LogFreeMessage(lpMessageBuffer);
			return;
		}

		ReportEvent(
			g_hEventLog,
			dwSeverity == STATUS_SEVERITY_SUCCESS
			? EVENTLOG_SUCCESS
			: EVENTLOG_INFORMATION_TYPE,
			wCategory,
			dwSeverity == STATUS_SEVERITY_SUCCESS
			? MSG_RUNTIME_SUCCESS
			: MSG_RUNTIME_INFO,
			NULL,
			1,
			0,
			const_cast<LPCTSTR*>(&lpMessageBuffer),
			NULL
		);
		LogFreeMessage(lpMessageBuffer);
	}
}

VOID LogVerbose(DWORD dwSeverity, WORD wCategory, LPCTSTR szFormat, ...)
{
	if (g_bVerbose)
	{
		LPTSTR lpMessageBuffer = NULL;
		if (g_hEventLog)
		{
			va_list args;
			va_start(args, szFormat);
			if (!LogCreateMessage(szFormat, args, &lpMessageBuffer))
			{
				va_end(args);
				return;
			}
			va_end(args);

			if (lpMessageBuffer == NULL)
				return;

			if (dwSeverity != STATUS_SEVERITY_SUCCESS &&
				dwSeverity != STATUS_SEVERITY_INFORMATIONAL)
				return;

			if (g_bConsole)
			{
				_tprintf(TEXT("[APP] debg: %s\n"), lpMessageBuffer);
			}
			LogFreeMessage(lpMessageBuffer);
		}
	}
}

BOOL LogClose()
{
	return DeregisterEventSource(g_hEventLog);
}
