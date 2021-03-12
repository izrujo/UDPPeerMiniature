#ifndef _UDPPEERDIALOG_H
#define _UDPPEERDIALOG_H

#include "resource.h"
#include "ConnectSocket.h"
#include "ListenSocket.h"
#include <afxwin.h>

class UDPPeerDialog : public CDialog
{
// Construction
public:
	UDPPeerDialog(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_UDPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	SOCKET broadcastSocket;
	CWinThread* broadcaster;
	BOOL isBroadcasting;
	BOOL isBroadcastPausing;
	void Broadcast();
	void PauseBroadcasting();
	void ResumeBroadcasting();
	void EndBroadcasting();
	static UINT BroadcastThread(LPVOID pParam);
	BOOL GetIsBroadcasting() const;
	BOOL GetIsBroadcastPausing() const;
    afx_msg void OnBroadcastButtonClicked();

    ListenSocket listenSocket;
	CWinThread* listener;
	BOOL isListening;
	BOOL isListenPausing;
	void Listen();
	void PauseListening();
	void ResumeListening();
	void EndListening();
	static UINT ListenThread(LPVOID pParam);
	BOOL GetIsListening() const;
	BOOL GetIsListenPausing() const;

	SOCKET collectSocket;
	CWinThread* collector;
	BOOL isCollecting;
	BOOL isCollectPausing;
	void Collect();
	void PauseCollecting();
	void ResumeCollecting();
	void EndCollecting();
	static UINT CollectThread(LPVOID pParam);
	BOOL GetIsCollecting() const;
	BOOL GetIsCollectPausing() const;

    CString receiveData;
    int otherPortno;
	int myPortno;
    CEdit printEdit;
    afx_msg void OnCollectButtonClicked();

    ConnectSocket connectSocket;
	CString message;
	afx_msg void OnSendButtonClicked();
	CString ipAddress;
	afx_msg void OnConnectButtonClicked();

};

inline BOOL UDPPeerDialog::GetIsBroadcasting() const {
	return this->isBroadcasting;
}
inline BOOL UDPPeerDialog::GetIsBroadcastPausing() const {
	return this->isBroadcastPausing;
}

inline BOOL UDPPeerDialog::GetIsListening() const {
	return this->isListening;
}
inline BOOL UDPPeerDialog::GetIsListenPausing() const {
	return this->isListenPausing;
}

inline BOOL UDPPeerDialog::GetIsCollecting() const {
	return this->isCollecting;
}
inline BOOL UDPPeerDialog::GetIsCollectPausing() const {
	return this->isCollectPausing;
}

#endif //_UDPPEERDIALOG_H