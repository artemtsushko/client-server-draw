
// DrawClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DrawClient.h"
#include "DrawClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDrawClientDlg dialog



CDrawClientDlg::CDrawClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDrawClientDlg::IDD, pParent)
	, m_pSocket(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDrawClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDrawClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_POLYLINE, &CDrawClientDlg::OnEditPolyline)
	ON_COMMAND(ID_EDIT_POLYGON, &CDrawClientDlg::OnEditPolygon)
	ON_COMMAND(ID_EDIT_ELLIPSOID, &CDrawClientDlg::OnEditEllipsoid)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_UNDO, &CDrawClientDlg::OnUndo)
	ON_COMMAND(ID_SEND, &CDrawClientDlg::OnSend)
	ON_COMMAND(ID_EDIT_RECTANGLE, &CDrawClientDlg::OnEditRectangle)
	ON_COMMAND(ID_CONNECTION_CONNECTSERVER, &CDrawClientDlg::OnConnect)
	ON_COMMAND(ID_CONNECTION_DISCONNECT, &CDrawClientDlg::OnDisconnect)
END_MESSAGE_MAP()


// CDrawClientDlg message handlers

BOOL CDrawClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// menu initialization
	editMenu.LoadMenu(IDR_EDIT);
	submenu = editMenu.GetSubMenu(0);
	ASSERT(submenu);
	submenu->CheckMenuRadioItem(ID_EDIT_POLYLINE, ID_EDIT_ELLIPSOID, ID_EDIT_POLYLINE, MF_BYCOMMAND);

	// socket creation
	m_pSocket = new CDrawClientSocket(this);
	m_pSocket->Create();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDrawClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDrawClientDlg::OnPaint()
{
	CPaintDC dc(this); // device context for paintiting
	CRect rect;
	GetClientRect(rect);
	FillRect(dc.m_hDC, rect, NULL);
	drawFigures(dc);
	drawCurrentFigure(dc);
	CDialogEx::OnPaint();
}

void CDrawClientDlg::drawFigures(HDC hDC)
{
	for (auto it = m_Figures.begin(); it != m_Figures.end(); ++it)
		(*it)->draw(hDC);
}

void CDrawClientDlg::drawCurrentFigure(HDC hDC)
{	
	// draw ellipsoid
	UINT state = submenu->GetMenuState(ID_EDIT_ELLIPSOID, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
	{
		if (m_currFigureCoord.size() > 2)
		{
			MessageBox(_T("Select exactly 2 points!"));
			m_currFigureCoord.clear();
			RedrawWindow();
		}
		else
		{
			CRect rect;
			GetClientRect(&rect);

			if (m_currFigureCoord.size() > 0)
			{
				MoveToEx(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, NULL);
				LineTo(hDC, m_currFigureCoord[0].x, rect.Height());
				MoveToEx(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, NULL);
				LineTo(hDC, rect.Width(), m_currFigureCoord[0].y);
			}
			if (m_currFigureCoord.size() > 1)
			{
				MoveToEx(hDC, m_currFigureCoord[1].x, m_currFigureCoord[1].y, NULL);
				LineTo(hDC, m_currFigureCoord[1].x, 0);
				MoveToEx(hDC, m_currFigureCoord[1].x, m_currFigureCoord[1].y, NULL);
				LineTo(hDC, 0, m_currFigureCoord[1].y);
			}
		}
	}
	else
	{
		// draw rectangle
		state = submenu->GetMenuState(ID_EDIT_RECTANGLE, MF_BYCOMMAND);
		ASSERT(state != 0xFFFFFFFF);
		if (state & MF_CHECKED)
		{
			if (m_currFigureCoord.size() > 2)
			{
				MessageBox(_T("Select exactly 2 points!"));
				m_currFigureCoord.clear();
				RedrawWindow();
			}
			else
			{
				CRect rect;
				GetClientRect(&rect);

				if (m_currFigureCoord.size() > 0)
				{
					MoveToEx(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, NULL);
					LineTo(hDC, m_currFigureCoord[0].x, rect.Height());
					MoveToEx(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, NULL);
					LineTo(hDC, rect.Width(), m_currFigureCoord[0].y);
				}
				if (m_currFigureCoord.size() > 1)
				{
					MoveToEx(hDC, m_currFigureCoord[1].x, m_currFigureCoord[1].y, NULL);
					LineTo(hDC, m_currFigureCoord[1].x, 0);
					MoveToEx(hDC, m_currFigureCoord[1].x, m_currFigureCoord[1].y, NULL);
					LineTo(hDC, 0, m_currFigureCoord[1].y);
				}
			}
		}
		else
		// Draw polygon/polyline
		{
			if (m_currFigureCoord.size() > 0)
			{
				MoveToEx(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, NULL);
				if (m_currFigureCoord.size() == 1)
					SetPixel(hDC, m_currFigureCoord[0].x, m_currFigureCoord[0].y, RGB(0, 0, 0));
				else
				for (int i = 1; i < m_currFigureCoord.size(); i++)
					LineTo(hDC, m_currFigureCoord[i].x, m_currFigureCoord[i].y);
			}
		}
	}
}

bool CDrawClientDlg::controlPointsNumber()
{
	UINT state = submenu->GetMenuState(ID_EDIT_POLYLINE, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
	if (m_currFigureCoord.size() < 2)
	{
		MessageBox(_T("Select at least 2 points!"));
		return false;
	}

	state = submenu->GetMenuState(ID_EDIT_POLYGON, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
	if (m_currFigureCoord.size() < 3)
	{
		MessageBox(_T("Select at least 3 points!"));
		return false;
	}

	state = submenu->GetMenuState(ID_EDIT_RECTANGLE, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
	if (m_currFigureCoord.size() != 2)
	{
		MessageBox(_T("Select exactly 2 points!"));
		if (m_currFigureCoord.size() > 2)
			m_currFigureCoord.clear();
		RedrawWindow();
		return false;
	}

	state = submenu->GetMenuState(ID_EDIT_ELLIPSOID, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
	if (m_currFigureCoord.size() != 2)
	{
		MessageBox(_T("Select exactly 2 points!"));
		if (m_currFigureCoord.size() > 2)
			m_currFigureCoord.clear();
		RedrawWindow();
		return false;
	}
	return true;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDrawClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDrawClientDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	submenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}


void CDrawClientDlg::OnEditPolyline()
{
	// TODO: Add your command handler code here
	submenu->CheckMenuRadioItem(ID_EDIT_POLYLINE, ID_EDIT_RECTANGLE, ID_EDIT_POLYLINE, MF_BYCOMMAND);
	m_currFigureCoord.clear();
	RedrawWindow();
}


void CDrawClientDlg::OnEditPolygon()
{
	// TODO: Add your command handler code here
	submenu->CheckMenuRadioItem(ID_EDIT_POLYLINE, ID_EDIT_RECTANGLE, ID_EDIT_POLYGON, MF_BYCOMMAND);
	m_currFigureCoord.clear();
	RedrawWindow();
}

void CDrawClientDlg::OnEditRectangle()
{
	// TODO: Add your command handler code here
	submenu->CheckMenuRadioItem(ID_EDIT_POLYLINE, ID_EDIT_RECTANGLE, ID_EDIT_RECTANGLE, MF_BYCOMMAND);
	m_currFigureCoord.clear();
	RedrawWindow();
}

void CDrawClientDlg::OnEditEllipsoid()
{
	// TODO: Add your command handler code here
	submenu->CheckMenuRadioItem(ID_EDIT_POLYLINE, ID_EDIT_RECTANGLE, ID_EDIT_ELLIPSOID, MF_BYCOMMAND);
	m_currFigureCoord.clear();
	RedrawWindow();
}


void CDrawClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	POINT p = point;
	m_currFigureCoord.push_back(p);
	CDialogEx::OnLButtonDown(nFlags, point);
	RedrawWindow();
}


void CDrawClientDlg::OnUndo()
{
	if (m_currFigureCoord.size())
		m_currFigureCoord.pop_back();
	RedrawWindow();
}


void CDrawClientDlg::OnSend()
{
	if (controlPointsNumber())
	{
		FigureType type = getCurrentType();
		m_pSocket->Send(&type, sizeof(FigureType));

		int size = m_currFigureCoord.size();
		m_pSocket->Send(&size, sizeof(int));

		for (int i = 0; i < size; i++)
		{
			m_pSocket->Send(&(m_currFigureCoord[i]), sizeof(POINT));
		}

		m_currFigureCoord.clear();
		RedrawWindow();
	}
}

void CDrawClientDlg::addFigure(Figure* pFigure)
{
	m_Figures.push_back(pFigure);
	RedrawWindow();
}

FigureType CDrawClientDlg::getCurrentType()
{
	FigureType type;

	UINT state = submenu->GetMenuState(ID_EDIT_POLYLINE, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
		type = POLYLINE;

	state = submenu->GetMenuState(ID_EDIT_POLYGON, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
		type = POLYGON;

	state = submenu->GetMenuState(ID_EDIT_RECTANGLE, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
		type = RECTANGLE;

	state = submenu->GetMenuState(ID_EDIT_ELLIPSOID, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);
	if (state & MF_CHECKED)
		type = ELLIPSE;

	return type;
}


void CDrawClientDlg::OnConnect()
{
	CConnectionDlg dlg;
	INT_PTR nRet = dlg.DoModal();
	if (nRet == IDOK)
	if (!m_pSocket->Connect(dlg.ip, dlg.port))
	{
		AfxMessageBox(_T("Connection error!"));
	}
}


void CDrawClientDlg::OnDisconnect()
{
	m_pSocket->Close();
}



// CConnectionDlg dialog

IMPLEMENT_DYNAMIC(CConnectionDlg, CDialogEx)

CConnectionDlg::CConnectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CConnectionDlg::IDD, pParent)
	, ip(_T("127.0.0.1"))
	, port(1001)
{

}

CConnectionDlg::~CConnectionDlg()
{
}

void CConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IP, ip);
	DDX_Text(pDX, IDC_EDIT2, port);
}


BEGIN_MESSAGE_MAP(CConnectionDlg, CDialogEx)
END_MESSAGE_MAP()


// CConnectionDlg message handlers
