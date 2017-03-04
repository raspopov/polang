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


class CTrans
{
public:
	inline CTrans() {}

	inline CTrans(const CString& sId)
	{
		m_sId.AddTail( sId );
	}

	inline CTrans(const CTrans& it)
	{
		for ( POSITION pos = it.m_sId.GetHeadPosition(); pos; )
		{
			m_sId.AddTail( it.m_sId.GetNext( pos ) );
		}
		m_sMsgstr = it.m_sMsgstr;
	}

	inline CTrans& operator=(const CTrans& it)
	{
		m_sId.RemoveAll();
		for ( POSITION pos = it.m_sId.GetHeadPosition(); pos; )
		{
			m_sId.AddTail( it.m_sId.GetNext( pos ) );
		}
		m_sMsgstr = it.m_sMsgstr;
		return *this;
	}

	inline CString GetId() const
	{
		CString sId;
		for ( POSITION pos = m_sId.GetHeadPosition(); pos; )
		{
			sId += _T(" ") + m_sId.GetNext( pos );
		}
		return sId;
	}

	CStringList	m_sId;
	CString		m_sMsgstr;
};


class CTranslation
{
public:
	inline CTranslation() {}

	inline POSITION GetHeadPosition() const
	{
		return m_MsgidToTrans.GetHeadPosition();
	}

	inline void GetNext(POSITION& pos, CTrans& trans, CString& sMsgid) const
	{
		m_MsgidToTrans.GetNextAssoc( pos, sMsgid, trans );
	}

	// Add English
	void SetAt(const CString& sIds, const CString& sMsgid);

	// Add localization
	void Add(const CString& sIds, const CString& sMsgstr);

	bool LoadPo(const CString& sFilename);
	bool LoadLang(const CString& sFilename, bool bMsgstr = false);
	bool SavePo(const CString& sFilename) const;
	bool SaveLang(const CString& sFilename) const;

private:
	CRBMap < CString, CTrans >	m_MsgidToTrans;
	CRBMap < CString, CString >	m_IdToMsgid;

	bool LoadPoFromString(const CString& sContent);

	static CString& Decode(CString& str);
};
