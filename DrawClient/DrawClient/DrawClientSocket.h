#pragma once
#include "Figure.h"
class CDrawClientDlg;

// CDrawClientSocket command target

class CDrawClientSocket : public CSocket
{
public:
	CDrawClientSocket(CDrawClientDlg* pDlg);
	virtual ~CDrawClientSocket();
protected:
	CDrawClientDlg* m_pDialog;
	enum State {
		WAIT_FOR_TYPE,
		WAIT_FOR_SIZE,
		WAIT_FOR_POINT
	} state;
	FigureType type;
	int pointsLeft;
	Figure* figure;
public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


