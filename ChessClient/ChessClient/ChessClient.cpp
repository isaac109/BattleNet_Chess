// ChessClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Client.h"


int _tmain(int argc, char* argv[])
{
	Client myClient("127.0.0.1",1111);

	myClient._argc = argc;
	myClient._argv = argv;

	if (!myClient.Connect()) 
	{
		std::cout << "Failed to connect to server..." << std::endl;
		system("pause");
		return 1;
	}

	std::string userinput; 
	while (true)
	{
		std::getline(std::cin,userinput); 
		if (!myClient.SendString(userinput)) 
			break; 
	}
	system("pause");
	return 0;
}

