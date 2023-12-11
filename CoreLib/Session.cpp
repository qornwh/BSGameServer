#include "Session.h"
#include "Types.h"

Session::Session(int socketFd) : _socketFd(socketFd), _recvBuffer(BUFFER_SIZE)
{
	Connect();
}

Session::~Session()
{
	cout << "Session Disconnected !!!" << endl;
	close(_socketFd);
};

bool Session::Send(SendBufferRef buffer)
{
	if (!IsConnected())
		return false;

	{
		WriteLockGuard wLock(lock, "writeLock");
		//_sendBuffers = queue<SendBufferRef>();
		_sendBuffers.push_back(buffer);
	}
	int size = _sendBuffers.size();
	struct iovec iov[size];
	{
		WriteLockGuard wLock(lock, "writeLock");
		int i = 0;
		for (auto sendBuffer : _sendBuffers)
		{
			iov[i].iov_base = sendBuffer->Buffer();
			iov[i].iov_len = sendBuffer->WriteSize();
			i++;
		}
	}

	struct msghdr message;
	memset(&message, 0, sizeof(message));
	message.msg_iov = iov;
	message.msg_iovlen = size;

	// MSG_NOSIGNAL 옵션 'SO_NOSIGPIPE' 가 없으므로 broken pipe해결
	int32 result = sendmsg(_socketFd, &message, MSG_NOSIGNAL); // sendflage 확인필요

	WriteLockGuard wLock(lock, "writeLock");
	_sendBuffers.clear();
	if (result <= 0)
	{
		return false;
	}
	return true;
}

void Session::Connect()
{
	_connected.store(true);
}

bool Session::ReciveMessage()
{
	int strLen = recv(_socketFd, _recvBuffer.WritePos(), _recvBuffer.FreeSize(), 0);

	if (strLen <= 0)
	{
		Disconnect();
		return false;
	}
	else
	{
		if (!_recvBuffer.OnWrite(strLen))
		{
			Disconnect();
			return false;
		}

		int dataSize = _recvBuffer.DataSize();

		BYTE* buffer = _recvBuffer.ReadPos();

		//cout << strLen << endl;
		int32 processLen = OnRecv(buffer, dataSize);
		//cout << "session OnRecv" << endl;
		if (!_recvBuffer.OnRead(processLen) || processLen <= 0 || dataSize < processLen)
		{
			Disconnect();
			return false;
		}

		_recvBuffer.Clean();
	}
	return true;
}

void Session::Disconnect()
{
	_recvBuffer.Clean();
	_sendBuffers.clear(); // queue clear 함수 없어서 이렇게함
	_connected.exchange(false);
}

PacketSession::PacketSession(int socketFd) : Session(socketFd)
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	return len;
}
