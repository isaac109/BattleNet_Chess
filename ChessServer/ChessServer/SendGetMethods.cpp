#include "stdafx.h"
#include "Server.h"

//Sends the size of the message first
bool Server::SendInt(int ID, int _int)
{
	int RetnCheck = send(players[FindClient(ID)].connection, (char*)&_int, sizeof(int), NULL); 
	if (RetnCheck == SOCKET_ERROR) 
		return false; 
	return true; 
}

//Retrieves the size of the incomeing massage
bool Server::GetInt(int ID, int & _int)
{
	int RetnCheck = recv(players[FindClient(ID)].connection, (char*)&_int, sizeof(int), NULL); 
	if (RetnCheck == SOCKET_ERROR) 
		return false; 
	return true;
}

//Sends the type of message
bool Server::SendPacketType(int ID, Packet _packettype)
{
	int RetnCheck = send(players[FindClient(ID)].connection, (char*)&_packettype, sizeof(Packet), NULL); 
	if (RetnCheck == SOCKET_ERROR) 
		return false;
	return true;
}

//Retrieves the type of message
bool Server::GetPacketType(int ID, Packet & _packettype)
{
	int RetnCheck = recv(players[FindClient(ID)].connection, (char*)&_packettype, sizeof(Packet), NULL); 
	if (RetnCheck == SOCKET_ERROR) 
		return false; 
	return true;
}

//Sends the actual message
bool Server::SendString(int ID, std::string & _string)
{
	if(_string.find("InitChess") != std::string::npos)
	{
		if (!SendPacketType(ID, P_GameStateChange)) 
		return false; 
	}
	else if(_string.find("MOVE:") != std::string::npos)
	{
		if (!SendPacketType(ID, P_ChessMove))
		return false;
	}
	else
	{
		if (!SendPacketType(ID, P_ChatMessage)) 
			return false; 
	}
	int bufferlength = _string.size(); 
	if (!SendInt(ID, bufferlength)) 
		return false; 
	int RetnCheck = send(players[FindClient(ID)].connection, _string.c_str(), bufferlength, NULL); 
	if (RetnCheck == SOCKET_ERROR) 
		return false; 
	return true; 
}

//Retrieves the actual message
bool Server::GetString(int ID, std::string & _string)
{
	int bufferlength; 
	if (!GetInt(ID, bufferlength)) 
		return false; 
	char * buffer = new char[bufferlength + 1]; 
	buffer[bufferlength] = '\0'; 
	int RetnCheck = recv(players[FindClient(ID)].connection, buffer, bufferlength, NULL); 
	_string = buffer; 
	delete[] buffer; 
	if (RetnCheck == SOCKET_ERROR) 
		return false; 
	return true;
}
