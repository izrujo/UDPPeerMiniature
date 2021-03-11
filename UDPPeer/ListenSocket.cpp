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

void ListenSocket::CloseClientSocket(CSocket* client) {
	if (client != NULL) {
		client->ShutDown(); //��� ����
		client->Close(); //���� �ݱ�
		delete client; //����
	}

}

void ListenSocket::SendChatDataAll(TCHAR* message) {
	if (this->clientSocket != NULL) {
		this->clientSocket->Send(message, lstrlen(message)); //�����ڵ带 ����ϹǷ� ���ڿ� ���̿� *2
		//��ȯ���� *2�� ���� ���ƾ� �Ѵ�.
	}
}