#include "stdafx.h"
#include "Server.h"
#include <algorithm>


//Initalises the server
Server::Server(int PORT, bool BroadcastPublically) 
{
	totalConnections = 0;
	addrlen = sizeof(addr);

	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	if (BroadcastPublically) //If server is open to public
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	else //If server is only for our router
		addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //Broadcast locally
	addr.sin_port = htons(PORT); //Port
	addr.sin_family = AF_INET; //IPv4 Socket

	sListen = socket(AF_INET, SOCK_STREAM, NULL); //Create socket to listen for new connections
	if (bind(sListen, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) //Bind the address to the socket, if we fail to bind the address..
	{
		std::string ErrorMsg = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	if (listen(sListen, SOMAXCONN) == SOCKET_ERROR) //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max Connections, if we fail to listen on listening socket...
	{
		std::string ErrorMsg = "Failed to listen on listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(NULL, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	serverptr = this;

	CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)StartChessGames, NULL, NULL, NULL);

}

//Is left open, listening for any new connections to the server
bool Server::ListenForNewConnection()
{
	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen); //Accept a new connection
	if (newConnection == 0) //If accepting the client connection failed
	{
		std::cout << "Failed to accept the client's connection." << std::endl;
		return false;
	}
	else //If client connection properly accepted
	{
		std::cout << "Client Connected! ID:" << totalConnections << std::endl;
		ChessClient player = ChessClient(totalConnections,newConnection);
		players.push_back(player);
		threads.push_back(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandlerThread, (LPVOID)(totalConnections), NULL, NULL));
		std::string MOTD = "Welcome to chess.\nThe commands are very simple.\nTo start looking for a match, enter(Play Chess).\nTo leave the match and re-enter the lobby type(Return to lobby).\nAnything else entered into the command prompt will be\n a message sent to the other players in the lobby.\n\nPlaying chess is simple. \nWhen it is your turn you will be able to click on a piece.\nThis will show you your available moves with that piece.\nReclicking that piece will de-select it.\nClicking any of the highlighted tiles will complete a move.\n\nHave Fun!";
		SendString(totalConnections, MOTD);
		totalConnections += 1; //Incremenent total # of clients that have connected
		return true;
	}
}

//Determines how to handle the incoming packet and what to return to the player or players
bool Server::ProcessPacket(int ID, Packet _packettype)
{
	int _id = FindClient(ID);
	if(_id == -1)
	{
		return false;
	}
	ChessClient& client = players[_id];

	switch (_packettype)
	{
	case P_ChatMessage: 
		{
			std::string Message; 
			if (!GetString(ID, Message)) 
				return false; 
							  
			if(client.chatState == ChatState::C_World)
			{
				for (int i = 0; i < players.size(); i++)
				{
					if (players[i].id == client.id) 
						continue;
					if (!SendString(players[i].id, Message))
					{
						std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << players[i].id << std::endl;
					}
				}
			}
			else if(client.chatState == ChatState::C_InGame)
			{
				if (!SendString(client.opponentID, Message)) 
				{
					std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << client.opponentID << std::endl;
				}
			}
			std::cout << "Processed chat message packet from user ID: " << ID << std::endl;
			break;
		}
	case P_GameStateChange:
		{
			std::string Message;
			if (!GetString(ID, Message)) 
				return false;
			if(Message == "RequestChess")
			{
				client.gameState = GameState::G_Waiting_For_chess;
				std::cout<< ID<<players[FindClient(ID)].gameState<<std::endl;
			}
			if(Message == "Return to lobby")
			{
				client.gameState = GameState::G_GlobalServer;
				if(client.opponentID != -1)
				{
					Message = "Your opponent has left the game.";
					_id = FindClient(client.opponentID);
					players[_id].opponentID = -1;
					if (!SendString(client.opponentID, Message)) 
					{
						std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << client.opponentID << std::endl;
					}
					client.opponentID = -1;
				}
				std::cout<< ID<<players[FindClient(ID)].gameState<<std::endl;
			}
			break;
		}
	case P_ChessMove: 
		{
			std::string Message;
			if (!GetString(ID, Message))
				return false; 
							  
			if (!SendString(client.opponentID, Message)) 
			{
				std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << client.opponentID << std::endl;
			}
			std::cout << "Processed chat message packet from user ID: " << ID << std::endl;
			break;
		}
	case P_Disconnect:
		{
			std::string Message = "Player " + std::to_string(client.id) + "has disconnected";
			for (int i = 0; i < players.size(); i++)
				{
					if (players[i].id == client.id)
						continue;
					if (!SendString(players[i].id, Message)) 
					{
						std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << players[i].id << std::endl;
					}
				}
			if(client.opponentID != -1)
			{
				Message = "You have won";
				_id = FindClient(client.opponentID);
				players[_id].opponentID = -1;
				if (!SendString(client.opponentID, Message)) 
				{
					std::cout << "Failed to send message from client ID: " << ID << " to client ID: " << client.opponentID << std::endl;
				}
			}
			break;
		}
	default: 
		{
			std::cout << "Unrecognized packet: " << _packettype << std::endl; 
			break;
		}
	}
	return true;
}

//Finds a client in the active clients based on the provided client ID
int Server::FindClient(int ID)
{
	for(int i = 0; i < players.size(); i++)
	{
		if(players[i].id == ID)
		{
			return i;
		}
	}
	return -1;
}

//Is always running, determines if there are two active people on the server who desire to play chess
//If so, place the two of them against each other, first to request as player 1
void Server::StartChessGames()
{
	std::vector<int> waiting;
	while(true)
	{
		for (int i = 0; i < serverptr->players.size(); i++)
		{
			if(serverptr->players[i].gameState == GameState::G_Waiting_For_chess && std::find(waiting.begin(), waiting.end(), serverptr->players[i].id) == waiting.end())
			{
				waiting.push_back(serverptr->players[i].id);
			}
			
		}
		for(int i = 0; i < waiting.size(); i++)
		{
			if(serverptr->FindClient(waiting[i]) == -1)
			{
				waiting.erase(waiting.begin()+i);
			}
		}
		if(waiting.size() >= 2)
		{
			std::cout<<"ready"<<std::endl;
			int p1 = waiting[0];
			int p2 = waiting[1];
			waiting.erase(waiting.begin(), waiting.begin()+2);
			serverptr->players[serverptr->FindClient(p1)].gameState = GameState::G_In_Chess;
			serverptr->players[serverptr->FindClient(p2)].gameState = GameState::G_In_Chess;
			serverptr->players[serverptr->FindClient(p1)].opponentID = p2;
			serverptr->players[serverptr->FindClient(p2)].opponentID = p1;
			std::string Message = "InitChessP1";
			if (!serverptr->SendString(p1, Message))
			{
				std::cout << "Failed to send message to client ID: " << p1 << std::endl;
			}
			Message = "InitChessP2";
			if (!serverptr->SendString(p2, Message)) 
			{
				std::cout << "Failed to send message to client ID: " << p2 << std::endl;
			}

		}
	}
}

//Is always running, A thread for each individual client
//Listens for incoming messages and sends them to be processed
void Server::ClientHandlerThread(int ID) //ID = the index in the SOCKET Connections array
{
	Packet PacketType;
	while (true)
	{
		if (!serverptr->GetPacketType(ID, PacketType)) 
			break; 
		if (!serverptr->ProcessPacket(ID, PacketType)) 
			break; 
	}
	std::cout << "Lost connection to client ID: " << ID << std::endl;
	serverptr->ProcessPacket(ID, Packet::P_Disconnect);
	closesocket(serverptr->players[serverptr->FindClient(ID)].connection);
	serverptr->players.erase(serverptr->players.begin() + serverptr->FindClient(ID));
}