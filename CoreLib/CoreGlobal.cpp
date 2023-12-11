#include "Pch.h"
#include "CoreGlobal.h"
#include "SendBuffer.h"

SendBufferManager* GSendBufferManger = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GSendBufferManger = new SendBufferManager();
	}
	~CoreGlobal()
	{
		delete GSendBufferManger;
	}
} GCoreGlobal;