#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <vector>
#include "ChessClient.h"

enum Packet
{
	P_ChatMessage,
	P_GameStateChange,
	P_ChessMove,
	P_Disconnect
};

class Server
{
public:
	Server(int PORT, bool BroadcastPublically = false);
	bool ListenForNewConnection();

private:
	bool SendInt(int ID, int _int);
	bool GetInt(int ID, int & _int);

	bool SendPacketType(int ID, Packet _packettype);
	bool GetPacketType(int ID, Packet & _packettype);

	bool SendString(int ID, std::string & _string);
	bool GetString(int ID, std::string & _string);

	bool ProcessPacket(int ID, Packet _packettype);

	int FindClient(int ID);

	static void ClientHandlerThread(int ID);
	static void StartChessGames();

private:
	std::vector<ChessClient> players;
	std::vector<HANDLE> threads;
	int totalConnections;

	
	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	int addrlen;
	SOCKET sListen;
};

static Server * serverptr; //Serverptr is necessary so the static ClientHandler method can access the server instance/functions.
