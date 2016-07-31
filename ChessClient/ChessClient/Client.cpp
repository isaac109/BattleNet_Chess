#include "stdafx.h"
#include "Client.h"
#include <string>

//Determines how to handle the incoming message
bool Client::ProcessPacket(Packet _packettype)
{
	switch (_packettype)
	{
	case P_ChatMessage:
	{
		std::string Message;
		if (!GetString(Message)) 
			return false; 
		std::cout << Message << std::endl; 
		break;
	}
	case P_GameStateChange:
		{
			std::string Message; 
			if (!GetString(Message)) 
				return false; 
			std::cout << Message << std::endl; 
			if(Message.find("InitChess") != std::string::npos)
			{
				if(Message.find("P1") != std::string::npos)
				{
					clientptr->gState = GameState::G_InChessP1;
					std::cout<<"Starting Chess as Player 1"<<std::endl;
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PlayChess, (LPVOID)(PlayerType::P_ONE), NULL, NULL);
				}
				else if(Message.find("P2") != std::string::npos)
				{
					clientptr->gState = GameState::G_InChessP2;
					std::cout<<"Starting Chess as Player 2"<<std::endl;
					CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)PlayChess, (LPVOID)(PlayerType::P_TWO), NULL, NULL);
				}
			}
			break;
		}
	case P_ChessMove:
		{
			std::string Message; 
			if (!GetString(Message)) 
				return false; 
			std::cout << Message << std::endl;
			GUI::MoveOpponent(Message);
			break;
		}
	default: 
		std::cout << "Unrecognized packet: " << _packettype << std::endl; 
		break;
	}
	return true;
}

//Constantly running to listen for incoming messages
void Client::ClientThread()
{
	Packet PacketType;
	while (true)
	{
		if (!clientptr->GetPacketType(PacketType)) 
			break; 
		if (!clientptr->ProcessPacket(PacketType)) 
			break; 
	}
	std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->CloseConnection()) 
	{
		std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else 
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

//Initializes the Client object
Client::Client(std::string IP, int PORT)
{
	sizeofaddr = sizeof(addr);

	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	addr.sin_addr.s_addr = inet_addr(IP.c_str()); //Address 127.0.0.1 = localhost (This pc)
	addr.sin_port = htons(PORT); //Port to connect on
	addr.sin_family = AF_INET; //IPv4 Socket
	gState = GameState::G_PublicChat;
	clientptr = this; //Update ptr to the client which will be used by our client thread
}

//Connects the client to the server
bool Client::Connect()
{
	connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
	{
		MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	std::cout << "Connected!" << std::endl;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientThread, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.
	return true;
}

//Closes the connection from the client to the server
bool Client::CloseConnection()
{
	if (closesocket(connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error is that operation is not performed on a socket (This happens when the socket has already been closed)
			return true; //return true since connection has already been closed

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(NULL, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

//Method from the client to the gui to initialize chess
void Client::PlayChess(PlayerType::Player player)
{
	GUI::Initialize(clientptr->_argc, clientptr->_argv, player);
	while(clientptr->gState != GameState::G_PublicChat)
	{
		std::string msg = GUI::Update();
		if(msg.find("MOVE:") != std::string::npos)
		{
			if (!clientptr->SendString(msg)) //Send string: userinput, If string fails to send... (Connection issue)
			break;
		}
	}

}