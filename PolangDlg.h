/*

Polang
Copyright (C) 2017 Nikolay Raspopov <raspopov@cherubicsoft.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include "BrowseCtrl.h"
#include "afxwin.h"


// CPolangDlg dialog

class CPolangDlg : public CDialogEx
{
public:
	CPolangDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POLANG_DIALOG };
#endif

protected:
	enum { OPT_NULL = -1, OPT_POT = 0, OPT_PO = 1, OPT_LANG = 2 };

	HICON				m_hIcon;
	CBrowseCtrl			m_wnd1File;
	CBrowseCtrl			m_wnd2File;
	CBrowseCtrl			m_wnd3File;
	CStatic				m_wnd1Title;
	CStatic				m_wnd2Title;
	CStatic				m_wnd3Title;
	CButton				m_wnd1Open;
	CButton				m_wnd2Open;
	CButton				m_wnd3Open;
	CButton				m_wnd12Set;
	CButton				m_wnd23Set;
	int					m_nOptions, m_nOptionsLast;
	CRect				m_rcInitial;
	CToolTipCtrl		m_pTips;
	BOOL				m_bPreserve;
	CButton				m_wndPreserve;

	void UpdateInterface(int nOptions);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage( MSG* pMsg );

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClicked1Open();
	afx_msg void OnBnClicked2Open();
	afx_msg void OnBnClicked3Open();
	afx_msg void OnBnClicked12Set();
	afx_msg void OnBnClicked23Set();
	afx_msg void OnBnClickedFormat();

	DECLARE_MESSAGE_MAP()
};
