#pragma once
#include <mutex>
#include <iostream>
#include <atomic>
#include <cstdlib>
#include <memory>
#include <queue>
#include <array>
#include <unordered_map>

using namespace std;

using BYTE = unsigned char;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

template <typename T, uint32 Size>
using Array = std::array<T, Size>;

template <typename T>
using Atomic = std::atomic<T>;

using ServiceRef = shared_ptr<class Service>;
using ServerServiceRef = std::shared_ptr<class ServerService>;
using ServerSockRef = shared_ptr<class ServerSock>;
using SessionRef = shared_ptr<class Session>;
using SendBufferChunkRef = shared_ptr<class SendBufferChunk>;
using SendBufferRef = shared_ptr<class SendBuffer>;
using PacketSessionRef = shared_ptr<class PacketSession>;

using JobRef = shared_ptr<class Job>;

// BS로 나중에 옮김 - 코어에서 제외
using BS_GameServerServiceRef = shared_ptr<class BS_GameServerService>;