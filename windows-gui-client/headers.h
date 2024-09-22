#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <VersionHelpers.h>
#include <ctime>
#include <fstream>
#include <ws2tcpip.h>
#include <msclr/marshal_cppstd.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ole32.lib")

using namespace std;