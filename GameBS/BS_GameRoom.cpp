#include "BS_GameRoom.h"
#include "BS_Player.h"
#include "../CoreLib/Utils.h"
#include "BS_GameSession.h"
#include "BS_MapInfo.h"
#include "BS_Packet.h"
#include <random>
#include "BS_PacketHandler.h"

BS_GameRoom::BS_GameRoom(int roomId) : _roomId(roomId), _tickTime(FunctionUtils::Utils::GetTickCount64_OS()), JobQueue()
{
	InitMapInfo();

	gen = mt19937(rd());
	uniform_int_distribution<int> disX(_mapInfo->GetMap().StartX() + 1000, _mapInfo->GetMap().EndX() - 1000);
	uniform_int_distribution<int> disY(_mapInfo->GetMap().StartY() + 1000, _mapInfo->GetMap().EndY() - 1000);

	for (int32 i = 1; i <= _monsterSize; i++)
	{
		int32 uuid = i * (-1);
		int32 startX = disX(gen);
		int32 startY = disY(gen);
		shared_ptr<BS_Monster_Info> info = make_shared<BS_Monster_Info>(uuid);
		info->SetType(10 + (uuid % 5) * (-1));
		info->SetSpawnPosition(startX, startY);
		info->SetPosition(startX, startY, 100);

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

void BS_GameRoom::BroadcastPushMessage(SendBufferRef SendBuffer)
{
	// 일단 세션에 메시지만 push하는 용도
	for (auto pair : _sessionMap)
	{
		pair.second->PushBuffer(SendBuffer);
	}
}

void BS_GameRoom::PushJobQueue(JobRef job)
{
	Push(job);
}

void BS_GameRoom::SetTickCountRoom()
{
	_tickTime = FunctionUtils::Utils::GetTickCount64_OS();
}

uint64 BS_GameRoom::GetTickCountRoom()
{
	return FunctionUtils::Utils::GetTickCount64_OS() - _tickTime;
}

void BS_GameRoom::InitMapInfo()
{
	// 일단 크기는 고정이다.
	_mapInfo = make_shared<BS_MapInfo>(5, 5);
}

void BS_GameRoom::SpanMoster()
{
	for (int i = 1; i <= _monsterSize; i++)
	{
		int32 uuid = i * (-1);
		if (!_monsterMap[uuid]->IsSpawned())
		{
			_monsterMap[uuid]->ResetSpawn();
			_monsterMap[uuid]->SetSpawn(true);
			// 포지션 리셋은 버리고 사망한 위치에서 스폰된다.
		}
	}
}

void BS_GameRoom::MoveMosters()
{
	BS_Protocol::BS_C_MOVE_LIST pkt;
	BS_Protocol::BS_ATTACK_UNIT_LIST attackPkt;
	for (auto &iter : _monsterMap)
	{
		auto info = iter.second;
		int32 playerUUid = info->GetAttackPlayerUUid();
		info->SetTick();
		if (info->IsSpawned() && info->IsMoving())
		{
			if (playerUUid > -1 && _sessionMap.find(playerUUid) != _sessionMap.end() && info->OnTarget(_sessionMap[playerUUid]->getPlayer(), _mapInfo))
			{
				// 타깃 추적
				shared_ptr<BS_Player_Info> playerInfo = _sessionMap[playerUUid]->getPlayer();
				bool IsAttack = info->MoveTarget(playerInfo->GetPosition());
				bool InRect = _mapInfo->InRect(info->GetPosition().X, info->GetPosition().Y, _mapInfo->GetMap());
				if (InRect && IsAttack)
				{
					BS_Protocol::BS_ATTACK_UNIT attackChildPkt;
					attackChildPkt.Code = info->GetCode();
					attackChildPkt.SkillCode = info->GetSkillCode();
					attackPkt.attackList.push_back(attackChildPkt);
					continue;
				}
			}
			else
			{
				int32 Yaw = info->GetPosition().Yaw;
				if (_tick == 1)
					Yaw = disRotate(gen);
				int32 X = info->GetPosition().X + (info->GetDistence() * (cos(Yaw / 3.14)));
				int32 Y = info->GetPosition().Y + (info->GetDistence() * (sin(Yaw / 3.14)));
				_mapInfo->InMonsterRect(X, Y);
				info->SetPosition(X, Y, info->GetPosition().Z, Yaw);
				// 그냥 이동이면 타깃 지운다.
				playerUUid = -1;
			}

			BS_Protocol::BS_C_MOVE childPkt;
			childPkt.Code = info->GetCode();
			childPkt.Position.X = info->GetPosition().X;
			childPkt.Position.Y = info->GetPosition().Y;
			childPkt.Position.Z = info->GetPosition().Z;
			childPkt.Position.Yaw = info->GetPosition().Yaw;
			childPkt.Target = playerUUid;
			pkt.moveList.push_back(childPkt);
		}
		else
		{
			info->SetMoving(true);
		}
	}
	if (attackPkt.attackList.size() > 0)
	{
		SendBufferRef attackSendBuffer = BS_PacketHandler::MakePacket(attackPkt);
		BroadcastPushMessage(attackSendBuffer);
	}
	SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
	Broadcast(sendBuffer);
	_tick = (_tick + 1) % 2;
}

void BS_GameRoom::RoomTask()
{
	// 스레드 루프 돌때 한번 MakeTask한다
	// 갱신타임 500ms에 한번씩 체크해서 스폰이랑 몬스터 움직이도록 한다.
	uint64 tick = FunctionUtils::Utils::GetTickCount64_OS();
	if (tick - _tickTime > _taskTime)
	{
		bool curLoopTask = isLoopTask.load();
		if (!curLoopTask && isLoopTask.compare_exchange_strong(curLoopTask, true))
		{
			SpanMoster();
			MoveMosters();
			//  여기서 이제 클라한테 모두 뿌려줘야되
			_tickTime = FunctionUtils::Utils::GetTickCount64_OS();
			isLoopTask.exchange(false);
		}
	}
}

unordered_map<int32, shared_ptr<class BS_Monster_Info>> BS_GameRoom::GetMonsterMap()
{
	return _monsterMap;
}

void BS_GameRoom::MonsterHit(SendBufferRef sendBuffer, int32 MonsterCode, int32 socketFd, int32 Demage)
{
	if (_monsterMap.find(MonsterCode) != _monsterMap.end())
	{
		// 일단 몬스터 히트될때 경직시간은 나중 이동될 거리를 줄이는것으로 간다.
		_monsterMap[MonsterCode]->SetMoving(false);
		_monsterMap[MonsterCode]->TakeDemage(Demage);
		if (!_monsterMap[MonsterCode]->DieUnit())
		{
			_monsterMap[MonsterCode]->SetAttackPlayerUUid(socketFd);
		}
		Broadcast(sendBuffer);
	}
}
