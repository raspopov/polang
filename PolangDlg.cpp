// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

	DDX_Control( pDX, IDC_1_2_SET, m_wnd12Set );
	DDX_Control( pDX, IDC_2_3_SET, m_wnd23Set );

	DDX_Radio( pDX, IDC_RADIO1, m_nOptions );
}

void CPolangDlg::UpdateInterface(int nOptions)
{
	CString s1Filename, s2Filename, s3Filename;
	m_wnd1File.GetWindowText( s1Filename );
	m_wnd2File.GetWindowText( s2Filename );
	m_wnd3File.GetWindowText( s3Filename );

	// Save previous
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
		theApp.WriteProfileString( SETTINGS, VAL_ENGLISH, s1Filename );
		theApp.WriteProfileString( SETTINGS, VAL_PO, s2Filename );
		theApp.WriteProfileString( SETTINGS, VAL_LANG, s3Filename );
		break;
	}

	switch ( nOptions )
	{
	case OPT_NULL:
		break;

	case OPT_POT:
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_ENGLISH );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_POT );
		if ( s2Filename.IsEmpty() && IsFileName( s1Filename ) )
			s2Filename = TrimExtension( s1Filename ) + _T( ".pot" );
		if ( s1Filename.IsEmpty() && IsFileName( s2Filename ) )
			s1Filename = TrimExtension( s2Filename ) + _T( ".lang" );
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_ENGLISH_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T("en_US.lang") );
		// Output
		m_wnd2Title.SetWindowText( LoadString( IDS_POT_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("pot"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("en_US.pot") );
		m_wnd12Set.EnableWindow();
		// Disabled
		m_wnd3Title.SetWindowText( _T("") );
		m_wnd3File.EnableWindow( FALSE );
		m_wnd3File.SetWindowText( _T("") );
		m_wnd3File.SetCueBanner( _T("") );
		m_wnd3Open.EnableWindow( FALSE );
		m_wnd23Set.EnableWindow( FALSE );
		break;

	case OPT_PO:
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_ENGLISH );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_LANG );
		s3Filename = theApp.GetProfileString( SETTINGS, VAL_PO );
		if ( s3Filename.IsEmpty() && IsFileName( s2Filename ) )
			s3Filename = TrimExtension( s2Filename ) + _T( ".po" );
		if ( s2Filename.IsEmpty() && IsFileName( s3Filename ) )
			s2Filename = TrimExtension( s3Filename ) + _T( ".lang" );
		if ( s1Filename.IsEmpty() && IsFileName( s2Filename ) )
			s1Filename = TrimFileName( s2Filename ) + _T( "en_US.lang" );
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_ENGLISH_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T("en_US.lang") );
		// Input
		m_wnd2Title.SetWindowText( LoadString( IDS_LANG_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("lang"), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("ru_RU.lang") );
		m_wnd12Set.EnableWindow( FALSE );
		// Output
		m_wnd3Title.SetWindowText( LoadString( IDS_PO_TITLE ) );
		m_wnd3File.EnableWindow();
		m_wnd3File.EnableFileBrowseButton( _T("po"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		m_wnd3File.SetWindowText( s3Filename );
		m_wnd3File.SetCueBanner( _T("ru_RU.po") );
		m_wnd3Open.EnableWindow();
		m_wnd23Set.EnableWindow();
		break;

	case OPT_LANG:
		s1Filename = theApp.GetProfileString( SETTINGS, VAL_ENGLISH );
		s2Filename = theApp.GetProfileString( SETTINGS, VAL_PO );
		s3Filename = theApp.GetProfileString( SETTINGS, VAL_LANG );
		if ( s3Filename.IsEmpty() && IsFileName( s2Filename ) )
			s3Filename = TrimExtension( s2Filename ) + _T( ".lang" );
		if ( s2Filename.IsEmpty() && IsFileName( s3Filename ) )
			s2Filename = TrimExtension( s3Filename ) + _T( ".po" );
		if ( s1Filename.IsEmpty() && IsFileName( s2Filename ) )
			s1Filename = TrimFileName( s2Filename ) + _T( "en_US.lang" );
		// Input
		m_wnd1Title.SetWindowText( LoadString( IDS_ENGLISH_TITLE ) );
		m_wnd1File.EnableFileBrowseButton( _T( "lang" ), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		m_wnd1File.SetWindowText( s1Filename );
		m_wnd1File.SetCueBanner( _T( "en_US.lang" ) );
		// Input
		m_wnd2Title.SetWindowText( LoadString( IDS_PO_TITLE ) );
		m_wnd2File.EnableFileBrowseButton( _T("po"), LoadString( IDS_POEDIT_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST );
		m_wnd2File.SetWindowText( s2Filename );
		m_wnd2File.SetCueBanner( _T("ru_RU.po") );
		m_wnd12Set.EnableWindow( FALSE );
		// Output
		m_wnd3Title.SetWindowText( LoadString( IDS_LANG_TITLE ) );
		m_wnd3File.EnableWindow();
		m_wnd3File.EnableFileBrowseButton( _T( "lang" ), LoadString( IDS_LANG_FILES ), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT );
		m_wnd3File.SetWindowText( s3Filename );
		m_wnd3File.SetCueBanner( _T( "ru_RU.lang" ) );
		m_wnd3Open.EnableWindow();
		m_wnd23Set.EnableWindow();
		break;
	}

	if ( nOptions != OPT_NULL )
	{
		theApp.WriteProfileInt( SETTINGS, VAL_OPTIONS, nOptions );
	}

	m_nOptionsLast = nOptions;

	if ( ! s1Filename.IsEmpty() )
	{
		m_wnd1File.SetFolder( TrimFileName( s1Filename ) );
		m_wnd2File.SetFolder( TrimFileName( s1Filename ) );
		m_wnd3File.SetFolder( TrimFileName( s1Filename ) );
	}
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
	ON_BN_CLICKED( IDC_1_2_SET, &CPolangDlg::OnBnClicked12Set )
	ON_BN_CLICKED( IDC_2_3_SET, &CPolangDlg::OnBnClicked23Set )
END_MESSAGE_MAP()

// CPolangDlg message handlers

BOOL CPolangDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetWindowText( theApp.m_pszAppName );

	GetWindowRect( m_rcInitial );

	SetIcon( m_hIcon, TRUE );		// Set big icon
	SetIcon( m_hIcon, FALSE );		// Set small icon

	m_pTips.Create( this, TTS_ALWAYSTIP );
	m_pTips.Activate( TRUE );
	m_pTips.SetMaxTipWidth( 300 );

	static CWnd* wnds[] = { &m_wnd1Open, &m_wnd2Open, &m_wnd3Open, &m_wnd12Set, &m_wnd23Set };
	for ( int i = 0; i < _countof( wnds ); ++i )
	{
		m_pTips.AddTool( wnds[ i ], LoadString( wnds[ i ]->GetDlgCtrlID() ) );
	}

	m_wnd1Open.SetIcon( ( HICON )LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_RUN ), IMAGE_ICON, 16, 16, LR_SHARED ) );
	m_wnd2Open.SetIcon( ( HICON )LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_RUN ), IMAGE_ICON, 16, 16, LR_SHARED ) );
	m_wnd3Open.SetIcon( ( HICON )LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_RUN ), IMAGE_ICON, 16, 16, LR_SHARED ) );
	m_wnd12Set.SetIcon( ( HICON )LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_DOWN ), IMAGE_ICON, 16, 16, LR_SHARED ) );
	m_wnd23Set.SetIcon( ( HICON )LoadImage( AfxGetResourceHandle(), MAKEINTRESOURCE( IDI_DOWN ), IMAGE_ICON, 16, 16, LR_SHARED ) );

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

BOOL CPolangDlg::PreTranslateMessage( MSG* pMsg )
{
	m_pTips.RelayEvent( pMsg );

	return __super::PreTranslateMessage( pMsg );
}

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
		if ( s2Filename.IsEmpty() || ! IsFileName( s1Filename ) )
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
		if ( s3Filename.IsEmpty() || ! IsFileName( s1Filename ) )
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
		if ( s3Filename.IsEmpty() || ! IsFileName( s2Filename ) )
		{
			AfxMessageBox( IDS_MSG_NO_FILE, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! translations.LoadPo( s2Filename ) )
		{
			AfxMessageBox( IDS_MSG_PO_LOAD_ERROR, MB_OK | MB_ICONEXCLAMATION );
			return;
		}

		if ( ! IsFileName( s1Filename ) )
		{
			// Save in sorted order
			if ( ! translations.SaveLang( s3Filename ) )
			{
				AfxMessageBox( IDS_MSG_LANG_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
				return;
			}
		}
		else
		{
			// Ssve preserving order
			if ( !translations.LoadLang( s1Filename, false, s3Filename ) )
			{
				AfxMessageBox( IDS_MSG_LANG_SAVE_ERROR, MB_OK | MB_ICONEXCLAMATION );
				return;
			}
		}

		if ( AfxMessageBox( IDS_MSG_LANG_SAVE_OK, MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			ShellExecute( GetSafeHwnd(), nullptr, s3Filename, nullptr, nullptr, SW_NORMAL );
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

void CPolangDlg::OnBnClicked12Set()
{
	CWaitCursor wc;

	UpdateData();

	CString s1Filename;
	m_wnd1File.GetWindowText( s1Filename );

	if ( ! s1Filename.IsEmpty() )
		m_wnd2File.SetWindowText( _T( "" ) );

	UpdateInterface( m_nOptions );
}

void CPolangDlg::OnBnClicked23Set()
{
	CWaitCursor wc;

	UpdateData();

	CString s2Filename;
	m_wnd2File.GetWindowText( s2Filename );

	if ( ! s2Filename.IsEmpty() )
		m_wnd3File.SetWindowText( _T("") );

	UpdateInterface( m_nOptions );
}
