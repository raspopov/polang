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

#define SETTINGS		_T("Settings")
#define VAL_OPTIONS		_T("Options")
#define VAL_ENGLISH		_T("EnglishPath")
#define VAL_LANG		_T("LangPath")
#define VAL_PO			_T("PoPath")
#define VAL_POT			_T("PotPath")
#define VAL_WINDOW		_T("Window")

// CPolangDlg dialog

CPolangDlg::CPolangDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx			( IDD_POLANG_DIALOG, pParent )
	, m_hIcon			( theApp.LoadIcon( IDR_MAINFRAME ) )
	, m_nOptions		( theApp.GetProfileInt( SETTINGS, VAL_OPTIONS, OPT_POT ) )
	, m_nOptionsLast	( OPT_NULL )
{
}

void CPolangDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_1_FILE, m_wnd1File );
	DDX_Control( pDX, IDC_2_FILE, m_wnd2File );
	DDX_Control( pDX, IDC_3_FILE, m_wnd3File );

	DDX_Control( pDX, IDC_1_TITLE, m_wnd1Title );
	DDX_Control( pDX, IDC_2_TITLE, m_wnd2Title );
	DDX_Control( pDX, IDC_3_TITLE, m_wnd3Title );

	DDX_Control( pDX, IDC_1_OPEN, m_wnd1Open );
	DDX_Control( pDX, IDC_2_OPEN, m_wnd2Open );
	DDX_Control( pDX, IDC_3_OPEN, m_wnd3Open );

	DDX_Radio( pDX, IDC_RADIO1, m_nOptions );
}

void CPolangDlg::UpdateInterface(int nOptions)
{
	CString s1Filename, s2Filename, s3Filename;
	m_wnd1File.GetWindowText( s1Filename );
	m_wnd2File.GetWindowText( s2Filename );
	m_wnd3File.GetWindowText( s3Filename );

	// Save previous
	CString str;
	switch ( m_nOptionsLast )
	{
	case OPT_NULL:
		break;

	case OPT_POT:
		theApp.WriteProfileString( SETTINGS, VAL_ENGLISH, s1Filename );
		theApp.WriteProfileString( SETTINGS, VAL_POT, s2Filename );
		break;

	case OPT_PO:
		theApp.WriteProfileString( SETTINGS, VAL_ENGLISH, s1Filename );
		theApp.WriteProfileString( SETTINGS, VAL_LANG, s2Filename );
		theApp.WriteProfileString( SETTINGS, VAL_PO, s3Filename );
		break;

	case OPT_LANG:
		theApp.WriteProfileString( SETTINGS, VAL_PO, s1Filename );
		theApp.WriteProfileString( SETTINGS, VAL_LANG, s2Filename );
		break;
	}

	switch ( nOptions )
	{
	case OPT_NULL:
		break;

	case OPT_POT:
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_ENGLISH_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_ENGLISH );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T("en_US.lang") );
		// Output
		m_wnd2Title.SetWindowText( LoadString( IDS_POT_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("pot"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_POT );
		if ( s2Filename.IsEmpty() && ! s1Filename.IsEmpty() )
			s2Filename = s1Filename.Left( (int)( PathFindExtension( (LPCTSTR)s1Filename ) - (LPCTSTR)s1Filename ) + 1 ) + _T("pot");
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("en_US.pot") );
		// Disabled
		m_wnd3Title.SetWindowText( _T("") );
		m_wnd3File.EnableWindow( FALSE );
		m_wnd3File.SetWindowText( _T("") );
		m_wnd3File.SetCueBanner( _T("") );
		m_wnd3Open.EnableWindow( FALSE );
		break;

	case OPT_PO:
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_ENGLISH_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_ENGLISH );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T("en_US.lang") );
		// Input
		m_wnd2Title.SetWindowText( LoadString( IDS_LANG_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_LANG );
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("ru_RU.lang") );
		// Output
		m_wnd3Title.SetWindowText( LoadString( IDS_PO_TITLE ) );
		m_wnd3File.EnableWindow();
		m_wnd3File.EnableFileBrowseButton( _T("po"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		s3Filename = theApp.GetProfileString( SETTINGS, VAL_PO );
		if ( s3Filename.IsEmpty() && ! s2Filename.IsEmpty() )
			s3Filename = s2Filename.Left( (int)( PathFindExtension( (LPCTSTR)s2Filename ) - (LPCTSTR)s2Filename ) + 1 ) + _T("po");
		m_wnd3File.SetWindowText( s3Filename );
		m_wnd3File.SetCueBanner( _T("ru_RU.po") );
		m_wnd3Open.EnableWindow();
		break;

	case OPT_LANG:
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_PO_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T("po"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_PO );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T("ru_RU.po") );
		// Output
		m_wnd2Title.SetWindowText( LoadString( IDS_LANG_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_LANG );
		if ( s2Filename.IsEmpty() && ! s1Filename.IsEmpty() )
			s2Filename = s1Filename.Left( (int)( PathFindExtension( (LPCTSTR)s1Filename ) - (LPCTSTR)s1Filename ) + 1 ) + _T("lang");
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("ru_RU.lang") );
		// Disabled
		m_wnd3Title.SetWindowText( _T("") );
		m_wnd3File.EnableWindow( FALSE );
		m_wnd3File.SetWindowText( _T("") );
		m_wnd3File.SetCueBanner( _T("") );
		m_wnd3Open.EnableWindow( FALSE );
		break;
	}

	if ( nOptions != OPT_NULL )
	{
		theApp.WriteProfileInt( SETTINGS, VAL_OPTIONS, nOptions );
	}

	m_nOptionsLast = nOptions;	
}

BEGIN_MESSAGE_MAP(CPolangDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_GETMINMAXINFO()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED( IDC_RADIO1, &CPolangDlg::OnBnClickedRadio )
	ON_BN_CLICKED( IDC_RADIO2, &CPolangDlg::OnBnClickedRadio )
	ON_BN_CLICKED( IDC_RADIO3, &CPolangDlg::OnBnClickedRadio )
	ON_WM_DESTROY()
	ON_BN_CLICKED( IDC_1_OPEN, &CPolangDlg::OnBnClicked1Open )
	ON_BN_CLICKED( IDC_2_OPEN, &CPolangDlg::OnBnClicked2Open )
	ON_BN_CLICKED( IDC_3_OPEN, &CPolangDlg::OnBnClicked3Open )
END_MESSAGE_MAP()

// CPolangDlg message handlers

BOOL CPolangDlg::OnInitDialog()
{
	__super::OnInitDialog();

	GetWindowRect( m_rcInitial );

	SetIcon( m_hIcon, TRUE );		// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	UpdateInterface( m_nOptions );

	CAutoVectorPtr< WINDOWPLACEMENT> pl;
	UINT len = 0;
	theApp.GetProfileBinary( SETTINGS, VAL_WINDOW, (LPBYTE*)&pl, &len );
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
	if ( IsIconic() )
	{
		CPaintDC dc( this ); // device context for painting

		SendMessage( WM_ICONERASEBKGND, reinterpret_cast< WPARAM >( dc.GetSafeHdc() ), 0 );

		// Center icon in client rectangle
		CRect rect;
		GetClientRect(&rect);
		int x = ( rect.Width() - GetSystemMetrics( SM_CXICON ) + 1 ) / 2;
		int y = ( rect.Height() - GetSystemMetrics( SM_CYICON ) + 1 ) / 2;

		// Draw the icon
		dc.DrawIcon( x, y, m_hIcon );
	}
	else
		__super::OnPaint();
}

void CPolangDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	__super::OnGetMinMaxInfo( lpMMI );

	lpMMI->ptMinTrackSize.x = m_rcInitial.Width();
	lpMMI->ptMinTrackSize.y = m_rcInitial.Height();
}

HCURSOR CPolangDlg::OnQueryDragIcon()
{
	return static_cast< HCURSOR >( m_hIcon );
}

void CPolangDlg::OnOK()
{
	CWaitCursor wc;

	UpdateData();

	CString s1Filename, s2Filename, s3Filename;
	m_wnd1File.GetWindowText( s1Filename );
	m_wnd2File.GetWindowText( s2Filename );
	m_wnd3File.GetWindowText( s3Filename );

	CTranslation translations;
	switch ( m_nOptions )
	{
	case OPT_POT:
		// en_US.lang + en_US.pot
		if ( s1Filename.IsEmpty() || s2Filename.IsEmpty() || GetFileAttributes( s1Filename ) == INVALID_FILE_ATTRIBUTES )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadLang( s1Filename ) )
		{
			AfxMessageBox( IDS_MSG_ENGLISH_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.SavePo( s2Filename ) )
		{
			AfxMessageBox( IDS_MSG_PO_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( AfxMessageBox( IDS_MSG_PO_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), nullptr, s2Filename, nullptr, nullptr, SW_NORMAL );
		}
		break;

	case OPT_PO:
		// en_US.lang + Local.lang -> Local.po
		if ( s1Filename.IsEmpty() || s3Filename.IsEmpty() || GetFileAttributes( s1Filename ) == INVALID_FILE_ATTRIBUTES )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadLang( s1Filename ) )
		{
			AfxMessageBox( IDS_MSG_ENGLISH_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! s2Filename.IsEmpty() )
		{
			if ( ! translations.LoadLang( s2Filename, true ) )
			{
				AfxMessageBox( IDS_MSG_LANG_ERROR, MB_OK | MB_ICONEXCLAMATION );
				return;
			}
		}

		if ( ! translations.SavePo( s3Filename ) )
		{
			AfxMessageBox( IDS_MSG_PO_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( AfxMessageBox( IDS_MSG_PO_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), nullptr, s3Filename, nullptr, nullptr, SW_NORMAL );
		}
		break;

	case OPT_LANG:
		// Local.po -> Local.lang
		if ( s1Filename.IsEmpty() || s2Filename.IsEmpty() || GetFileAttributes( s1Filename ) == INVALID_FILE_ATTRIBUTES )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadPo( s1Filename ) )
		{
			AfxMessageBox( IDS_MSG_PO_LOAD_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.SaveLang( s2Filename ) )
		{
			AfxMessageBox( IDS_MSG_LANG_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( AfxMessageBox( IDS_MSG_LANG_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), nullptr, s2Filename, nullptr, nullptr, SW_NORMAL );
		}
		break;
	}
}

void CPolangDlg::OnBnClickedRadio()
{
	UpdateData();

	UpdateInterface( m_nOptions );
}

void CPolangDlg::OnDestroy()
{
	UpdateInterface( OPT_NULL );

	WINDOWPLACEMENT pl = { sizeof( WINDOWPLACEMENT ) };
	GetWindowPlacement( &pl );
	theApp.WriteProfileBinary( SETTINGS, VAL_WINDOW, (LPBYTE)&pl, sizeof( WINDOWPLACEMENT ) );

	__super::OnDestroy();
}

void CPolangDlg::OnBnClicked1Open()
{
	CWaitCursor wc;

	UpdateData();

	UpdateInterface( m_nOptions );

	CString sFilename;
	m_wnd1File.GetWindowText( sFilename );
	ShellExecute( GetSafeHwnd(), nullptr, sFilename, nullptr, nullptr, SW_NORMAL );
}

void CPolangDlg::OnBnClicked2Open()
{
	CWaitCursor wc;

	UpdateData();

	UpdateInterface( m_nOptions );

	CString sFilename;
	m_wnd2File.GetWindowText( sFilename );
	ShellExecute( GetSafeHwnd(), nullptr, sFilename, nullptr, nullptr, SW_NORMAL );
}

void CPolangDlg::OnBnClicked3Open()
{
	CWaitCursor wc;

	UpdateData();

	UpdateInterface( m_nOptions );

	CString sFilename;
	m_wnd3File.GetWindowText( sFilename );
	ShellExecute( GetSafeHwnd(), nullptr, sFilename, nullptr, nullptr, SW_NORMAL );
}
