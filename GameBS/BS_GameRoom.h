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

	void SetTickCountRoom();
	uint64 GetTickCountRoom();

	void SpanMoster();
	void MoveMoster();

	void MapTask();

private:
	void InitMapInfo();

private:
	uint16 _roomId = -1;
	unordered_map<int32, shared_ptr<class BS_GameSession>> _sessionMap;

	uint64 _tickTime = 0;

	shared_ptr<class BS_MapInfo> _mapInfo;

	// 몬스터 개수 고정 10마리
	int32 _monsterSize = 10;

	unordered_map<int32, shared_ptr<class BS_Monster_Info>> _monsterMap;

	uint64 _taskTime = 500;

	random_device rd;
	mt19937 gen;
	uniform_int_distribution<int> disRotate{-180, 180};
};
