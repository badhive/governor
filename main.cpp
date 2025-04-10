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
#include <tchar.h>

#include "utils.hpp"
#include "service.hpp"

int __cdecl _tmain(int argc, TCHAR* argv[])
{
	DWORD status = ERROR_SUCCESS;
	BOOL bVerbose = FALSE;
	SERVICE_PARAMETERS svcParameters = { 0 };

	if (argc > 1)
	{
		if (lstrcmpi(argv[1], TEXT("install")) == 0)
		{
			for (int i = 2; i < argc; i++)
			{
				if (lstrcmpi(argv[i], TEXT("/run")) == 0)
				{
					svcParameters.StartAfterCreation = TRUE;
				}
				else if (lstrcmpi(argv[i], TEXT("/startup")) == 0)
				{
					svcParameters.StartupService = TRUE;
				}
			}
		}
		else if (lstrcmpi(argv[1], TEXT("uninstall")) == 0)
		{
			svcParameters.Uninstall = TRUE;
		}
		else if (lstrcmpi(argv[1], TEXT("console")) == 0)
		{
			if (argc > 2 && lstrcmpi(argv[2], TEXT("/verbose")) == 0)
			{
				bVerbose = TRUE;
			}
			SvcInit(FALSE, bVerbose);
			return 0;
		}
		else if (lstrcmp(argv[1], TEXT("/?")) == 0)
		{
			_tprintf(TEXT(

				"\nALCA: Governor\n\n"

				"ALCA Engine's flagship user-mode Windows Sensor.\n"
				"Logs can be viewed in Event Viewer (%s)\n\n"

				"Usage:\n\n"

				"  install       : Install as service\n"
				"      /run      : Run after installation\n"
				"      /startup  : Configure service to start automatically on boot\n\n"

				"  uninstall     : Uninstall service\n\n"

				"  console       : Run as console application\n"
				"      /verbose  : Log verbosely\n\n"

			), GV_SVCNAME);
			return 0;
		}
		else {
			_tprintf(TEXT("Invalid option(s) specified, use /? for help\n"));
			return 0;
		}
		SvcInstall(&svcParameters);
		return 0;
	}

	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ const_cast<wchar_t*>(GV_SVCNAME), (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	StartServiceCtrlDispatcher(DispatchTable);
	if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
	{
		_tprintf(TEXT("No options specified, use /? for help"));
		return 1;
	}
}
