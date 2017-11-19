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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define Classes		_T("SOFTWARE\\Classes\\")
#define Verb		_T("\\Shell")
#define Command		_T("\\Command")

// CPolangApp

BEGIN_MESSAGE_MAP(CPolangApp, CWinAppEx)
END_MESSAGE_MAP()

// CPolangApp construction

CPolangApp::CPolangApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

// The one and only CPolangApp object

CPolangApp theApp;

// CPolangApp initialization

BOOL CPolangApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls = { sizeof( InitCtrls ), ICC_WIN95_CLASSES };
	InitCommonControlsEx( &InitCtrls );

	__super::InitInstance();

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager( RUNTIME_CLASS( CMFCVisualManagerWindows ) );

	SetRegistryKey( AFX_IDS_COMPANY_NAME );

	int nRegister = 0;
	int nArgs = 0;
	if ( LPWSTR* szArglist = CommandLineToArgvW( GetCommandLineW(), &nArgs ) )
	{
		for ( int i = 1; i < nArgs; ++i )
		{
			if ( _tcsicmp( szArglist[ i ] + 1, _T("RegServer") ) == 0 )
			{
				nRegister = 1;
			}
			else if ( _tcsicmp( szArglist[ i ] + 1, _T( "UnRegServer" ) ) == 0 )
			{
				nRegister = -1;
			}
		}
		LocalFree( szArglist );
	}

	CString sPath;
	GetModuleFileName( NULL, sPath.GetBuffer( MAX_PATH ), MAX_PATH );
	sPath.ReleaseBuffer();
	sPath = CString( _T("\"") ) + sPath + _T("\" \"%1\"");

	const CString sApp = AfxGetAppName();
	const CString sName = LoadString( IDS_VERB_NAME );
	const CString sType = LoadString( IDS_TYPE );

	static const LPCTSTR szExts[] = { _T(".po"), _T(".pot"), _T(".lang") };
	for ( int i = 0; i < 3; ++i )
	{
		for ( auto & szExt : szExts )
		{
			const CString sDefaultProgId = sApp + szExt;

			CString sProgId;
			DWORD nSize = 64;
			DWORD nType = 0;
			LSTATUS res = SHGetValue( HKEY_CLASSES_ROOT, szExt, NULL, &nType, sProgId.GetBuffer( nSize + 1 ), &nSize );
			sProgId.ReleaseBuffer();
			if ( res == ERROR_SUCCESS && nSize && nType == REG_SZ )
			{
				// OK
			}
			else
			{
				// Own extension key
				sProgId = sDefaultProgId;

				if ( nRegister >= 0 )
				{
					res = SHRegSetUSValue( CString( Classes ) + szExt, NULL, REG_SZ,
						sProgId, sProgId.GetLength() * sizeof( TCHAR ), SHREGSET_HKCU );
					res = SHRegSetUSValue( CString( Classes ) + szExt, NULL, REG_SZ,
						sProgId, sProgId.GetLength() * sizeof( TCHAR ), SHREGSET_HKLM );

					res = SHRegSetUSValue( CString( Classes ) + sProgId, NULL, REG_SZ,
						sType, sType.GetLength() * sizeof( TCHAR ), SHREGSET_HKCU );
					res = SHRegSetUSValue( CString( Classes ) + sProgId, NULL, REG_SZ,
						sType, sType.GetLength() * sizeof( TCHAR ), SHREGSET_HKLM );
				}
			}

			if ( nRegister >= 0 )
			{
				res = SHRegSetUSValue( CString( Classes ) + sProgId + Verb + _T("\\") + sApp, NULL, REG_SZ,
					sName, sName.GetLength() * sizeof( TCHAR ), SHREGSET_HKCU );
				res = SHRegSetUSValue( CString( Classes ) + sProgId + Verb + _T( "\\" ) + sApp, NULL, REG_SZ,
					sName, sName.GetLength() * sizeof( TCHAR ), SHREGSET_HKLM );

				res = SHRegSetUSValue( CString( Classes ) + sProgId + Verb + _T("\\") + sApp + Command, NULL, REG_SZ,
					sPath, sPath.GetLength() * sizeof( TCHAR ), SHREGSET_HKCU );
				res = SHRegSetUSValue( CString( Classes ) + sProgId + Verb + _T( "\\" ) + sApp + Command, NULL, REG_SZ,
					sPath, sPath.GetLength() * sizeof( TCHAR ), SHREGSET_HKLM );
			}
			else
			{
				SHDeleteValue( HKEY_CLASSES_ROOT, sProgId + Verb + _T("\\") + sApp + Command, NULL );
				SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId + Verb + _T("\\") + sApp + Command );

				SHDeleteValue( HKEY_CLASSES_ROOT, sProgId + Verb + _T("\\") + sApp, NULL );
				SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId + Verb + _T("\\") + sApp );

				SHDeleteValue( HKEY_CLASSES_ROOT, sProgId + Verb, NULL );
				SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId + Verb );

				// Own extension key?
				if ( sProgId == sDefaultProgId )
				{
					SHDeleteValue( HKEY_CLASSES_ROOT, sProgId, NULL );

					SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId );

					// Check for ProgID existence
					res = SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId );
					if ( res == ERROR_FILE_NOT_FOUND )
					{
						SHDeleteValue( HKEY_CLASSES_ROOT, szExt, NULL );
						SHDeleteEmptyKey( HKEY_CLASSES_ROOT, szExt );
					}
				}
				else
				{
					SHDeleteEmptyKey( HKEY_CLASSES_ROOT, sProgId );
				}
			}
		}
	}
	if ( nRegister != 0 )
		// Registration only
		return FALSE;

	CPolangDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}
