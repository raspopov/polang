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
#include "BrowseCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBrowseCtrl

IMPLEMENT_DYNAMIC(CBrowseCtrl, CMFCEditBrowseCtrl)

BEGIN_MESSAGE_MAP(CBrowseCtrl, CMFCEditBrowseCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void CBrowseCtrl::OnBrowse()
{
	if ( ! m_sFolder.IsEmpty() )
	{
		SetCurrentDirectory( m_sFolder );
	}

	CMFCEditBrowseCtrl::OnBrowse();
}

// CBrowseCtrl message handlers

void CBrowseCtrl::OnDropFiles( HDROP hDropInfo )
{
	TCHAR szItem[ 1024 ] = {};
	if ( DragQueryFile( hDropInfo, 0, szItem, _countof( szItem ) ) && ( GetFileAttributes( szItem ) & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
	{
		SetWindowText( szItem );
	}

	DragFinish( hDropInfo );
}
