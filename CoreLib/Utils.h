#pragma once

#include <cmath>
#include "Pch.h"

namespace FunctionUtils
{
  // 시간관련 static 함수
  class Utils
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
    // 벡터 내적 공식 사용
    static double dotProduct(float x1, float y1, float x2, float y2)
    {
      return x1 * x2 + y1 * y2;
    }
    // 벡터 외적 공식 사용
    static double crossProduct(double x1, double y1, double x2, double y2)
    {
      return x1 * y2 - x2 * y1;
    }
    // 벡터 크기
    static double magnitude(float x, float y)
    {
      return sqrt(x * x + y * y);
    }
    // 벡터 정규화
    static void normalizeVector(float &x, float &y)
    {
      float mag = magnitude(x, y);

      // 벡터의 크기가 0이 아닌 경우에만 정규화 수행
      if (mag != 0.0)
      {
        x /= mag;
        y /= mag;
      }
    }
    // 라디안을 각도로 변환하는 함수
    static double radianToDegree(double radian)
    {
      return radian * 180.0 / M_PI;
    }

    // 두 벡터 간의 각도를 계산하는 함수 (라디안 단위)
    static float calculateAngle(float x1, float y1, float x2, float y2)
    {
      float x = x2 - x1;
      float y = y2 - y1;
      float x_forward = 1;
      float y_forward = 0;
      normalizeVector(x, y);
      double dot = dotProduct(x_forward, y_forward, x, y);

      double radian = acos(dot);
      float angleRad = radianToDegree(radian);

      if (crossProduct(x_forward, y_forward, x, y) > 0)
      {
        return angleRad;
      }
      else
      {
        return angleRad * (-1);
      }
    }
  };
}