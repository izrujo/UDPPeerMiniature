#ifndef _CONNECTSOCKET_H
#define _CONNECTSOCKET_H

#include <afxsock.h>

class ConnectSocket : public CSocket {
public:
	ConnectSocket();
	virtual ~ConnectSocket();
	virtual void OnClose(int nErrorCode);
	//virtual void OnReceive(int nErrorCode);
};

#endif //_CONNECTSOCKET_H