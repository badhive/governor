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

#include <initguid.h>

#include <windows.h>

typedef struct _SERVICE_PARAMETERS {
	BOOL Uninstall;
	BOOL StartAfterCreation;
	BOOL StartupService;
} SERVICE_PARAMETERS, * PSERVICE_PARAMETERS;

// Initialise generic service functions e.g. service control handler, start main service
VOID WINAPI SvcMain(DWORD, LPTSTR *);

// Exported to run as non-service
VOID SvcInit(BOOL, BOOL);

// Install the service based on provided command-line arguments
VOID SvcInstall(PSERVICE_PARAMETERS);
