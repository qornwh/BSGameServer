#pragma once
#include "../CoreLib/Pch.h"

struct Rect
{
	Rect(int32 x, int32 y) : X(x), Y(y) {}
	Rect(int32 x, int32 y, int32 centerX, int32 centerY) : X(x), Y(y), CenterX(centerX), CenterY(centerY) {}
	int32 X = 0;
	int32 Y = 0;
	int32 CenterX = 0;
	int32 CenterY = 0;

	int32 StartX()
	{
		return CenterX - X;
	}
	int32 StartY()
	{
		return CenterY - Y;
	}
	int32 EndX()
	{
		return CenterX + X;
	}
	int32 EndY()
	{
		return CenterY + Y;
	}
};

// 맵도 나중에 상속받아서 분리처야 될듯하다... 보스맵 일반몹맵 마을맵 => 오픈월드는 좀 힘들고 인스턴트 던전 형식으로 간다.
class BS_MapInfo : public enable_shared_from_this<BS_MapInfo>
{
	enum
	{
		SCALE = 500
	};

public:
	BS_MapInfo(int32 x, int32 y);
	~BS_MapInfo();

	bool InRect(int32 x, int32 y, Rect &rect);
	void InMonsterRect(int32 &x, int32 &y);

	Rect &GetMap() { return _mapInfo; }
	Rect &GetMosterMap() { return _monsterInfo; }

private:
	// 맵 전체 크기 계산은 width * 2, hight * 2, 중앙값 고정 한다. 0 0
	Rect _mapInfo{0, 0, 0, 0};

	// 몬스터 존재 영역 고정 한다.
	Rect _monsterInfo{0, 0, 0, 0};
};
