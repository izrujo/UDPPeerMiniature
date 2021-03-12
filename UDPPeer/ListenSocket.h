#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

#include "ClientSocket.h"
#include <afxsock.h>

class ListenSocket : public CAsyncSocket { //입출력이 비동기적으로 이루어지는 소켓
public:
	ListenSocket();
	virtual ~ListenSocket();

	//OnAccept - 서버가 접속을 기다리는 특정 포트로 클라이언트의 TCP 접속이 있을 때 자동으로 호출된다.
	virtual void OnAccept(int nErrorCode);

	void CloseClientSocket();
	
	void SendChatDataAll(TCHAR* message);

public:
	ClientSocket* clientSocket;
};

#endif //_LISTENSOCKET_H