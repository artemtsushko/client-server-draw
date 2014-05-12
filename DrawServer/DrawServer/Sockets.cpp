// C:\Users\Артем\Documents\Visual Studio 2013\Projects\Lab Works 4 smr\Lab02_Client-serverDraw\DrawServer\DrawServer\Sockets.cpp : implementation file
//

#include "stdafx.h"
#include "DrawServer.h"
#include "Sockets.h"


// CClientSocket

CClientSocket::CClientSocket(Pool* pool)
: pPool(pool), state(WAIT_FOR_TYPE), pointsLeft(0), figure(nullptr)
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnReceive(int nErrorCode)
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
				pPool->ProcessRead(this, figure);
				state = WAIT_FOR_TYPE;
			}
		}
		else
			AfxMessageBox(_T("Fatal error occurred"));
		break;
	}
	
	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	pPool->ProcessClose(this);
	CSocket::OnClose(nErrorCode);
}






// CListeningSocket

CListeningSocket::CListeningSocket(Pool* pool)
: pPool(pool)
{
}

CListeningSocket::~CListeningSocket()
{
}

void CListeningSocket::OnAccept(int nErrorCode)
{
	pPool->ProcessAccept();
	CSocket::OnAccept(nErrorCode);
}





// Pool

Pool::Pool() : m_pListeningSocket(nullptr)
{

}

Pool::~Pool()
{
	for (auto it = connectionList.begin(); it != connectionList.end(); ++it)
	{
		CClientSocket* pSocket = *it;
		pSocket->Close();
		delete pSocket;
	}
	
	m_pListeningSocket->Close();
	delete m_pListeningSocket;

	for (auto it = figures.begin(); it != figures.end(); ++it)
	{
		Figure* pFigure = *it;
		delete pFigure;
	}
}

BOOL Pool::Init(int port)
{
	m_pListeningSocket = new CListeningSocket(this);
	if (m_pListeningSocket->Create(port))
	{
		if (m_pListeningSocket->Listen())
			return TRUE;
	}
	return FALSE;
}

int Pool::ProcessAccept()
{
	CClientSocket* pSocket = new CClientSocket(this);
	if (m_pListeningSocket->Accept(*pSocket))
	{
		connectionList.push_back(pSocket);
		for (auto it = figures.begin(); it != figures.end(); ++it)
			Send2One(*(*it), pSocket);
	}
	else
		delete pSocket;
	return 0;
}

void Pool::ProcessClose(CClientSocket *pSocket)
{
	connectionList.remove(pSocket);
	delete pSocket;
}

void Pool::ProcessRead(CClientSocket *pSocket, Figure* msg)
{
	figures.push_back(msg);
	Send2All(*msg);
}

void Pool::Send2All(Figure figure)
{
	for (auto it = connectionList.begin(); it != connectionList.end(); ++it)
	{
		CClientSocket* pSock = *it;
		pSock->Send(&(figure.type), sizeof(FigureType));
		pSock->Send(&(figure.verticesNumber), sizeof(int));
		for (int i = 0; i < figure.verticesNumber; i++)
		{
			pSock->Send(&(figure.vertices[i]), sizeof(POINT));
		}
	}
}

void Pool::Send2One(Figure figure, CClientSocket* pSocket)
{
	pSocket->Send(&(figure.type), sizeof(FigureType));
	pSocket->Send(&(figure.verticesNumber), sizeof(int));
	for (int i = 0; i < figure.verticesNumber; i++)
	{
		pSocket->Send(&(figure.vertices[i]), sizeof(POINT));
	}
}
