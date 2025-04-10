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

#include "utils.hpp"

DWORD CreateSuspendedProcess(LPCWSTR lpExecutablePath, LPPROCESS_INFORMATION ppi)
{
    if (!ppi)
    {
        return STATUS_INVALID_PARAMETER;
    }
    DWORD status;
    STARTUPINFO si{ 0 };
    si.cb = sizeof(si);

    if (!CreateProcessW(
        NULL,
        const_cast<LPWSTR>(lpExecutablePath),
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        ppi
    ))
    {
        return GetLastError();
    }
    return ERROR_SUCCESS;
}
