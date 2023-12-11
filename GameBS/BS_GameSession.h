#pragma once
#include "../CoreLib/Pch.h"
#include "../CoreLib/Session.h"
#include "BS_Player.h"

class BS_GameSession : public Session
{
public:
	BS_GameSession(int socketFd);
	~BS_GameSession();

public:
	int32 OnRecv(BYTE *buffer, int32 len);
	void HandlePacket(BYTE *buffer, int32 len);
	void CreatePlayerInfo();
	shared_ptr<BS_Player_Info> getPlayer() { return _player; }

private:
	shared_ptr<BS_Player_Info> _player;
};
