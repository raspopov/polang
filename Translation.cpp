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
#include "Translation.h"

void CTranslation::SetAt(const CStringList& lIds, const CString& sMsgid)
{
	for ( POSITION pos = lIds.GetHeadPosition(); pos; )
	{
		SetAt( lIds.GetNext( pos ), sMsgid );
	}
}

void CTranslation::SetAt(const CString& sId, const CString& sMsgid)
{
	CTrans trans;
	if ( m_MsgidToTrans.Lookup( sMsgid, trans ) )
	{
		// Add new ID to existing Msgid
		if ( ! trans.m_sId.Find( sId ) )
		{
			trans.m_sId.AddTail( sId );
			m_MsgidToTrans.SetAt( sMsgid, trans );
		}
		else
			TRACE( "Duplicate id: %s\n", (LPCSTR)CT2A( sId ) );
	}
	else
	{
		// Add new MsgId
		m_MsgidToTrans.SetAt( sMsgid, CTrans( sId ) );
	}
	m_IdToMsgid.SetAt( sId, sMsgid );
}

void CTranslation::Add(const CString& sId, const CString& sMsgstr)
{
	CString sMsgid;
	if ( m_IdToMsgid.Lookup( sId, sMsgid ) )
	{
		CTrans trans;
		if ( m_MsgidToTrans.Lookup( sMsgid, trans ) )
		{
			trans.m_sMsgstr = sMsgstr;
			m_MsgidToTrans.SetAt( sMsgid, trans );
		}
	}
}

void CTranslation::Add(const CStringList& lIds, const CString& sMsgstr)
{
	for ( POSITION pos = lIds.GetHeadPosition(); pos; )
	{
		Add( lIds.GetNext( pos ), sMsgstr );
	}
}

CString CTranslation::Escape(CString str)
{
	str.Replace( _T("\""), _T("\\\"") );
	str.Replace( _T("\t"), _T("\\t") );
	str.Replace( _T("\n"), _T("\\n") );
	str.Remove( _T('\r') );
	return str;
}

CString CTranslation::Decode(CString str)
{
	CString tmp;
	LPTSTR dst = tmp.GetBuffer( str.GetLength() + 1 );
	for ( LPCTSTR src = str; *src; src++ )
	{
		if ( *src == _T('\\') )
		{
			switch ( *(src + 1) )
			{
			case _T('\\'):
				*dst++ = _T('\\');
				src++;
				break;

			case _T('r'):
				*dst++ = _T('\r');
				src++;
				break;

			case _T('n'):
				*dst++ = _T('\n');
				src++;
				break;

			case _T('t'):
				*dst++ = _T('\t');
				src++;
				break;

			case _T('\"'):
				*dst++ = _T('\"');
				src++;
				break;

			default:
				*dst++ = *src;
			}
		}
		else
			*dst++ = *src;
	}
	*dst = 0;
	tmp.ReleaseBuffer();
	return tmp;
}

bool CTranslation::LoadPoFromString(const CString& sContent)
{
	bool bRet = true;

	CStringList lRef;			// #:
	CString sString, sOriginalLine;
	enum
	{
		mode_start, mode_ref, mode_msgid, mode_msgstr
	}
	mode = mode_start;

	int nLine = 1;
	for ( int curPos = 0; bRet; ++nLine )
	{
		CString sLine = sContent.Tokenize( _T("\n"), curPos );
		if ( sLine.IsEmpty() )
			break;

		sOriginalLine = sLine;
		sLine.Trim();

		switch ( sLine[ 0 ] )
		{
		case _T('#'):
			if ( mode != mode_ref && mode != mode_start && mode != mode_msgstr )
			{
				TRACE( "Invalid .po-line #%d: %s\n", nLine, (LPCSTR)CT2A( sOriginalLine ) );
				bRet = false;
				break;
			}
			if ( sLine[ 1 ] == ':' )
			{
				// Ref
				switch ( mode )
				{
				case mode_msgstr:
					// Save previous string
					if ( ! lRef.IsEmpty() && ! sString.IsEmpty() )
						Add( lRef, Decode( sString ) );
					sString.Empty();
					lRef.RemoveAll();
					mode = mode_ref;
					break;

				case mode_ref:
					lRef.AddTail( sString.Trim() );
					sString.Empty();
					break;
				}

				if ( ! sString.IsEmpty() )
					sString += " ";
				sString += sLine.Mid( 2 ).Trim();
			}
			// else Comments
			break;

		case _T('M'):
		case _T('m'):
			if ( ( mode == mode_start || mode == mode_ref ) && _tcsncicmp( sLine, _T("msgid \""), 7 ) == 0 )
			{
				// ID
				lRef.AddTail( sString.Trim() );
				sString.Empty();

				sLine = sLine.Mid( 6, sLine.GetLength() - 6 );
				mode = mode_msgid;
			}
			else if ( mode == mode_msgid && _tcsncicmp( sLine, _T("msgstr \""), 8 ) == 0 )
			{
				// Translation
				if ( ! lRef.IsEmpty() && ! sString.IsEmpty() )
					SetAt( lRef, Decode( sString ) );

				sString.Empty();

				sLine = sLine.Mid( 7, sLine.GetLength() - 7 );
				mode = mode_msgstr;
			}
			else
			{
				// Unknown string
				TRACE( "Invalid .po-line #%d: %s\n", nLine, (LPCSTR)CT2A( sOriginalLine ) );
				bRet = false;
				break;
			}

		case _T('\"'):
			if ( mode != mode_msgid && mode != mode_msgstr )
			{
				TRACE( "Invalid .po-line #%d: %s\n", nLine, (LPCSTR)CT2A( sOriginalLine ) );
				bRet = false;
				break;
			}
			if ( sLine[ sLine.GetLength() - 1 ] == _T('\"') )
			{
				// String continue
				sString += sLine.Mid( 1, sLine.GetLength() - 2 );
			}
			else
			{
				// Unknown string
				TRACE( "Invalid .po-line #%d: %s\n", nLine, (LPCSTR)CT2A( sOriginalLine ) );
				bRet = false;
				break;
			}
			break;

		default:
			// Empty or unknown lines
			;
		}
	} // while

	if ( bRet && mode != mode_msgstr )
	{
		// Unknown string
		TRACE( "Invalid .po-line #%d: %s\n", nLine, (LPCSTR)CT2A( sOriginalLine ) );
		bRet = false;
	}

	if ( bRet )
	{
		// Save last string
		if ( ! lRef.IsEmpty() && ! sString.IsEmpty() )
			Add( lRef, Decode( sString ) );
	}

	return bRet;
}

bool CTranslation::LoadPo(const CString& sFilename)
{
	bool bResult = false;

	// Open input file
	FILE* fileIn = NULL;
	if ( _tfopen_s( &fileIn, _T("\\\\?\\") + sFilename, _T("rt,ccs=UTF-8") ) == 0 && fileIn )
	{
		CString sFile;

		while( ! feof( fileIn ) )
		{
			CString sLine;
			if ( ! _fgetts( sLine.GetBuffer( 1024 ), 1024, fileIn ) )
			{
				sLine.ReleaseBuffer( 0 );
				break;
			}
			sLine.ReleaseBuffer();

			sFile.Append( sLine );			
		}

		bResult = LoadPoFromString( sFile );

		fclose( fileIn );
	}

	return bResult;
}

bool CTranslation::LoadLang(const CString& sFilename, bool bMsgstr)
{
	bool bResult = false;

	// Open input file
	FILE* fileIn = NULL;
	if ( _tfopen_s( &fileIn, _T("\\\\?\\") + sFilename, _T("rt,ccs=UTF-8") ) == 0 && fileIn )
	{
		while( ! feof( fileIn ) )
		{
			CString sLine;
			if ( ! _fgetts( sLine.GetBuffer( 1024 ), 1024, fileIn ) )
			{
				sLine.ReleaseBuffer( 0 );
				break;
			}
			sLine.ReleaseBuffer();

			sLine.TrimLeft();
			sLine.TrimRight( _T("\r\n") );

			if ( sLine.IsEmpty() )
				// Skip empty line
				continue;

			if ( sLine.GetAt( 0 ) == _T('#') )
				// Skip comments
				continue;

			// "key=value"
			const int nSplit = sLine.Find( _T('=') );
			if ( nSplit > 0 )
			{
				const CString sId = sLine.Left( nSplit ).Trim();
				const CString sMsgid = sLine.Mid( nSplit + 1 );

				if ( bMsgstr )
					Add( sId, sMsgid );
				else
					SetAt( sId, sMsgid );

				bResult = true;
			}
		}

		fclose( fileIn );
	}

	return bResult;
}

bool CTranslation::SavePo(const CString& sFilename) const
{
	bool bResult = false;

	// Backup old output file
	CopyFile( _T("\\\\?\\") + sFilename, _T("\\\\?\\") + sFilename + _T(".bak"), FALSE );

	// Create output file
	FILE* fileOut = NULL;
	if ( _tfopen_s( &fileOut, _T("\\\\?\\") + sFilename, _T("wt,ccs=UTF-8") ) == 0 && fileOut )
	{
		// Header
		_fputts(
			_T("msgid \"\"\n")
			_T("msgstr \"\"\n")
			_T("\"Project-Id-Version: \\n\"\n")
			_T("\"POT-Creation-Date: \\n\"\n")
			_T("\"PO-Revision-Date: \\n\"\n")
			_T("\"Last-Translator: \\n\"\n")
			_T("\"Language-Team: \\n\"\n")
			_T("\"MIME-Version: 1.0\\n\"\n")
			_T("\"Content-Type: text/plain; charset=UTF-8\\n\"\n")
			_T("\"Content-Transfer-Encoding: 8bit\\n\"\n"), fileOut );

		for ( POSITION pos = GetHeadPosition(); pos; )
		{
			CString sMsgid;
			CTrans trans;
			GetNext( pos, trans, sMsgid );

			_fputts( _T("\n"), fileOut );
			_fputts( _T("#:") + trans.GetId() + _T("\n"), fileOut );
			_fputts( _T("msgid \"") + Escape( sMsgid ) + _T("\"\n"), fileOut );
			_fputts( _T("msgstr \"") + Escape( trans.m_sMsgstr ) + _T("\"\n"), fileOut );
		}

		bResult = true;

		fclose( fileOut );
	}

	return bResult;
}

bool CTranslation::SaveLang(const CString& sFilename) const
{
	bool bResult = false;

	CRBMap < CString, int > out;
	for ( POSITION posT = GetHeadPosition(); posT; )
	{
		CString sMsgid;
		CTrans trans;
		GetNext( posT, trans, sMsgid );

		for ( POSITION posI = trans.m_sId.GetHeadPosition(); posI; )
		{
			const CString sId = trans.m_sId.GetNext( posI );
			if ( ! trans.m_sMsgstr.IsEmpty() )
				out.SetAt( sId + _T("=") + trans.m_sMsgstr + _T("\n"), 0 );
		}
	}

	// Backup old output file
	CopyFile( _T("\\\\?\\") + sFilename, _T("\\\\?\\") + sFilename + _T(".bak"), FALSE );

	// Create output file
	FILE* fileOut = NULL;
	if ( _tfopen_s( &fileOut, _T("\\\\?\\") + sFilename, _T("wt,ccs=UTF-8") ) == 0 && fileOut )
	{
		// Sorted output
		for ( POSITION pos = out.GetHeadPosition(); pos; )
		{
			_fputts( out.GetNextKey( pos ), fileOut );
		}

		bResult = true;

		fclose( fileOut );
	}

	return bResult;
}
