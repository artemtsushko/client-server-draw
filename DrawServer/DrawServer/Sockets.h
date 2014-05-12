#pragma once
#include "Figure.h"
using namespace std;

class Pool;

// CClientSocket command target

class CClientSocket : public CSocket
{
public:
	CClientSocket(Pool* pool);
	virtual ~CClientSocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
protected:
	Pool* pPool;
	enum State {
		WAIT_FOR_TYPE,
		WAIT_FOR_SIZE,
		WAIT_FOR_POINT
	} state;
	FigureType type;
	int pointsLeft;
	Figure* figure;
};


// CListeningSocket command target

class CListeningSocket : public CSocket
{
public:
	CListeningSocket(Pool* pool);
	virtual ~CListeningSocket();
	virtual void OnAccept(int nErrorCode);
protected:
	Pool* pPool;
};


class Pool
{
public:
	Pool();
	virtual ~Pool();
	BOOL Init(int port);
	void Send2All(Figure data);
	void Send2One(Figure data, CClientSocket* pSocket);
	void ProcessClose(CClientSocket *pSocket);
	void ProcessRead(CClientSocket* pSocket, Figure* msg);
	int ProcessAccept();
protected:
	CListeningSocket *m_pListeningSocket;
	list<CClientSocket*> connectionList;
	list<Figure*> figures;
};