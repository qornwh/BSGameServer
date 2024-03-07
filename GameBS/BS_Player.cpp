#include "BS_Player.h"
#include "BS_MapInfo.h"
#include "../CoreLib/Utils.h"
#include <random>

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

void BS_Unit_Info::TakeDemage(int32 demage)
{
	_hp -= demage;
	if (_hp <= 0)
		SetSpawn(false);
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

BS_Monster_Info::BS_Monster_Info(int32 uuid) : BS_Unit_Info(uuid)
{
}

void BS_Monster_Info::SetInfo(uint16 type, int32 hp)
{
	SetType(type);
	SetHp(hp);
}

void BS_Monster_Info::SetAttackPlayerUUid(int32 uuid)
{
	_targetPlayerUUid = uuid;
}

bool BS_Monster_Info::MoveTarget(FVector &targetPosition)
{
	if (_attaking)
		return false; // 공격중이면 통과
	if (CheckAttackTarget(targetPosition))
	{
		// 먼저 공격 범위 내에 있으면 공격
		_attaking = true;
		return true;
	}
	else
	{
		/* 일단 보류
		// 플레이어 위치 까지 이동 250. astar
		// 영역을 쪼개서 구간별로 나눠서 구간자체를 벽으로 설정해야될듯함!!!
		// 쪼개는 기준이 어떻게 될것인가???? 되나?? // 그전에 쪼갤때 간격 100x100 여기에 유닛 들어감
		int X = GetPosition().X;
		int Y = GetPosition().Y;
		float Yaw = GetPosition().Yaw;

		// 변형된 astar를 생각해본다.
		// 케이스 1 : 바로 직선으로 이동할경우 벽이 체크되지 않으면 바로 이동.
		// 케이스 2 : 만약 최선거리에서 벽을 마주할 경우, (좌or우)상단 (좌or우)하단. 으로 이동한다.

		// 9 칸으로 나눠 본다.
		// 벽인지 체크는 ??
		int TopX = X + 200;
		int BottomX = X - 200;
		int TopY = Y + 200;
		int BottomY = Y - 200;
		*/

		float delX = targetPosition.X - _position.X;
		float delY = targetPosition.Y - _position.Y;

		float radian = atan2(delY, delX);

		_position.X += (_distence * cos(radian));
		_position.Y += (_distence * sin(radian));
		return false;
	}
}

bool BS_Monster_Info::CheckAttackTarget(FVector &targetPosition)
{
	// 몬스터 좌표에 원안 체크
	int32 radius = 200;
	int32 dist = sqrt(pow(_position.X - targetPosition.X, 2) + pow(_position.Y - targetPosition.Y, 2));
	if (radius >= dist)
	{
		float theta = FunctionUtils::Utils::calculateAngle(_position.X, _position.Y, targetPosition.X, targetPosition.Y);
		_position.Yaw = theta;
		return true;
	}

	return false;
}

void BS_Monster_Info::AttackTarget()
{
}

void BS_Monster_Info::SetMoving(bool isMoving)
{
	_moving = isMoving;
}

bool BS_Monster_Info::IsMoving()
{
	return _moving && !_attaking;
}

void BS_Monster_Info::ResetSpawn()
{
	_position.X = _startX;
	_position.Y = _startY;
	_position.Z = 100;
	_position.Yaw = 0;
	_hp = 100;
	_targetPlayerUUid = -1;
}

bool BS_Monster_Info::IsAttacking()
{
	return _attaking;
}

void BS_Monster_Info::SetTick()
{
	if (_attaking)
	{
		_tick = (_tick + 1) % 4;
		if (_tick == 0)
			_attaking = false;
	}
}

int32 BS_Monster_Info::GetAttackPlayerUUid()
{
	return _targetPlayerUUid;
}

bool BS_Monster_Info::OnTarget(shared_ptr<BS_Player_Info> playerInfo, shared_ptr<BS_MapInfo> mapInfo)
{
	if (_targetPlayerUUid > -1)
	{
		return mapInfo->InRect(playerInfo->GetPosition().X, playerInfo->GetPosition().Y, mapInfo->GetMap());
	}
	return false;
}

void BS_Monster_Info::SetSpawnPosition(int32 startX, int32 startY)
{
	_startX = startX;
	_startY = startY;
}

BS_Player_Info::BS_Player_Info(int32 uuid) : BS_Unit_Info(uuid)
{
}
