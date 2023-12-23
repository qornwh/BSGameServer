#pragma once

#include "Pch.h"

namespace FunctionUtils
{
  // 시간관련 static 함수
  class TimeUtils
  {
  public:
    // 리눅스에 없어서 만들어서 사용 => 나중에 매크로로 바꾸자
    static uint64 GetTickCount64_OS()
    {
      uint64 tick;
      // #if defined(WIN32) || defined(WIN64)
      //       tick = GetTickCount64();
      // #else
      struct timespec tp;
      clock_gettime(CLOCK_MONOTONIC, &tp);
      tick = (tp.tv_sec * 1000ull) + (tp.tv_nsec / 1000ull / 1000ull);
      // #endif
      return tick;
    }
  };
}