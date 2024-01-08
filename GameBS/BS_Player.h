#pragma once
#include <cstring>

#include "../CoreLib/Pch.h"

struct FVector
{
	FVector(int32 X, int32 Y, int32 Z, float Yaw) : X(X), Y(Y), Z(Z), Yaw(Yaw){};

	int32 X;
	int32 Y;
	int32 Z;
	float Yaw;
};

class BS_Unit_Info : public enable_shared_from_this<BS_Unit_Info>
{
public:
	BS_Unit_Info(int32 uuid);
	~BS_Unit_Info();

public:
	void SetName(BYTE *name, uint16 &nameLen);
	void SetNameLen(uint16 nameLen);
	void SetPosition(FVector &position);
	void SetPosition(int32 X, int32 Y, int32 Z);
	void SetPosition(int32 X, int32 Y, int32 Z, float Yaw);
	void SetType(uint16 type);
	void SetHp(int32 hp);
	void TakeDemage(int32 demage);

	BYTE *GetName() { return _name; }
	int32 GetCode() { return _uuid; }
	uint16 GetType() { return _type; }
	BYTE GetHp() { return _hp; }
	BYTE GetNameLen() { return _nameLen; }
	FVector &GetPosition() { return _position; }

	void HitUnit(int32 Damage);
	bool DieUnit();
	void SetSpawn(bool spawnd);
	bool IsSpawned() { return _isSpawned; }

protected:
	BYTE *_name;
	int32 _uuid;

	uint16 _nameLen;

	int32 _hp;
	uint16 _type;

	FVector _position{0, 0, 0, 0};

	// 처음생성하면 스폰되어야 되므로 true로 둔다!!!
	bool _isSpawned = true;
};

// 일단 socketFilediscriptor와 uuid 는 같이간다.
class BS_Player_Info : public BS_Unit_Info
{
public:
	BS_Player_Info(int32 uuid);
	//~BS_Player_Info();
};

// 일단 uuid 는 음의 정수로 간다.
class BS_Monster_Info : public BS_Unit_Info
{
public:
	BS_Monster_Info(int32 uuid);
	//~BS_Monster_Info();

	void SetSpawnPosition(int32 startX, int32 startY);
	void SetInfo(uint16 type, int32 hp);
	void SetAttackPlayerUUid(int32 uuid);
	int32 GetAttackPlayerUUid();
	bool OnTarget(shared_ptr<BS_Player_Info> playerInfo, shared_ptr<class BS_MapInfo> mapInfo);

	// 공격 가능 범위까지 이동(회전 부터, astar)
	bool MoveTarget(FVector &targetPosition);
	// 공격 가능 범위 체크
	bool CheckAttackTarget(FVector &targetPosition);
	// 공격
	void AttackTarget();

	void SetMoving(bool isMoving);
	bool IsMoving();

	bool IsAttacking();
	void SetTick();

	int32 GetSkillCode() { return _skillCode; }
	int32 GetDistence() { return _distence; }

	void ResetSpawn();

private:
	int32 _startX = 0;
	int32 _startY = 0;
	bool _moving = true;
	bool _attaking = false;
	int32 _skillCode = 1;

private:
	int32 _targetPlayerUUid = -1; // 공격할 대상
	int32 _distence = 150;				// 한번에 이동할 거리
	int32 _tick = -1;
};
