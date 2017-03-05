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

#include "stdafx.h"
#include "Polang.h"
#include "PolangDlg.h"
#include "Translation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SETTINGS	_T("Settings")
#define OPTIONS		_T("Options")
#define ENGLISH		_T("EnglishPath")
#define LANG		_T("LangPath")
#define PO			_T("PoPath")
#define WINDOW		_T("Window")

// CPolangDlg dialog

CPolangDlg::CPolangDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx			( IDD_POLANG_DIALOG, pParent )
	, m_bOptions		( theApp.GetProfileInt( SETTINGS, OPTIONS, 0 ) )
	, m_sEnglishFilename( theApp.GetProfileString( SETTINGS, ENGLISH ) )
	, m_sLangFilename	( theApp.GetProfileString( SETTINGS, LANG ) )
	, m_sPoFilename		( theApp.GetProfileString( SETTINGS, PO ) )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CPolangDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_ENGLISH_FILE, m_wndEnglish );
	DDX_Control( pDX, IDC_LANG_FILE, m_wndLang );
	DDX_Control( pDX, IDC_PO_FILE, m_wndPo );

	DDX_Radio( pDX, IDC_RADIO1, m_bOptions );
	DDX_Text( pDX, IDC_ENGLISH_FILE, m_sEnglishFilename );
	DDX_Text( pDX, IDC_LANG_FILE, m_sLangFilename );
	DDX_Text( pDX, IDC_PO_FILE, m_sPoFilename );
}

void CPolangDlg::UpdateInterface()
{
	static const CString sInput = LoadString( IDS_INPUT );
	static const CString sOutput = LoadString( IDS_OUTPUT );
	static const CString sEnglish = LoadString( IDC_ENGLISH_TITLE );
	static const CString sLang = LoadString( IDC_LANG_TITLE );
	static const CString sPo = LoadString( IDC_PO_TITLE );
	GetDlgItem( IDC_ENGLISH_TITLE )->SetWindowText( sInput + sEnglish );
	GetDlgItem( IDC_LANG_TITLE )->SetWindowText( ( m_bOptions ? sOutput : sInput ) + sLang );
	GetDlgItem( IDC_PO_TITLE )->SetWindowText( ( m_bOptions ? sInput : sOutput ) + sPo );

	m_wndEnglish.EnableFileBrowseButton( _T("lang"), _T("Language Files (*.lang)|*.lang|All Files|*.*||"),
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );

	m_wndLang.EnableFileBrowseButton( _T("lang"), _T("Language Files (*.lang)|*.lang|All Files|*.*||"),
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | ( m_bOptions ? OFN_OVERWRITEPROMPT : OFN_FILEMUSTEXIST ) );

	m_wndPo.EnableFileBrowseButton( _T("po"), _T("PoEdit Files (*.po;*.pot)|*.po;*.pot|All Files|*.*||"),
		OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | ( m_bOptions ? OFN_FILEMUSTEXIST : OFN_OVERWRITEPROMPT) );

	GetDlgItem( IDC_ENGLISH_TITLE )->EnableWindow( ! m_bOptions );
	GetDlgItem( IDC_ENGLISH_FILE )->EnableWindow( ! m_bOptions );
}

BEGIN_MESSAGE_MAP(CPolangDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDC_RADIO1, &CPolangDlg::OnBnClickedRadio )
	ON_BN_CLICKED( IDC_RADIO2, &CPolangDlg::OnBnClickedRadio )
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CPolangDlg message handlers

BOOL CPolangDlg::OnInitDialog()
{
	__super::OnInitDialog();

	GetWindowRect( m_rcInitial );

	SetIcon( m_hIcon, TRUE );		// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	UpdateInterface();

	CAutoVectorPtr< WINDOWPLACEMENT> pl;
	UINT len = 0;
	theApp.GetProfileBinary( SETTINGS, WINDOW, (LPBYTE*)&pl, &len );
	if ( len == sizeof( WINDOWPLACEMENT ) )
	{
		SetWindowPlacement( pl );
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPolangDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		__super::OnPaint();
	}
}

void CPolangDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo( lpMMI );

	lpMMI->ptMinTrackSize.x = m_rcInitial.Width();
	lpMMI->ptMinTrackSize.y = m_rcInitial.Height();
}

HCURSOR CPolangDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPolangDlg::OnOK()
{
	CWaitCursor wc;

	if ( ! UpdateData() )
		return;

	CTranslation translations;
	if ( m_bOptions )
	{
		// X.po -> X.lang

		if ( m_sPoFilename.IsEmpty() || m_sLangFilename.IsEmpty() ||
			 GetFileAttributes( m_sPoFilename ) == INVALID_FILE_ATTRIBUTES )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadPo( m_sPoFilename ) )
		{
			AfxMessageBox( IDS_MSG_PO_LOAD_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.SaveLang( m_sLangFilename ) )
		{
			AfxMessageBox( IDS_MSG_LANG_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( AfxMessageBox( IDS_MSG_LANG_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), NULL, m_sLangFilename, NULL, NULL, SW_NORMAL );
		}
	}
	else
	{
		// en_US.lang + X.lang -> X.po

		if ( m_sEnglishFilename.IsEmpty() || m_sLangFilename.IsEmpty() || m_sPoFilename.IsEmpty() ||
			 GetFileAttributes( m_sEnglishFilename ) == INVALID_FILE_ATTRIBUTES )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadLang( m_sEnglishFilename ) )
		{
			AfxMessageBox( IDS_MSG_ENGLISH_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! m_sLangFilename.IsEmpty() )
		{
			if ( ! translations.LoadLang( m_sLangFilename, true ) )
			{
				AfxMessageBox( IDS_MSG_LANG_ERROR, MB_OK | MB_ICONEXCLAMATION );
				return;
			}
		}

		if ( ! translations.SavePo( m_sPoFilename ) )
		{
			AfxMessageBox( IDS_MSG_PO_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( AfxMessageBox( IDS_MSG_PO_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), NULL, m_sPoFilename, NULL, NULL, SW_NORMAL );
		}
	}
}

void CPolangDlg::OnBnClickedRadio()
{
	UpdateData();

	UpdateInterface();
}

void CPolangDlg::OnDestroy()
{
	UpdateData();

	WINDOWPLACEMENT pl = { sizeof( WINDOWPLACEMENT ) };
	GetWindowPlacement( &pl );
	theApp.WriteProfileBinary( SETTINGS, WINDOW, (LPBYTE)&pl, sizeof( WINDOWPLACEMENT ) );
	theApp.WriteProfileInt( SETTINGS, OPTIONS, m_bOptions );
	theApp.WriteProfileString( SETTINGS, ENGLISH, m_sEnglishFilename );
	theApp.WriteProfileString( SETTINGS, LANG, m_sLangFilename );
	theApp.WriteProfileString( SETTINGS, PO, m_sPoFilename );

	__super::OnDestroy();
}
