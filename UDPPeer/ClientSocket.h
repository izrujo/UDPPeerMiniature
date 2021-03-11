#ifndef _CLIENTSOCKET_H
#define _CLIENTSOCKET_H

#include <afxsock.h>

class ClientSocket : public CSocket { //������� ���������� �̷������ ����
public:
	ClientSocket();
	virtual ~ClientSocket();

	void SetListenSocket(CAsyncSocket* socket);
	//������ ����Ǵ� ������ ȣ��ȴ�.
	virtual void OnClose(int nErrorCode);
	//ä�� �޽����� �޾��� �� ȣ��ȴ�. �޽����� �����ϰ� ����� ��� Ŭ���̾�Ʈ���� �����Ѵ�.
	virtual void OnReceive(int nErrorCode);

public:
	CAsyncSocket* listenSocket;
};

#endif //_CLIENTSOCKET_H