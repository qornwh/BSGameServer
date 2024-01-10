#include "BS_MapInfo.h"
#include "BS_Player.h"

BS_MapInfo::BS_MapInfo(int32 x, int32 y) : _mapInfo(x * SCALE - 100, y * SCALE - 100)
{
}

BS_MapInfo::~BS_MapInfo()
{
  cout << "~BS_MapInfo" << endl;
}

bool BS_MapInfo::InMonsterRect(int32 &x, int32 &y)
{
  bool flag = false;
  // 몬스터 영역 튀어나오는 값이면 제일 끝값으로 교체한다.
  if (_mapInfo.StartX() > x)
  {
    x = _mapInfo.StartX();
    flag = true;
  }
  else if (_mapInfo.EndX() < x)
  {
    x = _mapInfo.EndX();
    flag = true;
  }

  if (_mapInfo.StartY() > y)
  {
    y = _mapInfo.StartY();
    flag = true;
  }
  else if (_mapInfo.EndY() < y)
  {
    y = _mapInfo.EndY();
    flag = true;
  }

  return flag;
}

bool BS_MapInfo::InRect(int32 x, int32 y, Rect &rect)
{
  if (rect.StartX() < x && rect.EndX() > x && rect.StartY() < y && rect.EndY() > y)
    return true;

  return false;
}
