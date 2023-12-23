#include "BS_Player.h"

BS_Unit_Info::BS_Unit_Info(int32 uuid) : _uuid(uuid)
{
	_hp = 100;
	_type = 1;
}

BS_Unit_Info::~BS_Unit_Info()
{
	cout << "close player info" << endl;
}

void BS_Unit_Info::SetName(BYTE *name, uint16 &nameLen)
{
	_name = new BYTE[nameLen];
	memcpy(_name, name, nameLen);
	SetNameLen(nameLen);
}

void BS_Unit_Info::SetNameLen(uint16 nameLen)
{
	_nameLen = nameLen;
}

void BS_Unit_Info::SetPosition(FVector &position)
{
	_position.X = position.X;
	_position.Y = position.Y;
	_position.Z = position.Z;
}

void BS_Unit_Info::SetPosition(int32 X, int32 Y, int32 Z)
{
	_position.X = X;
	_position.Y = Y;
	_position.Z = Z;
}

void BS_Unit_Info::SetPosition(int32 X, int32 Y, int32 Z, float Yaw)
{
	_position.X = X;
	_position.Y = Y;
	_position.Z = Z;
	_position.Yaw = Yaw;
}

void BS_Unit_Info::SetType(uint16 type)
{
	_type = type;
}

void BS_Unit_Info::SetHp(int32 hp)
{
	_hp = hp;
}

void BS_Unit_Info::HitUnit(int32 Damage)
{
	_hp -= Damage;
}

bool BS_Unit_Info::DieUnit()
{
	return _hp > 0 ? false : true;
}

void BS_Unit_Info::SetSpawn(bool spawnd)
{
	_isSpawned = spawnd;
}

BS_Player_Info::BS_Player_Info(int32 socketFd) : _socketFd(socketFd), BS_Unit_Info(socketFd)
{
}

BS_Monster_Info::BS_Monster_Info(int32 uuid) : BS_Unit_Info(uuid)
{
}

void BS_Monster_Info::SetInfo(uint16 type, int32 hp)
{
	SetType(type);
	SetHp(hp);
}

void BS_Monster_Info::SetSpawnPosition(int32 startX, int32 startY)
{
	_startX = startX;
	_startY = startY;
}
