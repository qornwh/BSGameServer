#pragma once

#include <memory>
#include <vector>
#include <array>
#include <iostream>

#include "Pch.h"
#include "LockUtils.h"

class SendBufferChunk;

// -------------
//	SendBuffer
// ---------------
class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE *buffer, uint32 allocSize);
	~SendBuffer();

	unsigned char *Buffer() { return _buffer; }
	unsigned int AllocSize() { return _allocSize; }
	unsigned int WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

private:
	BYTE *_buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	SendBufferChunkRef _owner;
};

// -------------------
//	SendBufferChunk
// -------------------

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 0xFFFF
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	SendBufferRef Open(uint32 allocSize);
	void Close(uint32 writeSize);

	bool IsOpen() { return _open; }
	BYTE *Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	Array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32 _usedSize = 0;
};

// ---------------------
//	SendBufferManager
// ---------------------

class SendBufferManager
{
public:
	SendBufferRef Open(uint32 size);

private:
	SendBufferChunkRef Pop();
	void Push(SendBufferChunkRef buffer);

private:
	static void PushGlobal(SendBufferChunk *buffer);

private:
	// USE_LOCK;
	Lock lock;
	vector<SendBufferChunkRef> _sendBufferChunks;
};
