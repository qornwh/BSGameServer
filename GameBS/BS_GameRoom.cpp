#include "BS_GameRoom.h"
#include "BS_Player.h"
#include "../CoreLib/PlayerInfo.h"
#include "BS_GameSession.h"

BS_GameRoom::BS_GameRoom(int roomId) : _roomId(roomId), JobQueue()
{
}

BS_GameRoom::~BS_GameRoom()
{
	cout << "~BS_GameRoom" << endl;
}

void BS_GameRoom::CreatePlayerInfo()
{
}

void BS_GameRoom::AddSession(shared_ptr<BS_GameSession> gameSession)
{
	_sessionMap[gameSession->getSocketFd()] = gameSession;
}

void BS_GameRoom::DelSession(shared_ptr<BS_GameSession> gameSession)
{
	_sessionMap.erase(gameSession->getSocketFd());
}

void BS_GameRoom::Broadcast(SendBufferRef sendBuffer)
{
	for (auto pair : _sessionMap)
	{
		pair.second->Send(sendBuffer);
	}
}

void BS_GameRoom::BroadcastAnother(SendBufferRef sendBuffer, int32 socketFd)
{
	for (auto pair : _sessionMap)
	{
		if (socketFd != pair.first)
			pair.second->Send(sendBuffer);
	}
}

void BS_GameRoom::PushJobQueue(JobRef job)
{
	Push(job);
}
