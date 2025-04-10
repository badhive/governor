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

#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>
#include "trace.hpp"
#include <objbase.h>

#include "alca.h"
#include "log.hpp"
#include "server.hpp"
#include "utils.hpp"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "ole32.lib")

#define GV_SVCPORT 4164

typedef struct _GV_CONN_CTX
{
    SOCKET clientSocket;
    HANDLE hEvent;
} GV_CONN_CTX, * PGV_CONN_CTX;

void HandleConn(PGV_CONN_CTX ctx);

DWORD WINAPI HandleConnThunk(LPVOID param)
{
    PGV_CONN_CTX ctx = static_cast<PGV_CONN_CTX>(param);
    SOCKADDR clientAddr{ 0 };
    SOCKET s = ctx->clientSocket;
    int addrLen = sizeof(clientAddr);
    if (getpeername(ctx->clientSocket, &clientAddr, &addrLen) == SOCKET_ERROR)
    {
        LogWarning(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to get peer name: %d"), WSAGetLastError());
    }
    else
    {
        std::vector<TCHAR> a;
        DWORD dwAddrLen = 0;
        if (WSAAddressToString(&clientAddr,
            addrLen,
            NULL,
            a.data(),
            &dwAddrLen
        ) == SOCKET_ERROR && WSAGetLastError() != WSAEFAULT)
        {
            LogWarning(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to get peer address: %d"), WSAGetLastError());
        }
        else
        {
            a.resize(dwAddrLen);
            if (WSAAddressToString(&clientAddr,
                addrLen,
                NULL,
                a.data(),
                &dwAddrLen
            ) == SOCKET_ERROR)
            {
                LogWarning(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to get peer address: %d"), WSAGetLastError());
            }
            else
            {
                LogWrite(STATUS_SEVERITY_INFORMATIONAL,
                    REPORTING_CATEGORY,
                    TEXT("Received connection from %s"),
                    a.data()
                );
            }
        }
    }
    HandleConn(ctx);
    closesocket(s);
    return ERROR_SUCCESS;
}

DWORD GvAwaitAlcaRequests(HANDLE hEvent)
{
    WSADATA wsaData;
    int rc;
    DWORD mode, status;
    if ((rc = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("WSAStartup failed: %d"), rc);
        return rc;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((rc = ioctlsocket(serverSocket, FIONBIO, &mode)) != 0)
    {
        LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to set to non-blocking: %d"), rc);
        return rc;
    }
    sockaddr_in serverAddr{ 0 };
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(GV_SVCPORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    LogWrite(STATUS_SEVERITY_INFORMATIONAL, REPORTING_CATEGORY, TEXT("Listening on :%d..."), GV_SVCPORT);
    while (true)
    {
        status = WaitForSingleObject(hEvent, 0);
        if (status == WAIT_OBJECT_0)
        {
            break; // service exit requested
        }
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket != INVALID_SOCKET) {
            HANDLE hThread;
            GV_CONN_CTX *ctx = new GV_CONN_CTX { clientSocket, hEvent };
            if (HandleConnThunk(ctx) != ERROR_SUCCESS) // single-threaded
            {
                LogSystemError(REPORTING_CATEGORY, __FUNCTIONT__, WSAGetLastError());
            }
            continue;
        }
        if ((rc = WSAGetLastError()) != WSAEWOULDBLOCK)
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to accept connection: %d"), rc);
            continue;
        }
    }
    LogWrite(STATUS_SEVERITY_INFORMATIONAL, REPORTING_CATEGORY, TEXT("Stopping server..."));
    closesocket(serverSocket);
    return WSACleanup();
}

void HandleConn(PGV_CONN_CTX ctx)
{
    int rc;
    DWORD status;
    UINT32 packetSize = 0;
    ac_packet_handle hPacket;
    ac_packet_header packetHeader{0};
    std::vector<CHAR> fileNameBuffer;
    std::ofstream outfile;
    std::wstring szFilePath;
    SOCKET clientSocket = ctx->clientSocket;

    std::wstring guidString = RandomGuid();
    if (guidString.length() == 0)
        return;

    while (packetHeader.sequence != AC_PACKET_SEQUENCE_LAST)
    {
        rc = recv(clientSocket, reinterpret_cast<char*>(&packetSize), sizeof(packetSize), 0);
        if (rc <= 0)
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to receive data from ALCA: rc=%d"), rc);
            return;
        }
        packetSize = netint(packetSize);
        if (!packetSize)
            return;
        if (packetSize > AC_PACKET_MAX_RECV_SIZE)
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Received packet size exceeds maximum (%d > %d)"),
                packetSize,
                AC_PACKET_MAX_RECV_SIZE);
            return;
        }

        std::vector<char> packet(packetSize);
        rc = recv(clientSocket, packet.data(), packetSize, 0);
        if (rc <= 0)
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to receive packet: rc=%d"), rc);
            return;
        }
        if (ac_packet_read(reinterpret_cast<const uint8_t*>(packet.data()), packet.size(), &hPacket) < 0)
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to read packet - invalid format"));
            return;
        }
        ac_packet_get_header(hPacket, &packetHeader);

        // verify header
        if (packetHeader.magic != ALCA_MAGIC || packetHeader.version != (ALCA_VERSION))
        {
            LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Bad packet (invalid magic / version)"));
            return;
        }

        std::vector<UINT8> packetData(packetHeader.data_len);
        ac_packet_get_data(hPacket, packetData.data());
        if (packetHeader.data_type == AC_PACKET_DATA_REMOTE_SUBMIT)
        {
            if (!outfile.is_open())
                szFilePath = std::filesystem::temp_directory_path().wstring() + guidString + std::wstring(L".exe");
                outfile.open(szFilePath, std::ios::binary);
            outfile.write(reinterpret_cast<const char*>(packetData.data()), packetData.size());
        }
        else if (packetHeader.data_type == AC_PACKET_DATA_LOCAL_SUBMIT)
        {
            fileNameBuffer.insert(fileNameBuffer.end(), packetData.begin(), packetData.end());
        }
    }
    if (outfile.is_open())
    {
        outfile.close();
    }
    if (fileNameBuffer.size() > 0)
    {
        szFilePath.assign(fileNameBuffer.begin(), fileNameBuffer.end());
        LogWrite(STATUS_SEVERITY_INFORMATIONAL, REPORTING_CATEGORY, TEXT("Received local submission: %ls"), szFilePath.c_str());
    }

    governor::reporter reporter(clientSocket, packetHeader.packet_type);
    PROCESS_INFORMATION pi{ 0 };
    if ((status = CreateSuspendedProcess(szFilePath.c_str(), &pi)) != ERROR_SUCCESS)
    {
        LogSystemError(REPORTING_CATEGORY, __FUNCTIONT__, status);
        reporter.report_trace_status(AC_PACKET_DATA_SUBMIT_ERROR);
        return;
    }
    if (pi.hProcess == NULL || pi.hThread == NULL)
    {
        LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Process was not created"));
        return;
    }
    GV_EVENT_CONTEXT eventCtx{ &reporter };
    GV_TRACE_CONTEXT session{ pi, clientSocket, nullptr };
    auto trace = krabs::kernel_trace(GV_SVCNAMEW, &eventCtx);
    session.Trace = &trace;

    LogWrite(STATUS_SEVERITY_INFORMATIONAL, REPORTING_CATEGORY, TEXT("[PID=%d] Starting trace"), pi.dwProcessId);

    // report before creating thread - status update needs to come before events
    std::wstring wstr(GV_SVCNAMEW);
    std::string str(wstr.begin(), wstr.end());
    if ((rc = reporter.report_trace_status(
        AC_PACKET_DATA_TRACE_START,
        reinterpret_cast<const uint8_t*>(str.c_str()),
        str.size())) != 0)
    {
        LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to report status: %d"), rc);
        return;
    }
    HANDLE hThread = CreateThread(NULL, 0, GvTraceStart, &session, 0, NULL);
    if (hThread == NULL)
    {
        LogSystemError(REPORTING_CATEGORY, __FUNCTIONT__, GetLastError());
        return;
    }

    DWORD mode;
    if ((rc = ioctlsocket(clientSocket, FIONBIO, &mode)) != 0)
    {
        LogError(REPORTING_CATEGORY, __FUNCTIONT__, TEXT("Failed to make recv non-blocking: %d"), rc);
        reporter.report_trace_status(AC_PACKET_DATA_SUBMIT_ERROR);
        return;
    }
    // wait until exit to return
    while (1)
    {
        int done = 0, noexit = 0;
        std::wstring endReason;

        // client disconnect
        rc = recv(clientSocket, NULL, 0, 0);
        if (rc == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
        {
            done = 1;
            endReason = L"client disconnected";
        }
        // app exit signal
        else if (WaitForSingleObject(ctx->hEvent, 0) == WAIT_OBJECT_0)
        {
            done = 1;
            endReason = L"exit requested";
        }
        // trace exit / fail
        else if (WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0)
        {
            done = 1;
            endReason = L"trace completed";
        }
        // process exit
        else if (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
        {
            noexit = 1;
            done = 1;
            endReason = L"execution completed";
        }

        if (done)
        {
            if (!noexit)
                TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            LogWrite(STATUS_SEVERITY_INFORMATIONAL, REPORTING_CATEGORY, TEXT("[PID=%d] Ending trace... (reason = %ls)"), pi.dwProcessId, endReason.c_str());
            Sleep(1500); // with our settings, realtime etw buffers are flushed every second. Wait for all events to be processed before complete stoppage

            reporter.report_trace_status(AC_PACKET_DATA_TRACE_END);
            auto trace_stats = trace.query_stats();
            LogWrite(
                STATUS_SEVERITY_INFORMATIONAL,
                REPORTING_CATEGORY,
                TEXT("[PID=%d] Trace Statistics:\n")
                TEXT("    Buffers written  : %d\n")
                TEXT("    Buffers free     : %d\n")
                TEXT("    Buffers lost     : %d\n")
                TEXT("    ==============\n")
                TEXT("    Events handled   : %d\n")
                TEXT("    Events lost      : %d\n")
                TEXT("    ==============\n")
                TEXT("    Total events     : %d\n")
                , pi.dwProcessId
                , trace_stats.buffersWritten
                , trace_stats.buffersFree
                , trace_stats.buffersLost
                , trace_stats.eventsHandled
                , trace_stats.eventsLost
                , trace_stats.eventsTotal
            );
            return; // trace->stop() called
        }
    }
}
