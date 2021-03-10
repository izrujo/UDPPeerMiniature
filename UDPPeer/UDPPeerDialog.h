#ifndef _UDPPEERDIALOG_H
#define _UDPPEERDIALOG_H

#include "resource.h"
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
    void StartListening();

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    SOCKET m_connectSocket;
    SOCKET m_listenSocket;
    CString m_recvData;
    CEdit m_editCtrl;
    afx_msg void OnBnClickedOk();
	afx_msg void OnSendButtonClicked();
    int m_portno;
	CString m_message;
};

#endif //_UDPPEERDIALOG_H