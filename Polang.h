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

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPolangApp

class CPolangApp : public CWinAppEx
{
public:
	CPolangApp();

protected:
	virtual BOOL InitInstance() override;

	DECLARE_MESSAGE_MAP()
};

extern CPolangApp theApp;

// Load string from resources
inline CString LoadString(UINT nId)
{
	CString str;
	VERIFY( str.LoadString( nId ) );
	return str;
}

// Cut off file extension (i.e. extract path + filename only)
inline CString TrimExtension(LPCTSTR szFilename)
{
	if ( ! szFilename || ! *szFilename )
		return CString();
	return CString( szFilename, (int)( PathFindExtension( szFilename ) - szFilename ) );
}

// Cut off file name from path (i.e. extract path only)
inline CString TrimFileName(LPCTSTR szFilename)
{
	if ( ! szFilename || ! *szFilename )
		return CString();
	return CString( szFilename, (int)( PathFindFileName( szFilename ) - szFilename ) );
}

// Check if this is a real file on disk
inline bool IsFileName(LPCTSTR szFilename)
{
	if ( ! szFilename || ! *szFilename )
		return false;
	const DWORD attr = GetFileAttributes( szFilename );
	return ( ( attr != INVALID_FILE_ATTRIBUTES ) && ( ( attr & FILE_ATTRIBUTE_DIRECTORY ) == 0 ) );
}
