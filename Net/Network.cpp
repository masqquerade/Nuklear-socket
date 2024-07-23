#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include "Network.h"

#define BACKLOG 15

Network::Network()
{
	char host_port[5] = "1234"; // hardcoded
	strncpy_s(HOST_PORT, sizeof(HOST_PORT), host_port, sizeof(host_port));
}

bool Network::InitServer()
{
	int rc = 0;
	int reuse_addr = 1;
	ULONG nonblock_mode = 1;

	char addrstr[64];

	WSAPOLLFD fds[32] = {};
	int nfds = 0;

	WSADATA wsadata;

	struct addrinfo hints, * res = 0;
	struct sockaddr_in ipv4_addr;

	SOCKET listen_socket = INVALID_SOCKET;
	SOCKET client_socket = INVALID_SOCKET;

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		printf("WSAStartup error: %d", WSAGetLastError());
		return false;
	}

	if (gethostname(HOST_NAME, sizeof(HOST_NAME)) != 0)
	{
		printf("gethostname error: %d", WSAGetLastError());
		return false;
	}

	if (getaddrinfo(HOST_NAME, HOST_PORT, &hints, &res) != 0)
	{
		printf("getaddrinfo error: %d", WSAGetLastError());
		return false;
	}
	
	while (res)
	{
		switch (res->ai_family)
		{
		case AF_INET:
			ipv4_addr = *(struct sockaddr_in*)res->ai_addr;

			listen_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (listen_socket == INVALID_SOCKET)
			{
				printf("socket error: %d", WSAGetLastError());
				return false;
			}

			fds[nfds].fd = listen_socket;
			fds[nfds].events = POLLIN;
			nfds++;

			setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse_addr), sizeof(int));

			if (ioctlsocket(listen_socket, FIONBIO, &nonblock_mode) != 0)
			{
				printf("ioctlsocket error: %d", WSAGetLastError());
				return false;
			}

			if (bind(listen_socket, (struct sockaddr*)&ipv4_addr, sizeof(ipv4_addr)) != 0)
			{
				printf("bind error: %d", WSAGetLastError());
				return false;
			}

			if (listen(listen_socket, BACKLOG) != 0)
			{
				printf("listen error: %d", WSAGetLastError());
				return false;
			}

			inet_ntop(res->ai_family, &ipv4_addr.sin_addr, addrstr, sizeof(addrstr));
			printf("binded: %s\n", addrstr);

			break;
		}

		res = res->ai_next;
	}

	freeaddrinfo(res);

	while (true)
	{
		rc = WSAPoll(fds, nfds, (3 * 60 * 1000));
		
		if (rc == 0)
		{
			continue;
		}

		if (rc == SOCKET_ERROR)
		{
			printf("WSAPoll error: %d", WSAGetLastError());
			return false;
		}

		for (int i = 0; i < nfds; i++)
		{
			if (fds[i].revents && POLLIN)
			{
				if (fds[i].fd == listen_socket)
				{
					do
					{
						client_socket = accept(fds[i].fd, NULL, NULL);
					} 
					while (client_socket == INVALID_SOCKET && WSAGetLastError() == WSAEINTR);

					if (client_socket == INVALID_SOCKET && WSAGetLastError() == WSAEWOULDBLOCK)
					{
						break;
					}
					
					fds[nfds].fd = client_socket;
					fds[nfds].events = POLLIN | POLLOUT;
					nfds++;

					printf("new connection.");
				}
				else
				{
					// got msg from client
				}
			}
		}
	}

	return true;
}