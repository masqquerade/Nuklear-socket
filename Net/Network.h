#pragma once

#include <windows.h>

class Network
{
private:
	char HOST_NAME[128] = {0};
	char HOST_PORT[50];
	char CLIENT_NAME[50] = { 0 };
	int CLIENT_PORT = 0;

public:
	Network();

	bool InitServer();
};