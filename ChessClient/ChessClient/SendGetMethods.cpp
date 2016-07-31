#include "stdafx.h"
#include "Client.h"

//Sends the size of the message
bool Client::SendInt(int _int)
{
	int RetnCheck = send(connection, (char*)&_int, sizeof(int), NULL); //send int: _int
	if (RetnCheck == SOCKET_ERROR) //If int failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

//Retrieves the size of the message
bool Client::GetInt(int & _int)
{
	int RetnCheck = recv(connection, (char*)&_int, sizeof(int), NULL); //receive integer
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not get the integer
	return true;//Return true if we were successful in retrieving the int
}

//Sends the type of message
bool Client::SendPacketType(Packet _packettype)
{
	int RetnCheck = send(connection, (char*)&_packettype, sizeof(Packet), NULL); //Send packet: _packettype
	if (RetnCheck == SOCKET_ERROR) //If packettype failed to send due to connection issue
		return false; //Return false: Connection issue
	return true; //Return true: int successfully sent
}

//Retrives the type of message
bool Client::GetPacketType(Packet & _packettype)
{
	int RetnCheck = recv(connection, (char*)&_packettype, sizeof(Packet), NULL); //receive packet type (same as integer)
	if (RetnCheck == SOCKET_ERROR) //If there is a connection issue
		return false; //return false since we did not properly get the packet type
	return true;//Return true if we were successful in retrieving the packet type
}

//Sends the actual message
bool Client::SendString(std::string & _string)
{
	if(_string == "Play Chess")
	{
		if(gState != GameState::G_PublicChat)
		{
			std::cout<<"Already in a game, canceling request"<<std::endl;
			return false;
		}
		_string = "RequestChess";
		std::cout<<"Waiting for player"<<std::endl;
		if (!SendPacketType(P_GameStateChange)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	}
	else if(_string == "Return to lobby")
	{
		if(gState == GameState::G_PublicChat)
		{
			std::cout<<"Already in the lobby, canceling request"<<std::endl;
			return false;
		}
		gState = GameState::G_PublicChat;
		std::cout<<"Leaving chess and returning to lobby"<<std::endl;
		if (!SendPacketType(P_GameStateChange)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	}
	else if(_string.find("MOVE:") != std::string::npos)
	{
		if (!SendPacketType(P_ChessMove)) //Send packet type: Chat Message, If sending packet type fails...
		return false;
	}
	else
	{
		if (!SendPacketType(P_ChatMessage)) //Send packet type: Chat Message, If sending packet type fails...
		return false; //Return false: Failed to send string
	}
	
	int bufferlength = _string.size(); //Find string buffer length
	if (!SendInt(bufferlength)) //Send length of string buffer, If sending buffer length fails...
		return false; //Return false: Failed to send string buffer length
	int RetnCheck = send(connection, _string.c_str(), bufferlength, NULL); //Send string buffer
	if (RetnCheck == SOCKET_ERROR) //If failed to send string buffer
		return false; //Return false: Failed to send string buffer
	return true; //Return true: string successfully sent
}

//Retrieves the actual message
bool Client::GetString(std::string & _string)
{
	int bufferlength; //Holds length of the message
	if (!GetInt(bufferlength)) //Get length of buffer and store it in variable: bufferlength
		return false; //If get int fails, return false
	char * buffer = new char[bufferlength + 1]; //Allocate buffer
	buffer[bufferlength] = '\0'; //Set last character of buffer to be a null terminator so we aren't printing memory that we shouldn't be looking at
	int RetnCheck = recv(connection, buffer, bufferlength, NULL); //receive message and store the message in buffer array, set RetnCheck to be the value recv returns to see if there is an issue with the connection
	_string = buffer; //set string to received buffer message
	delete[] buffer; //Deallocate buffer memory (cleanup to prevent memory leak)
	if (RetnCheck == SOCKET_ERROR) //If connection is lost while getting message
		return false; //If there is an issue with connection, return false
	return true;//Return true if we were successful in retrieving the string
}