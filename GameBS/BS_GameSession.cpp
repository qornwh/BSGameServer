
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
	uint16 id = header->id;

	// cout << " packet code : " << id << " header size : " << len << endl;
	switch (id)
	{
	case 1: // 로드
	{
		int offset = sizeof(PacketHeader);

		uint16 *type = PacketUtils::ReadBufferPtr<uint16>(buffer, offset);
		uint16 *nameLen = PacketUtils::ReadBufferPtr<uint16>(buffer, offset);

		BYTE *name = PacketUtils::ReadBufferStr(buffer, offset, *nameLen);

		CreatePlayerInfo();
		getPlayer()->SetName(name, *nameLen);
		getPlayer()->SetType(*type);

		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);

		if (room != nullptr)
		{
			JobRef job = make_shared<Job>(&BS_GameRoom::AddSession, room, static_pointer_cast<BS_GameSession>(shared_from_this()));
			room->PushJobQueue(job);
		}
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
		childPkt.Target = -1;
		pkt.moveList.emplace_back(childPkt);

		SendBufferRef sendBuffer = BS_PacketHandler::MakePacket(pkt);
		getPlayer()->SetPosition(Position->X, Position->Y, Position->Z, Position->Yaw);
		shared_ptr<BS_GameRoom> room = GBSRoomManger->getRoom(0);
		if (room != nullptr)
		{
			JobRef job = make_shared<Job>(&BS_GameRoom::BroadcastPushMessage, room, sendBuffer);
			room->PushJobQueue(job);
		}
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
	default:
		break;
	}
}

// 바뀔수 있음
void BS_GameSession::CreatePlayerInfo()
{
	_player = make_shared<BS_Player_Info>(getSocketFd());
}
