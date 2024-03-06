#include "Pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"

void ThreadManager::ThreadTLS()
{
	static Atomic<uint32> SThreadId{1};
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::ThreadDestory()
{
	// 스마트 포인터 참조 제거
	LSendBufferChunk = nullptr;
}

void ThreadManager::ThreadJoinAll()
{
}
