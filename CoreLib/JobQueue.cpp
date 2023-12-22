#include "JobQueue.h"
#include "Job.h"

void JobQueue::Push(JobRef job)
{
	_qSize.fetch_add(1);
	const int32 jobCount = _qSize;

	{
		WriteLockGuard wLock(lock, "JobQueue::Push");
		_q.push(job);
	}

	if (jobCount > 0)
	{
		if (!_isThreadUsed)
		{
			// 사용중 아닐경우 바로 Execute
			Execute();
		}
		else
		{
			// 사용중일경우 다른스레드로 넘기기 위해 글로벌 변수에 추가 구현 필요
		}
	}
}

void JobQueue::Execute()
{
	WriteLockGuard wLock(lock, "JobQueue::Execute");
	while (!_q.empty())
	{
		JobRef job = _q.front();
		_q.pop();
		_qSize.fetch_sub(1);
		job->Execute();
	}
}
