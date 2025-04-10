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

#ifndef GV_LOG_H
#define GV_LOG_H

#include <windows.h>

#include "governor.h"

#define __FUNCTIONT__ TEXT(__FUNCTION__)

BOOL LogInit(BOOL);

VOID LogSetVerbose(BOOL);

VOID LogSystemError(WORD wCategory, LPCTSTR szFunction, DWORD dwError);

VOID LogError(WORD wCategory, LPCTSTR szFunction, LPCTSTR szFormat, ...);

VOID LogWarning(WORD wCategory, LPCTSTR szFunction, LPCTSTR szFormat, ...);

VOID LogWrite(DWORD dwSeverity, WORD wCategory, LPCTSTR szFormat, ...);

VOID LogVerbose(DWORD dwSeverity, WORD wCategory, LPCTSTR szFormat, ...);

BOOL LogClose();

#endif //GV_LOG_H