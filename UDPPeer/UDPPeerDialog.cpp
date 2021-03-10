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
    , m_recvData(_T(""))
    , m_portno(63396)
    , m_message(_T(""))
{
	this->m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void UDPPeerDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, this->m_recvData);
    DDX_Control(pDX, IDC_EDIT1, this->m_editCtrl);
    DDX_Text(pDX, IDC_EDIT3, this->m_portno);
}

BEGIN_MESSAGE_MAP(UDPPeerDialog, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDOK, &UDPPeerDialog::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &UDPPeerDialog::OnSendButtonClicked)
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


void UDPPeerDialog::OnBnClickedOk()
{
    StartListening();
    SetTimer(0x1, 100, NULL);
    GetDlgItem(IDOK)->EnableWindow(FALSE);
    GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
    //OnOK();
}

void UDPPeerDialog::OnSendButtonClicked()
{
    WORD w = MAKEWORD(1, 1);
    WSADATA wsadata;
    ::WSAStartup(w, &wsadata);

    this->m_listenSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->m_listenSocket == -1)
    {
        AfxMessageBox("Error in creating socket");
    }
    char opt = 1;
    setsockopt(this->m_listenSocket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(char));
    SOCKADDR_IN brdcastaddr;
    memset(&brdcastaddr, 0, sizeof(brdcastaddr));
    brdcastaddr.sin_family = AF_INET;
    brdcastaddr.sin_port = htons(this->m_portno);
    brdcastaddr.sin_addr.s_addr = INADDR_BROADCAST;
    int len = sizeof(brdcastaddr);

    ((CEdit*)GetDlgItem(IDC_EDIT_MESSAGE))->GetWindowText(this->m_message);

    char sbuf[1024];
    sprintf(sbuf, "%s\r\n", this->m_message);
    int ret = sendto(this->m_listenSocket, sbuf, strlen(sbuf), 0, (sockaddr*)&brdcastaddr, len);
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
        AfxMessageBox("Broadcasting is done");
    }
    ::closesocket(this->m_listenSocket);
}

void UDPPeerDialog::StartListening()
{
    UpdateData(TRUE);

    this->m_connectSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (this->m_connectSocket == -1)
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
    UDPserveraddr.sin_port = htons(this->m_portno);
    UDPserveraddr.sin_addr.s_addr = INADDR_ANY;

    int len = sizeof(UDPserveraddr);

    if (bind(this->m_connectSocket, (SOCKADDR*)&UDPserveraddr, sizeof(SOCKADDR_IN)) < 0)
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
    FD_SET(this->m_connectSocket, &fds);

    int rc = select(sizeof(fds) * 8, &fds, NULL, NULL, &timeout);
    if (rc > 0)
    {
        char rbuf[1024];
        SOCKADDR_IN clientaddr;
        int len = sizeof(clientaddr);
        if (recvfrom(this->m_connectSocket, rbuf, 1024, 0, (sockaddr*)&clientaddr, &len) > 0)
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

            rData.Format("%s\r\nBroadcast Server: %s \r\n%s\r\n\r\n", (const char*)CTime::GetCurrentTime().Format("%B %d, %Y %H:%M:%S"), p, rbuf);

            this->m_recvData = rData + this->m_recvData;
            this->m_message = this->m_recvData;
            UpdateData(FALSE);
        }
    }
    CDialog::OnTimer(nIDEvent);
}