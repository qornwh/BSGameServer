#pragma once
#include <thread>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <atomic>
#include <functional>

using namespace std;

class ThreadManager
{
private:
	ThreadManager() {}
	ThreadManager(const ThreadManager& ref) {}
	ThreadManager& operator=(const ThreadManager& ref) {}
	~ThreadManager() {}

public:
	static ThreadManager& GetInstance()
	{
		static ThreadManager* instance = new ThreadManager();
		return *instance;
	}

public:
	void CreateThread(function<void()> callback)
	{
		threadId.fetch_add(1);

		threadList.push_back(thread([=]() {
			ThreadTLS();
			callback();
			ThreadDestory();
		}));
	}
	void ThreadTLS();
	void ThreadDestory();
	void ThreadJoinAll();

private:
	vector<thread> threadList;
	atomic<int> threadId;
};

