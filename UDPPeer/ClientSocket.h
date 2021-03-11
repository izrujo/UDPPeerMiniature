#ifndef _CLIENTSOCKET_H
#define _CLIENTSOCKET_H

#include <afxsock.h>

class ClientSocket : public CSocket { //입출력이 동기적으로 이루어지는 소켓
public:
	ClientSocket();
	virtual ~ClientSocket();

	void SetListenSocket(CAsyncSocket* socket);
	//연결이 종료되는 시점에 호출된다.
	virtual void OnClose(int nErrorCode);
	//채팅 메시지를 받았을 때 호출된다. 메시지를 수신하고 연결된 모든 클라이언트에게 전송한다.
	virtual void OnReceive(int nErrorCode);

public:
	CAsyncSocket* listenSocket;
};

#endif //_CLIENTSOCKET_H