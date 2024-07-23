#pragma once

#include <WinSock2.h>
#include <Windows.h>

// TODO

class Protocol
{
public:
	static bool ProtoRecv(SOCKET fd, char *buf);
	static bool ProtoSend(SOCKET fd, char *buf);
};