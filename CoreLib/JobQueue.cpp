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
			// �ٷ� ť�� ������ Execute
			Execute();
		}
		else
		{
			// �۷ι� ť�� ��� ���߿� ����
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
