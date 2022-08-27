#include "game/shared/gamepch.h"
#include "game/datatree/stringtable.h"

#include <string.h>

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CStringTable::CStringTable( int nSize, int nNumStrings, XEngine::XAllocator * pAllocator )
{
	XASSERT(nNumStrings > 0);
	XASSERT(nSize >= nNumStrings);
	m_nSize = nSize;
	m_nUsedSize = 0;
	m_nMaxNumStrings = nNumStrings;
	m_nNumStrings = 0;
	m_pIndices = XNEW(pAllocator) size_t[nNumStrings];
	m_pStrings = XNEW(pAllocator) char[nSize];
}

///------------------------------------------------------------------------

CStringTable::~CStringTable()
{
	XDELETE_ARRAY(m_pStrings);
	XDELETE_ARRAY(m_pIndices);
	m_nNumStrings = 0;
	m_nMaxNumStrings = 0;
	m_nSize = 0;
	m_nUsedSize = 0;
}

///------------------------------------------------------------------------

int CStringTable::AddString( char const * pString )
{
	size_t nStringLength = strlen(pString) + 1; ///strlen leaves out the Terminator.  Arnold is angry!
	XASSERT(nStringLength <= m_nSize - m_nUsedSize);
	XASSERT(m_nNumStrings < m_nMaxNumStrings);
	m_pIndices[m_nNumStrings] = nStringLength;
	strcpy_s(&m_pStrings[m_nUsedSize], nStringLength, pString);
	m_nUsedSize += nStringLength;
	return m_nNumStrings++;  ///Postincrement!
}

///------------------------------------------------------------------------

char const * CStringTable::GetString( int nIndex ) const
{
	XASSERT(nIndex < m_nNumStrings);
	return &m_pStrings[m_pIndices[nIndex]];
}

///------------------------------------------------------------------------