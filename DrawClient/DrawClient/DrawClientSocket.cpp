// DrawClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "DrawClient.h"
#include "DrawClientSocket.h"
#include "DrawClientDlg.h"


// CDrawClientSocket

CDrawClientSocket::CDrawClientSocket(CDrawClientDlg* pDlg)
: m_pDialog(pDlg), state(WAIT_FOR_TYPE), pointsLeft(0), figure(nullptr)
{
}

CDrawClientSocket::~CDrawClientSocket()
{
}


// CDrawClientSocket member functions


void CDrawClientSocket::OnReceive(int nErrorCode)
{
	int nRead;
	switch (state)
	{
	case WAIT_FOR_TYPE:
		nRead = Receive(&type, sizeof(FigureType));
		if (nRead == SOCKET_ERROR || !nRead) {
			AfxMessageBox(_T("Error occurred"));
			Close();
			return;
		}
		state = WAIT_FOR_SIZE;
		break;

	case WAIT_FOR_SIZE:
		nRead = Receive(&pointsLeft, sizeof(int));
		if (!nRead || nRead == SOCKET_ERROR) {
			AfxMessageBox(_T("Error occurred"));
			Close();
			return;
		}
		figure = new Figure(type, pointsLeft);
		state = WAIT_FOR_POINT;
		break;

	case WAIT_FOR_POINT:
		if (pointsLeft > 0)
		{
			nRead = Receive(&(figure->vertices[figure->verticesNumber - pointsLeft]), sizeof(POINT));
			if (!nRead || nRead == SOCKET_ERROR) {
				AfxMessageBox(_T("Error occurred"));
				Close();
				return;
			}
			--pointsLeft;
			if (!pointsLeft)
			{
				m_pDialog->addFigure(figure);
				state = WAIT_FOR_TYPE;
			}
		}
		else
			AfxMessageBox(_T("Fatal socket error occurred"));
		break;
	}

	CSocket::OnReceive(nErrorCode);
}


void CDrawClientSocket::OnClose(int nErrorCode)
{
	AfxMessageBox(_T("Connection broken"));
	CSocket::OnClose(nErrorCode);
}
