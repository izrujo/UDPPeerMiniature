#ifndef _LISTENSOCKET_H
#define _LISTENSOCKET_H

#include "ClientSocket.h"
#include <afxsock.h>

class ListenSocket : public CAsyncSocket { //������� �񵿱������� �̷������ ����
public:
	ListenSocket();
	virtual ~ListenSocket();

	//OnAccept - ������ ������ ��ٸ��� Ư�� ��Ʈ�� Ŭ���̾�Ʈ�� TCP ������ ���� �� �ڵ����� ȣ��ȴ�.
	virtual void OnAccept(int nErrorCode);

	void CloseClientSocket();
	
	void SendChatDataAll(TCHAR* message);

public:
	ClientSocket* clientSocket;
};

#endif //_LISTENSOCKET_H