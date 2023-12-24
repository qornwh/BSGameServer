#pragma once
#include <cstring>

#include "../CoreLib/Pch.h"
#include "BS_Packet.h"
#include "../CoreLib/PacketHeader.h"
#include "../CoreLib/SendBuffer.h"

class BufferWrite
{
public:
	BufferWrite()
	{
	}

	BufferWrite(BYTE *buffer, uint16 size, uint16 pos = 0) : _buffer(buffer), _size(size), _pos(pos)
	{
	}

	~BufferWrite()
	{
	}

public:
	BYTE *Buffer() { return _buffer; }
	uint16 Size() { return _size; }
	uint16 WriteSize() { return _pos; }
	uint16 FreeSize() { return _size - _pos; }

public:
	template <typename T>
	bool Write(T *src) { return Write(src, sizeof(T)); }

	bool Write(void *src, uint16 len)
	{
		if (FreeSize() < len)
			return false;

		memcpy(&_buffer[_pos], src, len);
		_pos += len;
		return true;
	}

private:
	BYTE *_buffer = nullptr;
	uint16 _size = 0;
	uint16 _pos = 0;
};

class BS_PacketHandler
{
public:
	enum PacketCode
	{

	};

	// 플레이어 이름
	static SendBufferRef MakePacket(BS_Protocol::BS_USER_DATA &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 1);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);

		for (auto user : pkt.Users)
		{
			bw.Write(&user.Code);
			bw.Write(&user.Name);
			bw.Write(user.Name, user.NameLen);
		}

		return sendBuffer;
	}

	// 유저 정보 전달
	static SendBufferRef MakePacket(BS_Protocol::BS_LOAD_DATA &pkt)
	{
		uint16 playerSize = pkt.players.size();
		uint16 monsterSize = pkt.monsters.size();
		// const uint16 dataSize = pkt.size() + playerSize + monsterSize; // 안필요해 보임 playerSize + monsterSize
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 1);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);

		bw.Write(&playerSize);
		for (auto player : pkt.players)
		{
			bw.Write(&player.Type);
			bw.Write(&player.Hp);
			bw.Write(&player.Code);
			bw.Write(&player.Position.X);
			bw.Write(&player.Position.Y);
			bw.Write(&player.Position.Z);
			bw.Write(&player.Position.Yaw);
			bw.Write(&player.NameLen);
			bw.Write(player.Name, player.NameLen);
		}

		bw.Write(&monsterSize);
		for (auto monster : pkt.monsters)
		{
			bw.Write(&monster.Type);
			bw.Write(&monster.Hp);
			bw.Write(&monster.Code);
			bw.Write(&monster.Position.X);
			bw.Write(&monster.Position.Y);
			bw.Write(&monster.Position.Z);
			bw.Write(&monster.Position.Yaw);
			bw.Write(&monster.NameLen);
			bw.Write(monster.Name, monster.NameLen);
		}
		return sendBuffer;
	}

	// 유저 추가 메시지
	static SendBufferRef MakePacket(BS_Protocol::Player &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 2);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		bw.Write(&pkt.Type);
		bw.Write(&pkt.Hp);
		bw.Write(&pkt.Code);
		bw.Write(&pkt.NameLen);
		bw.Write(pkt.Name, pkt.NameLen);
		return sendBuffer;
	}

	// 이동 메시지
	static SendBufferRef MakePacket(BS_Protocol::BS_C_MOVE_LIST &pkt)
	{
		uint16 moveSize = pkt.moveList.size();
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 3);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);

		bw.Write(&moveSize);
		for (auto unit : pkt.moveList)
		{
			bw.Write(&unit.Code);
			bw.Write(&unit.Position.X);
			bw.Write(&unit.Position.Y);
			bw.Write(&unit.Position.Z);
			bw.Write(&unit.Position.Yaw);
		}
		return sendBuffer;
	}

	// 유저 채팅 메시지
	static SendBufferRef MakePacket(BS_Protocol::BS_C_CHAT &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 4);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		bw.Write(&pkt.Code);
		bw.Write(&pkt.Type);
		bw.Write(&pkt.TextLen);
		bw.Write(pkt.Text, pkt.TextLen);
		return sendBuffer;
	}

	// 내 정보 메시지
	static SendBufferRef MakeMyPacket(BS_Protocol::Player &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 5);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		bw.Write(&pkt.Type);
		bw.Write(&pkt.Hp);
		bw.Write(&pkt.Code);
		bw.Write(&pkt.NameLen);
		bw.Write(pkt.Name, pkt.NameLen);
		return sendBuffer;
	}

	// 유저 종료 메시지
	static SendBufferRef MakePacket(BS_Protocol::BS_CLOSE_PLAYER &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 6);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		bw.Write(&pkt.Code);
		return sendBuffer;
	}

	// 유저 스킬 메시지
	static SendBufferRef MakePacket(BS_Protocol::BS_ATTACK_PLAYER &pkt)
	{
		const uint16 dataSize = pkt.size();
		SendBufferRef sendBuffer = MakeSendBuffer(dataSize, 7);

		BufferWrite bw(sendBuffer->Buffer() + sizeof(PacketHeader), dataSize);
		bw.Write(&pkt.Code);
		bw.Write(&pkt.SkillCode);
		return sendBuffer;
	}

	// 헤더
	static SendBufferRef MakeSendBuffer(uint16 dataSize, uint16 pktId)
	{
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManger->Open(packetSize);
		PacketHeader *header = reinterpret_cast<PacketHeader *>(sendBuffer->Buffer());
		header->id = pktId;
		header->size = packetSize;

		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};