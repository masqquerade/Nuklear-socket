#include <Windows.h>
#include <winsock.h>
#include "Network.h"
#include <stdio.h>

int main()
{
	Network *net = new Network();

	if (!net->InitServer())
	{
		printf("InitServer error");
		return -1;
	}

	return 0;
}