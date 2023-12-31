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

void BS_Monster_Info::SetAttackPlayerUUid(int32 uuid)
{
	_targetPlayerUUid = uuid;
}

bool BS_Monster_Info::MoveTarget(FVector &targetPosition)
{
	float theta = FunctionUtils::Utils::calculateAngle(_position.X, _position.Y, targetPosition.X, targetPosition.Y);

	if (CheckAttackTarget(targetPosition))
	{
		// 먼저 공격 범위 내에 있으면 공격
		AttackTarget();
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

		_position.X = targetPosition.X;
		_position.Y = targetPosition.Y;
		_position.Yaw += theta;
		return false;
	}
}

bool BS_Monster_Info::CheckAttackTarget(FVector &targetPosition)
{
	// 일단 범위 하드코딩 => 나중에 공격 스킬 범위를 지정해둔다.. 그럼따라서 파라미터 추가 필요할듯
	const int x = 200; // 범위
	const int y = 300; // 범위
	int startX = _position.X - abs((x * (cos(45))));
	int endX = _position.X + abs((x * (cos(45))));
	int startY = _position.Y - abs((y * (sin(45))));
	int endY = _position.Y + abs((y * (sin(45))));
	cout << "target : " << targetPosition.X << " : " << targetPosition.Y << endl;
	cout << "check " << startX << ", " << endX << ", " << startY << ", " << endY << endl;
	if (startX <= targetPosition.X && targetPosition.X <= endX && startY <= targetPosition.Y && targetPosition.Y <= endY)
	{
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
	return _moving;
}

int32 BS_Monster_Info::GetAttackPlayerUUid()
{
	return _targetPlayerUUid;
}

bool BS_Monster_Info::OnTarget(shared_ptr<BS_Player_Info> playerInfo, shared_ptr<BS_MapInfo> mapInfo)
{
	// 일단 여기에 타겟 체크도 둔다.
	if (_targetPlayerUUid > -1)
	{
		return mapInfo->InRect(playerInfo->GetPosition().X, playerInfo->GetPosition().Y, mapInfo->GetMosterMap());
	}
	return false;
}

void BS_Monster_Info::SetSpawnPosition(int32 startX, int32 startY)
{
	_startX = startX;
	_startY = startY;
}
