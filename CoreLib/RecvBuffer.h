#pragma once

#include <cstring>
#include <vector>

#include "Pch.h"

/* -------------
	RecvBuffer
--------------- */

// [][][][][][][][][]
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 }; // 10배 이상 크게

public:
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int numOfBytes);
	bool OnWrite(int numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int	DataSize() { return _writePos - _readPos; }
	int	FreeSize() { return _capacity - _writePos; }

private:
	int	_capacity = 0;
	int	_bufferSize = 0;
	int	_readPos = 0;
	int	_writePos = 0;
	std::vector<unsigned char> _buffer;
};

