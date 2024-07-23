#pragma once

#include <WinSock2.h>
#include <Windows.h>

class IOController
{
public:
	static bool RecvAll(SOCKET fd, char *buf, int size);
	static bool SendAll(SOCKET fd, char* buf, int size);
};