#include "LockUtils.h"

Lock::Lock()
{
	pthread_rwlock_init(&rwlock, nullptr);
}

void Lock::ReadLock()
{
	while (true)
	{
		for (int i = 0; i < MAX_SPIN_COUNT; i++)
		{
			if (pthread_rwlock_tryrdlock(&rwlock) == 0)
			{
				pthread_rwlock_rdlock(&rwlock);
				return;
			}
		}
		this_thread::yield();
	}
}

void Lock::ReadUnLock()
{
	pthread_rwlock_unlock(&rwlock);
}

void Lock::WriteLock()
{
	while (true)
	{
		for (int i = 0; i < MAX_SPIN_COUNT; i++)
		{
			// pthread_rwlock_tryrdlock 하지 않은 이유는 2개 다하면 EBUSY라는 모드가 되서 pthread_rwlock_trywrlock만한다. pthread_rwlock_trywrlock만 해도 read 걸린다
			if (pthread_rwlock_trywrlock(&rwlock) == 0)
			{
				pthread_rwlock_wrlock(&rwlock);
				return;
			}
		}
		this_thread::yield();
	}
}

void Lock::WriteUnLock()
{
	pthread_rwlock_unlock(&rwlock);
}
