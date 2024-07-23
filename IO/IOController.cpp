#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include "IOController.h"

// serialization TODO

bool IOController::RecvAll(SOCKET fd, char* buf, int size)
{
	while (size)
	{
		int bytes = recv(fd, buf, size, 0);
		
		if (bytes < 0)
		{
			printf("recv error: %d", WSAGetLastError());
			return false;
		}

		if (bytes >= size)
		{
			return true;
		}

		size -= bytes;
		buf += bytes;
	}

	return true;
}

bool IOController::SendAll(SOCKET fd, char* buf, int size)
{
	while (size)
	{
		int bytes = send(fd, buf, size, 0);
		if (bytes < 0)
		{
			printf("send error: %d", WSAGetLastError());
			return false;
		}

		if (bytes >= size)
		{
			return true;
		}

		size -= bytes;
		buf += bytes;
	}

	return true;
}