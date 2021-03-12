#include "ListenSocket.h"
#include "ClientSocket.h"

ListenSocket::ListenSocket() {
	this->clientSocket = NULL;
}

ListenSocket::~ListenSocket() {

}

void ListenSocket::OnAccept(int nErrorCode) {
	this->clientSocket = new ClientSocket;
	if (Accept(*this->clientSocket)) {
		this->clientSocket->SetListenSocket(this);
		AfxMessageBox(_T("Other Peer Connected!!"));
	}
	else {
		delete this->clientSocket;
		AfxMessageBox(_T("Error: Failed to accept new Client!"));
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void ListenSocket::CloseClientSocket() {
	if (this->clientSocket != NULL) {
		this->clientSocket->ShutDown(); //통신 중지
		this->clientSocket->Close(); //소켓 닫기
		delete this->clientSocket; //삭제
		this->clientSocket = NULL;
	}
}

void ListenSocket::SendChatDataAll(TCHAR* message) {
	if (this->clientSocket != NULL) {
		this->clientSocket->Send(message, lstrlen(message)); //유니코드를 사용하므로 문자열 길이에 *2
		//반환값은 *2한 값과 같아야 한다.
	}
}