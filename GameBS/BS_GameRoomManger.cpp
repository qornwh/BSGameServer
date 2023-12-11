#include "BS_GameRoomManger.h"
#include "BS_GameRoom.h"

shared_ptr<BS_GameRoomManger> GBSRoomManger = make_shared<BS_GameRoomManger>();

BS_GameRoomManger::BS_GameRoomManger()
{
	CreateRoom();
}

BS_GameRoomManger::~BS_GameRoomManger()
{
	cout << "~BS_GameRoomManger" << endl;
}

void BS_GameRoomManger::CreateRoom()
{
	// 디폴트용 맵 생성
	GameRoomRef room = make_shared<BS_GameRoom>(_id);
	_roomMap[_id] = room;
	_id.fetch_add(1);
}

void BS_GameRoomManger::EreaseRoom(int32 id)
{
	if (_roomMap[id])
		_roomMap.erase(id);
}

GameRoomRef BS_GameRoomManger::getRoom(int32 id)
{
	return _roomMap[id];
}
