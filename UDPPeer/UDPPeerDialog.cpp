#include "UDPPeerApp.h"
#include "UDPPeerDialog.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDialog : public CDialog
{
public:
	CAboutDialog();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDialog::CAboutDialog() : CDialog(CAboutDialog::IDD)
{
}

void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
END_MESSAGE_MAP()

UDPPeerDialog::UDPPeerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD, pParent)
	, receiveData(_T(""))
	, portno(63527)
	, message(_T(""))
	, ipAddress(_T(""))
{
	this->broadcaster = NULL;
	this->isBroadcasting = FALSE;
	this->isBroadcastPausing = FALSE;

	this->listener = NULL;
	this->isListening = FALSE;
	this->isListenPausing = FALSE;

	this->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void UDPPeerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PRINT, this->receiveData);
	DDX_Control(pDX, IDC_EDIT_PRINT, this->printEdit);
	DDX_Text(pDX, IDC_STATIC_PORTNUMBER, this->portno);
}

BEGIN_MESSAGE_MAP(UDPPeerDialog, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROADCAST, &UDPPeerDialog::OnBroadcastButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_COLLECT, &UDPPeerDialog::OnCollectButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &UDPPeerDialog::OnSendButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &UDPPeerDialog::OnConnectButtonClicked)
END_MESSAGE_MAP()


// CUDPPeerDialog message handlers

BOOL UDPPeerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(this->m_hIcon, TRUE);			// Set big icon
	SetIcon(this->m_hIcon, FALSE);		// Set small icon

	//1.1. Send 버튼을 비활성화하다.
	GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);

	//1.2. 청자를 만들다. -서브 스레드 생성
	//1.3. 청자에서 듣다. -서브 스레드 실행, 접속 받으면 메시지 박스 출력
	//listenSocket에서 listen. ChatServerExample 참고
	this->Listen();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void UDPPeerDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDialog dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void UDPPeerDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, this->m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR UDPPeerDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(this->m_hIcon);
}

void UDPPeerDialog::OnClose() {
	//5.1. 스레드를 모두 종료하다.
	this->EndBroadcasting();

	this->EndListening();

	this->listenSocket.clientSocket->ShutDown();
	this->listenSocket.clientSocket->Close();
	this->listenSocket.ShutDown();
	this->listenSocket.Close(); //서버 종료.

	//5.2. 윈도우를 닫다.
	CDialog::OnClose();
}

void UDPPeerDialog::OnBroadcastButtonClicked()
{
	//2.1. 송신자를 만들다 -서브스레드 생성
	//2.2. 송신자에서 송신하다. -서브스레드 실행
	if (this->isBroadcasting == FALSE && this->isBroadcastPausing == FALSE) {
		this->Broadcast();
		GetDlgItem(IDC_BUTTON_BROADCAST)->SetWindowTextA("Pause B");
	}
	else if (this->isBroadcasting == TRUE && this->isBroadcastPausing == FALSE) {
		this->PauseBroadcasting();
		GetDlgItem(IDC_BUTTON_BROADCAST)->SetWindowTextA("Broadcast");
		AfxMessageBox("Pause Broadcasting");
	}
	else if (this->isBroadcasting == FALSE && this->isBroadcastPausing == TRUE) {
		this->ResumeBroadcasting();
		GetDlgItem(IDC_BUTTON_BROADCAST)->SetWindowTextA("Pause B");
		AfxMessageBox("Resume Broadcasting");
	}
}

void UDPPeerDialog::OnCollectButtonClicked() {
	StartCollecting();
	SetTimer(0x1, 100, NULL);
	GetDlgItem(IDC_BUTTON_COLLECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(FALSE);
}

void UDPPeerDialog::OnSendButtonClicked()
{
	//4.1. 메시지를 가져오다.
	((CEdit*)GetDlgItem(IDC_EDIT_MESSAGE))->GetWindowTextA(this->message);

	//4.2. 메시지를 보내다. connectSocket에 send 하기 ChatClientExample 참고
	this->connectSocket.Send((LPVOID)(LPCTSTR)this->message,
		this->message.GetLength());

	((CEdit*)GetDlgItem(IDC_EDIT_MESSAGE))->SetWindowTextA("");
}

void UDPPeerDialog::OnConnectButtonClicked() {
	//수신한 IP정보를 바탕으로 상대방 Peer에 접속을 요청하다.
			//connectSocket connect ChatClientExample 참고
	this->connectSocket.Create();
	if (this->connectSocket.Connect(this->ipAddress, this->portno) == FALSE)
	{
		AfxMessageBox(_T("ERROR: Failed to connect server"));
		PostQuitMessage(0);
	}
	else {
		AfxMessageBox(_T("Connect Success\r\nReady to Send Message"));
		//Send 버튼을 활성화하다.
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(TRUE);
	}
}

void UDPPeerDialog::StartCollecting()
{
	UpdateData(TRUE);

	this->collectSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (this->collectSocket == -1)
	{
		MessageBox("Error in creating socket");
		return;
	}

	//struct hostent *hostentry = gethostbyname(m_serveraddr);
	//char *pipaddr = inet_ntoa (*(struct in_addr *)*hostentry->h_addr_list);
	//UDPserveraddr.sin_addr.s_addr = inet_addr(pipaddr);

	SOCKADDR_IN UDPserveraddr;
	memset(&UDPserveraddr, 0, sizeof(UDPserveraddr));
	UDPserveraddr.sin_family = AF_INET;
	UDPserveraddr.sin_port = htons(this->portno);
	UDPserveraddr.sin_addr.s_addr = INADDR_ANY;

	int len = sizeof(UDPserveraddr);

	if (bind(this->collectSocket, (SOCKADDR*)&UDPserveraddr, sizeof(SOCKADDR_IN)) < 0)
	{
		MessageBox("ERROR binding in the server socket");
		return;
	}
}

void UDPPeerDialog::OnTimer(UINT_PTR nIDEvent)
{
	fd_set fds;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100;

	FD_ZERO(&fds);
	FD_SET(this->collectSocket, &fds);

	int rc = select(sizeof(fds) * 8, &fds, NULL, NULL, &timeout);
	if (rc > 0)
	{
		char rbuf[1024];
		SOCKADDR_IN clientaddr;
		int len = sizeof(clientaddr);
		if (recvfrom(this->collectSocket, rbuf, 1024, 0, (sockaddr*)&clientaddr, &len) > 0)
		{
			for (int i = 1024; i >= 1; i--)
			{
				if (rbuf[i] == '\n' && rbuf[i - 1] == '\r')
				{
					rbuf[i - 1] = '\0';
					break;
				}
			}
			char* p = inet_ntoa(clientaddr.sin_addr);
			int serverportno = ntohs(clientaddr.sin_port);
			CString rData;

			rData.Format("%s\r\nBroadcast Server- %s: %d \r\n%s\r\n\r\n", (const char*)CTime::GetCurrentTime().Format("%B %d, %Y %H:%M:%S"), p, serverportno, rbuf);

			this->receiveData = rData + this->receiveData;
			this->ipAddress = p;
			this->portno = atoi(rbuf);

			UpdateData(FALSE);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void UDPPeerDialog::Broadcast() {
	this->isBroadcasting = TRUE;

	this->broadcaster = AfxBeginThread(BroadcastThread, (LPVOID)this);
}

void UDPPeerDialog::PauseBroadcasting() {
	this->isBroadcasting = FALSE;
	this->isBroadcastPausing = TRUE;
	this->broadcaster->SuspendThread();
}

void UDPPeerDialog::ResumeBroadcasting() {
	this->isBroadcasting = TRUE;
	this->isBroadcastPausing = FALSE;
	this->broadcaster->ResumeThread();
}

void UDPPeerDialog::EndBroadcasting() {
	this->isBroadcasting = FALSE;
	if (this->broadcaster != NULL) {
		if (this->isBroadcastPausing == TRUE) {
			this->broadcaster->ResumeThread();
		}
		::WaitForSingleObject(this->broadcaster->m_hThread, INFINITE);

	}
	this->isBroadcastPausing = FALSE;
}

UINT UDPPeerDialog::BroadcastThread(LPVOID pParam) {
	UDPPeerDialog* dlg = (UDPPeerDialog*)pParam;

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@자기 포트번호 넣는 곳@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int myPortNumber = 59765;
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@자기 포트번호 넣는 곳@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int count = 0;
	while (dlg->isBroadcasting == TRUE) {
		WORD w = MAKEWORD(1, 1);
		WSADATA wsadata;
		::WSAStartup(w, &wsadata);

		dlg->broadcastSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (dlg->broadcastSocket == -1)
		{
			AfxMessageBox("Error in creating socket");
		}
		char opt = 1;
		setsockopt(dlg->broadcastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(char));
		SOCKADDR_IN brdcastaddr;
		memset(&brdcastaddr, 0, sizeof(brdcastaddr));
		brdcastaddr.sin_family = AF_INET;
		brdcastaddr.sin_port = htons(myPortNumber);
		brdcastaddr.sin_addr.s_addr = INADDR_BROADCAST;
		int len = sizeof(brdcastaddr);

		char sbuf[1024];
		sprintf(sbuf, "%d", myPortNumber);
		int ret = sendto(dlg->broadcastSocket, sbuf, strlen(sbuf), 0, (sockaddr*)&brdcastaddr, len);
		if (count <= 0) {
			if (ret < 0)
			{
				AfxMessageBox("Error broadcasting to the clients");
			}
			else if (ret < strlen(sbuf))
			{
				AfxMessageBox("Not all data broadcasted to the clients");
			}
			else
			{
				CString msg;
				msg.Format("Broadcasting is done by port %d", myPortNumber);
				AfxMessageBox(msg);
			}
		}
		::closesocket(dlg->broadcastSocket);
		count++;
	}

	return 0;
}

void UDPPeerDialog::Listen() {
	this->isListening = TRUE;

	this->listener = AfxBeginThread(ListenThread, (LPVOID)this);
}

void UDPPeerDialog::PauseListening() {
	this->isListening = FALSE;
	this->isListenPausing = TRUE;
	this->listener->SuspendThread();
}

void UDPPeerDialog::ResumeListening() {
	this->isListening = TRUE;
	this->isListenPausing = FALSE;
	this->listener->ResumeThread();
}

void UDPPeerDialog::EndListening() {
	this->isListening = FALSE;
	if (this->listener != NULL) {
		if (this->isListenPausing == TRUE) {
			this->listener->ResumeThread();
		}
		::WaitForSingleObject(this->listener->m_hThread, INFINITE);

	}
	this->isListenPausing = FALSE;
}

UINT UDPPeerDialog::ListenThread(LPVOID pParam) {
	UDPPeerDialog* dlg = (UDPPeerDialog*)pParam;

	if (dlg->listenSocket.Create(dlg->portno, SOCK_STREAM)) //소켓 생성(바인드되는 포트번호, TCP 소켓 플래그)
	{
		if (!dlg->listenSocket.Listen()) //서버가 클라이언트의 접속을 받을 수 있는 상태로 설정
		{
			AfxMessageBox(_T("ERROR: Listen() return FALSE"));
		}
		else {
			AfxMessageBox(_T("I am Listening"));
		}
	}
	else
	{
		DWORD error = GetLastError();
		AfxMessageBox(_T("ERROR: Failed to create server socket!"));
	}


	return 0;
}