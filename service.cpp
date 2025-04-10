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

#include <tchar.h>

#include "utils.hpp"
#include "log.hpp"
#include "service.hpp"
#include "server.hpp"

SERVICE_STATUS g_ServiceStatus;
SERVICE_STATUS_HANDLE g_hServiceStatus;
HANDLE g_hStopEvent;

// Handle control codes from SCM
VOID WINAPI SvcCtrlHandler(DWORD);

// Report service status to SCM on request
VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint);

VOID SvcUninstall()
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	
	if (!RegDeleteKey(
		HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Services\\eventlog\\Application\\") GV_SVCNAME
	)) {
		_tprintf(TEXT("Failed to deregister event provider: 0x%x\n"), GetLastError());
		return;
	}

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		_tprintf(TEXT("Failed to open Service Control Manager: 0x%x\n"), GetLastError());
		return;
	}
	hService = OpenService(hSCManager, GV_SVCNAME, DELETE);
	if (!hService)
	{
		_tprintf(TEXT("Failed to open service: 0x%x. Are you sure you installed it?\n"), GetLastError());
		CloseHandle(hSCManager);
		return;
	}
	if (!DeleteService(hService))
	{
		_tprintf(TEXT("Failed to delete service: 0x%x\n"), GetLastError());
	}
	CloseHandle(hService);
	CloseHandle(hSCManager);
}

// Only invoked from direct user interaction, when "install" argument passed to application
VOID SvcInstall(PSERVICE_PARAMETERS pParameters)
{
	SC_HANDLE hSCManager;
	SC_HANDLE hService;
	DWORD status;
	HKEY hKey, hProvKey;
	const DWORD dwTypesSupported = 5;
	const DWORD dwCategoryCount = 6;

	TCHAR szPath[MAX_PATH];

	if (pParameters->Uninstall)
	{
		SvcUninstall();
		return;
	}

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		_tprintf(TEXT("Failed to get service exe path: 0x%x\n"), GetLastError());
		return;
	}

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!hSCManager)
	{
		_tprintf(TEXT("Failed to open Service Control Manager: 0x%x\n"), GetLastError());
		return;
	}
	hService = CreateService(
		hSCManager,
		GV_SVCNAME,
		TEXT("ALCA Governor"),
		SC_MANAGER_CREATE_SERVICE,
		SERVICE_WIN32_OWN_PROCESS,
		pParameters->StartupService
		? SERVICE_AUTO_START
		: SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);

	if (hService == NULL)
	{
		status = GetLastError();
		if (status == ERROR_SERVICE_EXISTS)
		{
			_tprintf(TEXT("The service has already been installed.\n"));
			return;
		}
		_tprintf(TEXT("Failed to create new service: 0x%x\n"), status);
		return;
	}

	status = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Services\\eventlog\\Application"),
		0,
		KEY_WRITE,
		&hKey
	);

	if (status != ERROR_SUCCESS)
	{
		_tprintf(TEXT("Failed to open event log registry: 0x%x\n"), GetLastError());
		CloseHandle(hService);
		return;
	}

	status = RegCreateKeyEx(
		hKey,
		GV_SVCNAME,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hProvKey,
		NULL
	);
	
	if (status != ERROR_SUCCESS)
	{
		_tprintf(TEXT("Failed to register event provider: 0x%x\n"), GetLastError());
		CloseHandle(hService);
		RegCloseKey(hKey);
		return;
	}

	if(RegSetValueEx(
		hProvKey,
		TEXT("EventMessageFile"),
		0,
		REG_SZ,
		reinterpret_cast<const BYTE*>(szPath),
		(_tcslen(szPath) + 1) * sizeof(TCHAR))
	 != ERROR_SUCCESS)
		goto END;

	if (RegSetValueEx(
		hProvKey,
		TEXT("CategoryMessageFile"),
		0,
		REG_SZ,
		reinterpret_cast<const BYTE*>(szPath),
		(_tcslen(szPath) + 1) * sizeof(TCHAR))
		!= ERROR_SUCCESS)
		goto END;

	if (RegSetValueEx(
		hProvKey,
		TEXT("ParameterMessageFile"),
		0,
		REG_SZ,
		reinterpret_cast<const BYTE*>(szPath),
		(_tcslen(szPath) + 1) * sizeof(TCHAR))
		!= ERROR_SUCCESS)
		goto END;

	if (RegSetValueEx(
		hProvKey,
		TEXT("CategoryCount"),
		0,
		REG_DWORD,
		reinterpret_cast<const BYTE*>(&dwCategoryCount),
		sizeof(DWORD))
		!= ERROR_SUCCESS)
		goto END;

	if (RegSetValueEx(
		hProvKey,
		TEXT("TypesSupported"),
		0,
		REG_DWORD,
		reinterpret_cast<const BYTE*>(&dwTypesSupported),
		sizeof(DWORD))
		!= ERROR_SUCCESS)
		goto END;

	if (pParameters->StartAfterCreation)
	{
		if (!StartService(hService, 0, NULL))
		{
			_tprintf(TEXT("Failed to start service: 0x%x\n"), GetLastError());
		}
	}
END:
	CloseHandle(hKey);
	CloseHandle(hProvKey);
}

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	g_hServiceStatus = RegisterServiceCtrlHandler(
		GV_SVCNAME,
		SvcCtrlHandler
	);
	if (!g_hServiceStatus)
	{
		ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
		return;
	}

	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	
	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 0);

	if (!LogInit(FALSE))
	{
		ReportSvcStatus(SERVICE_STOPPED, GetLastError(), 0);
		return;
	}

	SvcInit(TRUE, FALSE);
}

static BOOL WINAPI ctrlHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT)
	{
		SetEvent(g_hStopEvent);
	}
	return TRUE;
}

VOID SvcInit(BOOL bService, BOOL bVerbose)
{
	g_hStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!bService)
	{
		if (!LogInit(TRUE))
		{
			// Branch only executed by non-service since service already called in SvcMain LogInit
			_tprintf(TEXT("Failed to initialise event logging: 0x%x\n"), GetLastError());
			return;
		}
		LogSetVerbose(bVerbose);
		_tprintf(TEXT("Initialised event logging (source: %s)\n"), GV_SVCNAME);
		if (!SetConsoleCtrlHandler(ctrlHandler, TRUE))
		{
			_tprintf(TEXT("Failed to set CTRL-C handler: %d\n"), GetLastError());
			return;
		}
	}
	HANDLE hThread = CreateThread(NULL, 0, GvAwaitAlcaRequests, g_hStopEvent, 0, NULL);
	if (hThread == NULL)
	{
		LogError(MAIN_CATEGORY, __FUNCTIONT__, TEXT("Failed to create thread: %d"), GetLastError());
		return;
	}
	LogWrite(STATUS_SEVERITY_INFORMATIONAL, MAIN_CATEGORY, TEXT("Started %s successfully"), GV_SVCNAME);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	if (!bService)
	{
		_tprintf(TEXT("Exited successfully.\n"));
	}
}

VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	g_ServiceStatus.dwCurrentState = dwCurrentState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		g_ServiceStatus.dwControlsAccepted = 0;
	else g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		g_ServiceStatus.dwCheckPoint = 0;
	else g_ServiceStatus.dwCheckPoint = dwCheckPoint++;

	SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
		case SERVICE_CONTROL_STOP:
			ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

			SetEvent(g_hStopEvent); // Stop work, SvcInit updates service status
			ReportSvcStatus(g_ServiceStatus.dwCurrentState, NO_ERROR, 0);
			return;

		default:
			break;
	}
}
