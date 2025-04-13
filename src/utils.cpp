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
#include <ntstatus.h>
#include <aclapi.h>
#include <sddl.h>

#include "utils.hpp"

struct {
    HMODULE hNtdll;
    struct {
        NtQueryInformationProcess_t NtQueryInformationProcess;
    } Functions;
} UtilContext;

DWORD InitUtils()
{
    ZeroMemory(&UtilContext, sizeof(UtilContext));
    UtilContext.hNtdll = LoadLibraryW(L"ntdll.dll");
    if (UtilContext.hNtdll == NULL)
    {
        return GetLastError();
    }
    UtilContext.Functions.NtQueryInformationProcess = reinterpret_cast<NtQueryInformationProcess_t>(
        GetProcAddress(UtilContext.hNtdll, "NtQueryInformationProcess"));
    if (UtilContext.Functions.NtQueryInformationProcess == NULL)
    {
        return GetLastError();
    }
    return ERROR_SUCCESS;
}

VOID CleanupUtils()
{
    if (UtilContext.hNtdll)
        FreeLibrary(UtilContext.hNtdll);
    ZeroMemory(&UtilContext, sizeof(UtilContext));
}

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

std::wstring RandomGuid()
{
    std::wstring guidString;
    GUID fileGuid{ 0 };
    if (CoCreateGuid(&fileGuid) == S_OK)
    {
        guidString.resize(39);
        int rc = StringFromGUID2(fileGuid, guidString.data(), guidString.size());
        guidString.resize(38);
    }
    return guidString;
}

DWORD GetProcessPEB(HANDLE hProcess, PPEB pPeb)
{
    PROCESS_BASIC_INFORMATION pbi{};
    NTSTATUS status = UtilContext.Functions.NtQueryInformationProcess(
        hProcess,
        ProcessBasicInformation,
        &pbi,
        sizeof(pbi),
        NULL
    );
    if (status != STATUS_SUCCESS)
    {
        return status;
    }
    if (pbi.PebBaseAddress == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }
    SIZE_T dwBytesRead = 0;
    if (!ReadProcessMemory(
        hProcess,
        pbi.PebBaseAddress,
        pPeb,
        sizeof(PEB),
        &dwBytesRead
    ))
    {
        return GetLastError();
    }
}

VOID GetCurrentPEB(PPEB pPeb)
{
#if defined(_M_X64) // x64
    PTEB pTeb = (PTEB)__readgsqword(offsetof(NT_TIB, Self));
#else // x86
    PTEB pTeb = (PTEB)__readfsdword(offsetof(NT_TIB, Self));
#endif
    if (pTeb)
        *pPeb = *(pTeb->ProcessEnvironmentBlock);
}
