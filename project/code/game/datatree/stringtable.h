#pragma once

class CStringTable
{
public:
	CStringTable(int nSize, int nNumStrings, XEngine::XAllocator * pAllocator);
	~CStringTable();
	int AddString(char const * pString);
	char const * GetString(int nIndex) const;
protected:
	size_t		m_nSize;
	size_t		m_nUsedSize;
	int			m_nMaxNumStrings;
	int			m_nNumStrings;
	size_t *	m_pIndices;
	char *		m_pStrings;
};