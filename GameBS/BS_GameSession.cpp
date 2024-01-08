
#include <iostream>
#include <cstring>
#include <locale>
#include <iconv.h>

#include "../CoreLib/Pch.h"
#include "../CoreLib/PacketHeader.h"
#include "BS_GameSession.h"
#include "BS_Packet.h"
#include "BS_PacketHandler.h"
#include "../CoreLib/Job.h"
#include "BS_GameRoom.h"
#include "BS_GameRoomManger.h"
#include "BS_PacketUtils.h"

BS_GameSession::BS_GameSession(int socketFd) : Session(socketFd)
{
	// cout << "create GameSession" << endl;
}

BS_GameSession::~BS_GameSession()
{
	_player = nullptr;

	// cout << "disconnect GameSession !!!" << endl;
}

int32 BS_GameSession::OnRecv(BYTE *buffer, int32 len)
{
	int32 useLen = 0;
	while (true)
	{
		int32 dataSize = len - useLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader *header = reinterpret_cast<PacketHeader *>(&buffer[useLen]);
		if (dataSize < header->size)
		{
			cout << " header code : " << header->id << " header size : " << header->size << " data size : " << dataSize << endl;
			break;
		}

		// 패킷 메시지
		HandlePacket(&buffer[useLen], header->size);

		useLen += header->size;
	}

	return useLen;
}

void BS_GameSession::HandlePacket(BYTE *buffer, int32 len)
{
	PacketHeader *header = reinterpret_cast<PacketHeader *>(buffer);
	int id = header->id;
	BYTE *messagePeekStart = &buffer[sizeof(PacketHeader)];

	// cout << " packet code : " << id << " header size : " << len << endl;
	switch (id)
	{
	case 1: // 로드
	{
		// 일단 정리필요
		int offset = sizeof(PacketHeader);

		uint16 *type = PacketUtils::ReadBufferPtr<uint16>(buffer, offset);
		uint16 *nameLen = PacketUtils::ReadBufferPtr<uint16>(buffer, offset);

		BYTE *name = PacketUtils::ReadBufferStr(buffer, offset, *nameLen);

		CreatePlayerInfo();
		getPlayer()->SetName(name, *nameLen);
		getPlayer()->SetType(*type);

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		// 현재 접속된 모든 정보 전달. // 일단 gameroom에 넣어야된다. 락땜에
		{
			BS_Protocol::BS_LOAD_DATA pkt;

			for (auto &iter : GetService()->GetSessionMap())
			{
				int fd = iter.first;
				if (fd != getSocketFd())
				{
					shared_ptr<BS_GameSession> gameSession = static_pointer_cast<BS_GameSession>(iter.second);
					if (gameSession->getPlayer() != nullptr)
					{
						shared_ptr<BS_Player_Info> info = gameSession->getPlayer();
						BS_Protocol::Player player{info->GetType(), info->GetHp(), info->GetCode()};
						player.Type = info->GetType();
						player.Hp = info->GetHp();
						player.Code = info->GetCode();
						player.Position.X = info->GetPosition().X;
						player.Position.Y = info->GetPosition().Y;
						player.Position.Z = info->GetPosition().Z;
						player.Position.Yaw = info->GetPosition().Yaw;
						player.NameLen = info->GetNameLen();
						player.Name = info->GetName();
						pkt.players.push_back(player);
					}
				}
			}

			for (auto &iter : room->GetMonsterMap())
			{
				auto info = iter.second;
				if (info->IsSpawned())
				{
					BS_Protocol::Monster monster{info->GetType(), info->GetHp(), info->GetCode()};
					monster.Type = info->GetType();
					monster.Hp = info->GetHp();
					monster.Code = info->GetCode();
					monster.Target = info->GetAttackPlayerUUid();
					monster.Position.X = info->GetPosition().X;
					monster.Position.Y = info->GetPosition().Y;
					monster.Position.Z = info->GetPosition().Z;
					monster.Position.Yaw = info->GetPosition().Yaw;
					monster.NameLen = info->GetNameLen();
					monster.Name = info->GetName();
					pkt.monsters.push_back(monster);
				}
			}
			SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
			Send(sendBuffer);
			cout << "send packet data size : " << pkt.size() + sizeof(PacketHeader) << endl;
		}

		// 내정보를 모든 클라에게 전송 broadCast
		{
			shared_ptr<BS_Player_Info> info = getPlayer();
			BS_Protocol::Player pkt{info->GetType(), info->GetHp(), info->GetCode()};
			pkt.NameLen = info->GetNameLen();
			pkt.Name = info->GetName();

			{
				SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
				// 가상함수 선언해둠 캐스팅(GameServerService) 필요 x
				GetService()->Broadcast(sendBuffer, getSocketFd());
				// 내 원래 생각은 여기서 BS_GameServerService에 플레이어 정보 넣어두려 했음 그러나 캐스팅 어떻게 하는지 몰라 포기
			}

			{
				SendBufferRef sendBuffer = BS_PacketHandler::MakeMyPacket(pkt);
				// 가상함수 선언해둠 캐스팅(GameServerService) 필요 x
				Send(sendBuffer);
			}

			if (room != nullptr)
			{
				JobRef job = make_shared<Job>(&BS_GameRoom::AddSession, room, static_pointer_cast<BS_GameSession>(shared_from_this()));
				room->PushJobQueue(job);
			}
		}
	}
	break;
	case 2: // 테스트 클라이언트 !!
	{
		int offset = sizeof(PacketHeader);
	}
	break;
	case 3: // 이동 일단 그냥 한번 해보자 실시간으로 주나 그냥 올때마다 주나 차이가 없을지도 몰라
	{
		int offset = sizeof(PacketHeader);

		FVector *Position = PacketUtils::ReadBufferPtr<FVector>(buffer, offset);

		BS_Protocol::BS_C_MOVE_LIST pkt;

		BS_Protocol::BS_C_MOVE childPkt;
		childPkt.Code = getSocketFd();
		childPkt.Position.X = Position->X;
		childPkt.Position.Y = Position->Y;
		childPkt.Position.Z = Position->Z;
		childPkt.Position.Yaw = Position->Yaw;
		pkt.moveList.emplace_back(childPkt);

		// std::cout << " code : " << pkt.Code << " x : " << pkt.Position.X << " y : " << pkt.Position.Y << std::endl;

		SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
		GetService()->Broadcast(sendBuffer, getSocketFd());
		getPlayer()->SetPosition(Position->X, Position->Y, Position->Z, Position->Yaw);
	}
	break;
	case 4:
	{
		// 채팅
		int offset = sizeof(PacketHeader);

		uint8 *Type = PacketUtils::ReadBufferPtr<uint8>(buffer, offset);
		uint32 *TextLen = PacketUtils::ReadBufferPtr<uint32>(buffer, offset);
		BYTE *Text = PacketUtils::ReadBufferStr(buffer, offset, *TextLen);

		BS_Protocol::BS_C_CHAT pkt;
		pkt.Code = getSocketFd();
		pkt.Type = *Type;
		pkt.TextLen = *TextLen;
		pkt.Text = Text;

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		if (room != nullptr)
		{
			SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
			JobRef job = make_shared<Job>(&BS_GameRoom::Broadcast, room, sendBuffer);
			room->PushJobQueue(job);
		}
	}
	break;
	case 7:
	{
		// 공격
		int offset = sizeof(PacketHeader);
		int32 *SkillCode = PacketUtils::ReadBufferPtr<int32>(buffer, offset);

		BS_Protocol::BS_ATTACK_UNIT pkt;

		pkt.Code = getSocketFd();
		pkt.SkillCode = *SkillCode;

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		if (room != nullptr)
		{
			SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
			JobRef job = make_shared<Job>(&BS_GameRoom::Broadcast, room, sendBuffer);
			room->PushJobQueue(job);
		}
	}
	break;
	case 8:
	{
		// 공격 -> 히트 : 추가로 데미지 전달까지 완성되어야 된다!!
		int offset = sizeof(PacketHeader);
		int32 *TargetCode = PacketUtils::ReadBufferPtr<int32>(buffer, offset);
		int32 *AttackCode = PacketUtils::ReadBufferPtr<int32>(buffer, offset);

		BS_Protocol::BS_HIT_UNIT pkt;

		pkt.Damage = 10;
		pkt.TargetCode = *TargetCode;
		if (pkt.TargetCode < 0)
		{
			// 플레이어 -> 몬스터 공격
			pkt.AttackCode = _player->GetCode();
		}
		else
		{
			// 몬스터 -> 플레이어 공격
			pkt.AttackCode = *AttackCode;
		}

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		if (room != nullptr)
		{
			SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
			JobRef job = make_shared<Job>(&BS_GameRoom::MonsterHit, room, sendBuffer, *TargetCode, _player->GetCode(), pkt.Damage);
			room->PushJobQueue(job);
		}
	}
	break;
	case 9:
	{
		// 몬스터 kill, respone 메시지
	}
	break;
	case 10:
	{
		//
	}
	break;
	default:
		break;
	}
}

// 바뀔수 있음
void BS_GameSession::CreatePlayerInfo()
{
	_player = make_shared<BS_Player_Info>(getSocketFd());
}
