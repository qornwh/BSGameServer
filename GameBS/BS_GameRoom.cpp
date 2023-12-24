#include "BS_GameRoom.h"
#include "BS_Player.h"
#include "../CoreLib/PlayerInfo.h"
#include "../CoreLib/Utils.h"
#include "BS_GameSession.h"
#include "BS_MapInfo.h"
#include "BS_Packet.h"
#include <random>
#include "BS_PacketHandler.h"

BS_GameRoom::BS_GameRoom(int roomId) : _roomId(roomId), _tickTime(FunctionUtils::TimeUtils::GetTickCount64_OS()), JobQueue()
{
	InitMapInfo();

	gen = mt19937(rd());
	uniform_int_distribution<int> disX(_mapInfo->GetMosterMap().StartX(), _mapInfo->GetMosterMap().EndX());
	uniform_int_distribution<int> disY(_mapInfo->GetMosterMap().StartY(), _mapInfo->GetMosterMap().EndY());

	for (int32 i = 1; i <= _monsterSize; i++)
	{
		int32 uuid = i * (-1);
		int32 startX = disX(gen);
		int32 startY = disY(gen);
		shared_ptr<BS_Monster_Info> info = make_shared<BS_Monster_Info>(uuid);
		info->SetSpawnPosition(startX, startY);
		info->SetPosition(startX, startY, 0);

		wstring name(L"몬스터");
		name.append(std::to_wstring(uuid));

		vector<BYTE> arrName;

		for (int32 i = 0; i < name.size(); i++)
		{
			BYTE bottom = static_cast<BYTE>(name[i] & 0x00FF);
			BYTE top = static_cast<BYTE>((name[i] & 0xFF00) >> 8);
			arrName.push_back(bottom);
			arrName.push_back(top);
		}

		uint16 arrNameLen = arrName.size();
		info->SetName(arrName.data(), arrNameLen);

		_monsterMap[uuid] = info;
	}

	SetTickCountRoom();
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

void BS_GameRoom::SetTickCountRoom()
{
	_tickTime = FunctionUtils::TimeUtils::GetTickCount64_OS();
}

uint64 BS_GameRoom::GetTickCountRoom()
{
	return FunctionUtils::TimeUtils::GetTickCount64_OS() - _tickTime;
}

void BS_GameRoom::InitMapInfo()
{
	// 일단 크기는 고정이다.
	_mapInfo = make_shared<BS_MapInfo>(30, 30);
}

void BS_GameRoom::SpanMoster()
{
	for (int i = 1; i <= _monsterSize; i++)
	{
		int32 uuid = i * (-1);
		if (!_monsterMap[uuid]->IsSpawned())
		{
			_monsterMap[uuid]->SetSpawn(true);
			// 포지션 리셋은 버리고 사망한 위치에서 스폰된다.
		}
	}
}

void BS_GameRoom::MoveMoster()
{
	BS_Protocol::BS_C_MOVE_LIST pkt;
	for (auto &iter : _monsterMap)
	{
		auto info = iter.second;
		if (info->IsSpawned())
		{
			int32 rot = disRotate(gen);
			int32 x = info->GetPosition().X * (cosf(rot));
			int32 y = info->GetPosition().Y * (sinf(rot));
			info->SetPosition(x, y, info->GetPosition().Z);
			BS_Protocol::BS_C_MOVE childPkt;
			childPkt.Code = info->GetCode();
			childPkt.Position.X = info->GetPosition().X;
			childPkt.Position.Y = info->GetPosition().Y;
			childPkt.Position.Z = info->GetPosition().Z;
			childPkt.Position.Yaw = info->GetPosition().Yaw;
			pkt.moveList.push_back(childPkt);
		}
	}

	SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
	Broadcast(sendBuffer);
}

void BS_GameRoom::RoomTask()
{
	// 스레드 루프 돌때 한번 MakeTask한다
	// 갱신타임 500ms에 한번씩 체크해서 스폰이랑 몬스터 움직이도록 한다.
	uint64 tick = FunctionUtils::TimeUtils::GetTickCount64_OS();
	if (tick - _tickTime > _taskTime)
	{
		bool curLoopTask = isLoopTask.load();
		if (!curLoopTask && isLoopTask.compare_exchange_strong(curLoopTask, true))
		{
			_tickTime = FunctionUtils::TimeUtils::GetTickCount64_OS();
			SpanMoster();
			MoveMoster();
			// 여기서 이제 클라한테 모두 뿌려줘야되
		}
	}
}

unordered_map<int32, shared_ptr<class BS_Monster_Info>> BS_GameRoom::GetMonsterMap()
{
	return _monsterMap;
}
