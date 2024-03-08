#include "JobQueue.h"
#include "Job.h"

void JobQueue::Push(JobRef job)
{
	const int32 jobCount = _qSize.fetch_add(1);

	{
		WriteLockGuard wLock(lock, "JobQueue::Push");
		_q.push(job);
	}
}

void JobQueue::Execute()
{
	int32 jobCount = 0;
	queue<JobRef> jobs;
	{
		WriteLockGuard wLock(lock, "JobQueue::Execute");
		jobCount = static_cast<int32>(_q.size());
		for (int32 i = 0; i < jobCount; i++)
		{
			jobs.push(_q.front());
			_q.pop();
		}
	}

	while (!jobs.empty())
	{
		JobRef job = jobs.front();
		job->Execute();
		jobs.pop();
	}
	_qSize.fetch_sub(jobCount);
}
