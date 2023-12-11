#pragma once
#include "../CoreLib/Pch.h"

class PacketUtils
{
public:
  template <typename T>
  static T *ReadBufferPtr(BYTE *buffer, int &offset);

  static BYTE *ReadBufferStr(uint8 *buffer, int &ptr, uint32 len);
};

template <typename T>
T *PacketUtils::ReadBufferPtr(BYTE *buffer, int &offset)
{
  int _offset = offset;
  offset += sizeof(T);
  T *temp = reinterpret_cast<T *>(&buffer[_offset]);
  return temp;
}

BYTE *PacketUtils::ReadBufferStr(BYTE *buffer, int &ptr, uint32 len)
{
  return &buffer[ptr];
}