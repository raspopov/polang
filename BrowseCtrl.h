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

// CBrowseCtrl

class CBrowseCtrl : public CMFCEditBrowseCtrl
{
	DECLARE_DYNAMIC(CBrowseCtrl)

public:
	inline CBrowseCtrl() = default;

	void SetFolder(const CString& sFolder)
	{
		m_sFolder = sFolder;
		m_sFolder.TrimRight( _T( '\\' ) );
	}

protected:
	CString		m_sFolder;

	virtual void OnBrowse();

	afx_msg void OnDropFiles( HDROP hDropInfo );

	DECLARE_MESSAGE_MAP()
};
