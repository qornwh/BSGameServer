#pragma once
#include "Pch.h"

struct PlayerInfo
{
	PlayerInfo(uint8 Type, uint8 Hp, uint32 Code) : Type(Type), Hp(Hp), Code(Code)
	{
	};

	uint8 Type;
	uint8 Hp;
	uint32 Code;
};
//
//struct FVector
//{
//	FVector(int32 X, int32 Y, int32 Code) : X(X), Y(Y)
//	{
//	};
//
//	int32 X;
//	int32 Y;
//	int32 Z;
//};