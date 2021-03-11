#include "ClientSocket.h"
#include "ListenSocket.h"
#include "UDPPeerDialog.h"

#pragma warning(disable:4996)
ClientSocket::ClientSocket(){
	this->listenSocket = NULL;
}

ClientSocket::~ClientSocket(){

}

void ClientSocket::SetListenSocket(CAsyncSocket* socket){
	this->listenSocket = socket;
}

void ClientSocket::OnClose(int nErrorCode){
	CSocket::OnClose(nErrorCode);

	ListenSocket* serverSocket = (ListenSocket*)this->listenSocket;
	serverSocket->CloseClientSocket(this);
}

void ClientSocket::OnReceive(int nErrorCode){
	CString strTemp = _T("");
	CString strIPAddress = _T("");
	UINT uPortNumber = 0;

	TCHAR buffer[1024];
	::ZeroMemory(buffer, sizeof(buffer));

	GetPeerName(strIPAddress, uPortNumber);
	if (Receive(buffer, sizeof(buffer)) > 0) {
		UDPPeerDialog* main = (UDPPeerDialog*)AfxGetMainWnd();

		strTemp.Format(_T("[%s:%d] : %s"), strIPAddress, uPortNumber, buffer);
		main->receiveData = strTemp + main->receiveData;
		//main->UpdateData(FALSE);

		/*ListenSocket* serverSocket = (ListenSocket*)this->listenSocket;
		serverSocket->SendChatDataAll(buffer);*/
	}

	CSocket::OnReceive(nErrorCode);
}