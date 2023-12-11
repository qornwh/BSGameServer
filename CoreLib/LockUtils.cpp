#include "LockUtils.h"

void Lock::ReadLock()
{
	while (true)
	{
		int cur = readCount.load();
		for (int i = 0; i < MAX_SPIN_COUNT; i++)
		{
			if (readCount.compare_exchange_strong(cur, cur + 1))
			{
				return;
			}
		}

		// To-do crash 처리

		this_thread::yield();
	}
}

void Lock::ReadUnLock()
{
	readCount.fetch_sub(1);
}

void Lock::WriteLock()
{
	while (true)
	{
		for (int i = 0; i < MAX_SPIN_COUNT; i++)
		{
			int readCur = readCount.load();
			int writeCur = writeCount.load();
			// 밑에 cas 2개쓴거 한번에 안됨 문젱있음, read write 한변수에 담고 cas 처리해야됨
			if (readCount.compare_exchange_strong(readCur, 0) && writeCount.compare_exchange_strong(writeCur, writeCur + 1))
			{
				return;
			}
		}

		// To-do crash 처리

		this_thread::yield();
	}
}

void Lock::WriteUnLock()
{
	writeCount.fetch_sub(1);
}
