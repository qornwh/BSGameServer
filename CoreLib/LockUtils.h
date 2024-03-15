#pragma once

#include <thread>

#include "Pch.h"

class Lock
{
public:
	void ReadLock();
	void ReadUnLock();
	void WriteLock();
	void WriteUnLock();

private:
	atomic<int> readCount;
	atomic<int> writeCount;
	const int MAX_SPIN_COUNT = 5000;
};

/* ------------------
	LockGuards
--------------------- */

class ReadLockGuard
{
public:
	ReadLockGuard(Lock &lock, const char *name) : _lock(lock), _name(name) { _lock.ReadLock(); }
	~ReadLockGuard() { _lock.ReadUnLock(); }

private:
	Lock &_lock;
	const char *_name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock &lock, const char *name) : _lock(lock), _name(name) { _lock.WriteLock(); }
	~WriteLockGuard() { _lock.WriteUnLock(); }

private:
	Lock &_lock;
	const char *_name;
};
