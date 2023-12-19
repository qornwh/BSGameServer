#pragma once

#include "../CoreLib/Pch.h"

using GameRoomRef = shared_ptr<class BS_GameRoom>;

class BS_GameRoomManger : public enable_shared_from_this<BS_GameRoomManger>
{
public:
	enum RoomType
	{
		space, // 지역
		paty,	 // 파티
	};

	BS_GameRoomManger();
	~BS_GameRoomManger();
	void CreateRoom();
	void EreaseRoom(int32 id);
	GameRoomRef getRoom(int32 id);

private:
	Atomic<int32> _id{0};
	unordered_map<int32, GameRoomRef> _roomMap;
};

extern shared_ptr<BS_GameRoomManger> GBSRoomManger;