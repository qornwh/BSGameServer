#include "BS_MapInfo.h"
#include "BS_Player.h"

BS_MapInfo::BS_MapInfo(int32 x, int32 y) : _mapInfo(x * SCALE - 100, y * SCALE - 100)
{
  _monsterInfo.X = 3 * SCALE;
  _monsterInfo.Y = 3 * SCALE;

  _monsterInfo.CenterX = 500;
  _monsterInfo.CenterY = 500;
}

BS_MapInfo::~BS_MapInfo()
{
  cout << "~BS_MapInfo" << endl;
}

void BS_MapInfo::InMonsterRect(int32 &x, int32 &y)
{
  // 몬스터 영역 튀어나오는 값이면 제일 끝값으로 교체한다.
  if (_monsterInfo.StartX() > x)
  {
    x = _monsterInfo.StartX();
  }
  else if (_monsterInfo.EndX() < x)
  {
    x = _monsterInfo.EndX();
  }

  if (_monsterInfo.StartY() > x)
  {
    y = _monsterInfo.StartY();
  }
  else if (_monsterInfo.EndY() < x)
  {
    y = _monsterInfo.EndY();
  }
}

bool BS_MapInfo::InRect(int32 x, int32 y, Rect &rect)
{
  if (rect.StartX() < x && rect.EndX() > x && rect.StartY() > y && rect.EndY() < y)
    return true;

  return false;
}
