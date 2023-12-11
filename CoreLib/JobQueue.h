#pragma once
#include "Pch.h"
#include "LockUtils.h"

// 이거는 풀링하지 않겠다. 그냥 룸생성시 같이 생성되도록 구현 !!!
class JobQueue : public enable_shared_from_this<JobQueue>
{
private:
public:
	JobQueue(){};
	~JobQueue(){};
	void Push(JobRef job);
	void Execute();

private:
	queue<JobRef> _q;
	Atomic<int32> _qSize{0};
	Atomic<bool> _isThreadUsed{false};
	// USE_LOCK;
	Lock lock;
};
