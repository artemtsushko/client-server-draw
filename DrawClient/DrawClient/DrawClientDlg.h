#pragma once
using namespace std;
#include "Figure.h"
#include "DrawClientSocket.h"

// CDrawClientDlg dialog
class CDrawClientDlg : public CDialogEx
{
// Construction
public:
	CDrawClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DRAWCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CMenu editMenu;
	CMenu* submenu;

	vector<POINT> m_currFigureCoord;
	vector<Figure*> m_Figures;
	CDrawClientSocket* m_pSocket;

	void drawFigures(HDC hDC);
	void drawCurrentFigure(HDC hDC);
	bool controlPointsNumber();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnEditPolyline();
	afx_msg void OnEditPolygon();
	afx_msg void OnEditEllipsoid();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUndo();
	afx_msg void OnSend();
	afx_msg void OnEditRectangle();
public:
	void addFigure(Figure* pFigure);
protected:
	FigureType getCurrentType();
public:
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
};
#pragma once


// CConnectionDlg dialog

class CConnectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConnectionDlg)

public:
	CConnectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectionDlg();

// Dialog Data
	enum { IDD = IDD_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString ip;
	int port;
};
