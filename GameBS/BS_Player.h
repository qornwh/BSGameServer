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

class BS_Player_Info : public enable_shared_from_this<BS_Player_Info>
{
public:
	BS_Player_Info(int socketFd);
	~BS_Player_Info();

public:
	void SetName(BYTE *id, uint16 &idLen);
	void SetNameLen(uint16 nameLen);
	// void SetPw(BYTE* pw, uint16& pwLen);
	void SetPosition(FVector &position);
	void SetPosition(int32 X, int32 Y, int32 Z);
	void SetType(uint16 type);

	// BYTE* GetId() { return _id; }
	// BYTE* GetPw() { return _pw; }
	BYTE *GetName() { return _name; }
	BYTE GetCode() { return _uuid; }
	uint16 GetType() { return _type; }
	BYTE GetHp() { return _hp; }
	BYTE GetNameLen() { return _nameLen; }
	FVector &GetPosition() { return _position; }

private:
	int _socketFd;

private:
	// BYTE _id[10];
	// BYTE _pw[10];
	BYTE *_name;
	BYTE _uuid;

	uint16 _nameLen;

	int32 _hp;
	uint16 _type;

	FVector _position{0, 0, 0, 0};
	// uuid 필요
};
