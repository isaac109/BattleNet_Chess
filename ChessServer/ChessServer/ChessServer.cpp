// ChessServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Server.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Server myServer(1111); //Create server on port 1111
	for (int i = 0; i < 100; i++) //Up to 100 times (allows the server to not be overwhelmed)
	{
		myServer.ListenForNewConnection(); //Accept new connection (if someones trying to connect)
	}
	system("pause");
	return 0;
	return 0;
}

