#ifndef _UDPPEERAPP_H
#define _UDPPEERAPP_H

#include <afxwin.h>
#include "resource.h"		// main symbols

class UDPPeerApp : public CWinApp
{
public:
	UDPPeerApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern UDPPeerApp theApp;

#endif //_UDPPEERAPP_H