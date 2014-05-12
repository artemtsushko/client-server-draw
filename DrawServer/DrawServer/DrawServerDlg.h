#pragma once
#include "Sockets.h"

// CDrawServerDlg dialog
class CDrawServerDlg : public CDialogEx
{
// Construction
public:
	CDrawServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DRAWSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	int port;
	Pool* m_pPool;
public:
	afx_msg void OnBnClickedListen();
};
