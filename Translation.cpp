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

CStringA CTranslation::UTF8Encode(__in_bcount(nInput) LPCWSTR psInput, __in int nInput)
{
	CStringA strUTF8;
	int nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput, strUTF8.GetBuffer( nInput * 4 + 1 ), nInput * 4 + 1, nullptr, nullptr );
	if ( nUTF8 == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER )
	{
		nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput, nullptr, 0, nullptr, nullptr );
		nUTF8 = ::WideCharToMultiByte( CP_UTF8, 0, psInput, nInput, strUTF8.GetBuffer( nUTF8 ), nUTF8, nullptr, nullptr );
	}
	strUTF8.ReleaseBuffer( nUTF8 );
	return strUTF8;
}

CStringA CTranslation::UTF8Encode(__in const CStringW& strInput)
{
	return UTF8Encode( strInput, strInput.GetLength() );
}

CString CTranslation::EscapePo(__in CString str)
{
	str.Replace( _T("\""), _T("\\\"") );
	str.Replace( _T("\t"), _T("\\t") );
	str.Replace( _T("\n"), _T("\\n") );
	str.Remove( _T('\r') );
	return str;
}

CString CTranslation::EscapeLang(__in CString str)
{
	str.Replace( _T( "\n" ), _T( "\\n" ) );
	return str;
}

CString CTranslation::Decode(__in CString str)
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
					for ( int nRefPos = 0; ; )
					{
						const CString sRef = sString.Tokenize( _T( " " ), nRefPos );
						if ( sRef.IsEmpty() )
							break;
						lRef.AddTail( sRef );
					}
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
				for ( int nRefPos = 0; ; )
				{
					const CString sRef = sString.Tokenize( _T( " " ), nRefPos );
					if ( sRef.IsEmpty() )
						break;
					lRef.AddTail( sRef );
				}
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
	FILE* fileIn = nullptr;
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

CString CTranslation::Get(const CString& sId) const
{
	CString sMsgid;
	if ( m_IdToMsgid.Lookup( sId, sMsgid ) )
	{
		CTrans trans;
		if ( m_MsgidToTrans.Lookup( sMsgid, trans ) )
		{
			return trans.m_sMsgstr;
		}
	}
	return CString();
}

bool CTranslation::LoadLang(const CString& sFilename, bool bMsgstr, const CString& sAndSaveToFilename)
{
	bool bResult = false;

	FILE* fileAndSaveTo = nullptr;
	if ( ! sAndSaveToFilename.IsEmpty() )
	{
		// Backup old output file
		CopyFile( _T( "\\\\?\\" ) + sAndSaveToFilename, _T( "\\\\?\\" ) + sAndSaveToFilename + _T( ".bak" ), FALSE );

		// Create output file
		if ( _tfopen_s( &fileAndSaveTo, _T( "\\\\?\\" ) + sAndSaveToFilename, _T( "wb" ) ) != 0 || ! fileAndSaveTo )
		{
			return false;
		}
	}

	// Open input file
	FILE* fileIn = nullptr;
	if ( _tfopen_s( &fileIn, _T("\\\\?\\") + sFilename, _T("rt,ccs=UTF-8") ) == 0 && fileIn )
	{
		while( ! feof( fileIn ) )
		{
			bool bSaved = false;

			CString sLine;
			if ( ! _fgetts( sLine.GetBuffer( 1024 ), 1024, fileIn ) )
			{
				sLine.ReleaseBuffer( 0 );
				break;
			}
			sLine.ReleaseBuffer();

			// "key=value"
			const int nSplit = sLine.Find( _T('=') );
			if ( nSplit > 0 )
			{
				const CString sId = sLine.Left( nSplit ).Trim();
				const CString sMsgid = sLine.Mid( nSplit + 1 ).TrimRight( _T("\r\n") );

				if ( ! sId.IsEmpty() &&				// Skip empty key line
					sId.GetAt( 0 ) != _T( '#' ) )	// Skip comments
				{
					if ( fileAndSaveTo )
					{
						const CString sMsgstr = Get( sId );
						if ( ! sMsgstr.IsEmpty() )
						{
							fputs( UTF8Encode( sId ) + "=" + UTF8Encode( EscapeLang( sMsgstr ) ) + "\n", fileAndSaveTo );
							bSaved = true;
						}
					}
					else
					{
						if ( bMsgstr )
							Add( sId, sMsgid );
						else
							SetAt( sId, sMsgid );
					}

					bResult = true;
				}
			}

			if ( fileAndSaveTo && ! bSaved )
			{
				fputs( UTF8Encode( sLine ), fileAndSaveTo );
			}
		}

		fclose( fileIn );
	}

	if ( fileAndSaveTo )
		fclose( fileAndSaveTo );

	return bResult;
}

bool CTranslation::SavePo(const CString& sFilename) const
{
	bool bResult = false;

	// Backup old output file
	CopyFile( _T("\\\\?\\") + sFilename, _T("\\\\?\\") + sFilename + _T(".bak"), FALSE );

	// Create output file
	FILE* fileOut = nullptr;
	if ( _tfopen_s( &fileOut, _T("\\\\?\\") + sFilename, _T("wb") ) == 0 && fileOut )
	{
		// Header
		fputs(
			"msgid \"\"\n"
			"msgstr \"\"\n"
			"\"Project-Id-Version: \\n\"\n"
			"\"POT-Creation-Date: \\n\"\n"
			"\"PO-Revision-Date: \\n\"\n"
			"\"Last-Translator: \\n\"\n"
			"\"Language-Team: \\n\"\n"
			"\"MIME-Version: 1.0\\n\"\n"
			"\"Content-Type: text/plain; charset=UTF-8\\n\"\n"
			"\"Content-Transfer-Encoding: 8bit\\n\"\n"
			"\"X-Poedit-SourceCharset: UTF-8\\n\"\n", fileOut );

		for ( POSITION pos = GetHeadPosition(); pos; )
		{
			CString sMsgid;
			CTrans trans;
			GetNext( pos, trans, sMsgid );

			fputs( "\n#:" + UTF8Encode( trans.GetId() ) + "\n", fileOut );
			fputs( "msgid \"" + UTF8Encode( EscapePo( sMsgid ) ) + "\"\n", fileOut );
			fputs( "msgstr \"" + UTF8Encode( EscapePo( trans.m_sMsgstr ) ) + "\"\n", fileOut );
		}

		bResult = true;

		fclose( fileOut );
	}

	return bResult;
}

bool CTranslation::SaveLang(const CString& sFilename) const
{
	bool bResult = false;

	// Sort output strings excluding empty ones
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
			{
				out.SetAt( sId + _T("=") + EscapeLang( trans.m_sMsgstr ), 0 );
			}
		}
	}

	// Backup old output file
	CopyFile( _T("\\\\?\\") + sFilename, _T("\\\\?\\") + sFilename + _T(".bak"), FALSE );

	// Create output file
	FILE* fileOut = nullptr;
	if ( _tfopen_s( &fileOut, _T("\\\\?\\") + sFilename, _T("wb") ) == 0 && fileOut )
	{
		for ( POSITION pos = out.GetHeadPosition(); pos; )
		{
			fputs( UTF8Encode( out.GetNextKey( pos ) ) + "\n", fileOut );
		}

		bResult = true;

		fclose( fileOut );
	}

	return bResult;
}
