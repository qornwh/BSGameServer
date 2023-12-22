#include "BS_Player.h"

BS_Player_Info::BS_Player_Info(int socketFd) : _socketFd(socketFd)
{
	_uuid = socketFd;
	_hp = 1;
	_type = 1;
}

BS_Player_Info::~BS_Player_Info()
{
	cout << "close player info" << endl;
}

void BS_Player_Info::SetName(BYTE *name, uint16 &nameLen)
{
	_name = new BYTE[nameLen];
	memcpy(_name, name, nameLen);
	SetNameLen(nameLen);
}

void BS_Player_Info::SetNameLen(uint16 nameLen)
{
	_nameLen = nameLen;
}

// void BS_Player_Info::SetPw(BYTE* pw, uint16& pwLen)
//{
//	memcpy(_pw, pw, pwLen);
// }

void BS_Player_Info::SetPosition(FVector &position)
{
	_position.X = position.X;
	_position.Y = position.Y;
	_position.Z = position.Z;
}

void BS_Player_Info::SetPosition(int32 X, int32 Y, int32 Z)
{
	_position.X = X;
	_position.Y = Y;
	_position.Z = Z;
}

void BS_Player_Info::SetPosition(int32 X, int32 Y, int32 Z, float Yaw)
{
	_position.X = X;
	_position.Y = Y;
	_position.Z = Z;
	_position.Yaw = Yaw;
}

void BS_Player_Info::SetType(uint16 type)
{
	_type = type;
}
