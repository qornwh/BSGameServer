#pragma once

#include "../CoreLib/JobQueue.h"

class BS_GameRoom : public JobQueue
{
public:
	BS_GameRoom(int roomId);
	~BS_GameRoom();

public:
	void CreatePlayerInfo();
	void AddSession(shared_ptr<class BS_GameSession> gameSession);
	void DelSession(shared_ptr<class BS_GameSession> gameSession);
	void Broadcast(SendBufferRef sendBuffer);
	void BroadcastAnother(SendBufferRef sendBuffer, int32 socketFd);
	void PushJobQueue(JobRef job);

private:
	uint16 _roomId;
	unordered_map<int32, shared_ptr<class BS_GameSession>> _sessionMap;
};
