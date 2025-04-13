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
#include <string>
#include <win32.h>

#define GV_SVCNAME TEXT("Alca.Sensor.Governor")
#define GV_SVCNAMEW L"Alca.Sensor.Governor"

DWORD CreateSuspendedProcess(LPCWSTR lpExecutablePath, LPPROCESS_INFORMATION ppi);

std::wstring RandomGuid();

DWORD GetProcessPEB(HANDLE hProcess, PPEB pPeb);

VOID GetCurrentPEB(PPEB pPeb);

DWORD InitUtils();

VOID CleanupUtils();