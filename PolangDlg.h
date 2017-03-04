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

// CPolangDlg dialog

class CPolangDlg : public CDialogEx
{
public:
	CPolangDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_POLANG_DIALOG };
#endif

protected:
	HICON				m_hIcon;
	CMFCEditBrowseCtrl	m_wndEnglish;
	CMFCEditBrowseCtrl	m_wndLang;
	CMFCEditBrowseCtrl	m_wndPo;
	BOOL				m_bOptions;
	CRect				m_rcInitial;
	CString				m_sEnglishFilename;
	CString				m_sLangFilename;
	CString				m_sPoFilename;

	void UpdateInterface();

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo( MINMAXINFO* lpMMI );
	afx_msg void OnEnChangeLangFile();

	DECLARE_MESSAGE_MAP()
};
