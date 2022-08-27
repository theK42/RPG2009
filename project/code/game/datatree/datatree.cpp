#include "game/shared/gamepch.h"
#include "game/datatree/datatree.h"
#include "game/datatree/stringtable.h"

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CDataTree::SIDIndexMap::SIDIndexPair::SIDIndexPair()
{
	m_nChecksum = CRC(0x00000000, "");
	m_nIndex = -1;
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CDataTree::SIDIndexMap::SIDIndexMap()
{
	m_nMaxItems = 0;
	m_nNumItems = 0;
	m_pIDIndexPairs = nullptr;
}

///------------------------------------------------------------------------

CDataTree::SIDIndexMap::~SIDIndexMap()
{
	if (m_pIDIndexPairs != nullptr)
	{
		XDELETE_ARRAY(m_pIDIndexPairs);
		m_pIDIndexPairs = nullptr;
		m_nNumItems = 0;
		m_nMaxItems = 0;
	}
}

///------------------------------------------------------------------------

void CDataTree::SIDIndexMap::Setup( int nMaxItems, XEngine::XAllocator * pAllocator )
{
	XASSERT(m_nMaxItems == 0);
	m_nMaxItems = nMaxItems;
	if (nMaxItems != 0)
	{
		m_pIDIndexPairs = XNEW(pAllocator) SIDIndexPair[nMaxItems];
	}
}

///------------------------------------------------------------------------

void CDataTree::SIDIndexMap::AddItem( XChecksum nID )
{
	XASSERT (m_nNumItems < m_nMaxItems);
	XASSERT (!HasItemIndex(nID));
	int nInsertionIndex = 0;
	while (nInsertionIndex < m_nNumItems && m_pIDIndexPairs[nInsertionIndex].m_nChecksum < nID)
	{
		nInsertionIndex++;
	}
	int nMovementIndex = m_nNumItems;
	while (nMovementIndex > nInsertionIndex)
	{
		m_pIDIndexPairs[nMovementIndex] = m_pIDIndexPairs[nMovementIndex - 1];
		nMovementIndex--;
	}
	m_pIDIndexPairs[nInsertionIndex].m_nChecksum = nID;
	m_pIDIndexPairs[nInsertionIndex].m_nIndex = m_nNumItems++; ///Post-increment!  What fun!
}

///------------------------------------------------------------------------

bool CDataTree::SIDIndexMap::HasItemIndex( XChecksum nID ) const
{
	return GetItemIndex(nID) >= 0;
}

///------------------------------------------------------------------------

int CDataTree::SIDIndexMap::GetItemIndex( XChecksum nID ) const
{
	int nMinIndex = 0;
	int nMaxIndex = m_nNumItems - 1;
	int nCurIndex = 0;
	while (nMinIndex < nMaxIndex)
	{
		nCurIndex = ((nMaxIndex - nMinIndex) / 2) + nMinIndex;
		if (m_pIDIndexPairs[nCurIndex].m_nChecksum <= nID)
		{
			nMinIndex = nCurIndex + 1;
		}
		if (m_pIDIndexPairs[nCurIndex].m_nChecksum >= nID)
		{
			nMaxIndex = nCurIndex - 1;
		}
	}
	if (m_pIDIndexPairs[nCurIndex].m_nChecksum != nID)
	{
		nCurIndex = -1;	
	}
	return nCurIndex;
}

///------------------------------------------------------------------------

XChecksum CDataTree::SIDIndexMap::GetItem( int nIndex ) const
{
	XASSERT(IsBounded(nIndex, 0, m_nNumItems - 1));
	return m_pIDIndexPairs[nIndex].m_nChecksum;
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CDataTree::CHeader::CHeader()
{
	m_nMaxTrees = 0;
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupInts( int nMaxInts, XEngine::XAllocator * pAllocator )
{
	m_oIntMap.Setup(nMaxInts, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupFloats( int nMaxFloats, XEngine::XAllocator * pAllocator )
{
	m_oFloatMap.Setup(nMaxFloats, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupBools( int nMaxBools, XEngine::XAllocator * pAllocator )
{
	m_oBoolMap.Setup(nMaxBools, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupStrings( int nMaxStrings, XEngine::XAllocator * pAllocator )
{
	m_oStringMap.Setup(nMaxStrings, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupChecksums( int nMaxChecksums, XEngine::XAllocator * pAllocator )
{
	m_oChecksumMap.Setup(nMaxChecksums, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::SetupTrees( int nMaxTrees, int nMaxKeys, XEngine::XAllocator * pAllocator )
{
	XASSERT(m_nMaxTrees == 0);
	m_nMaxTrees = nMaxTrees;
	if (nMaxKeys > 0)
	m_oKeyMap.Setup(nMaxKeys, pAllocator);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddInt( XChecksum nID )
{
	m_oIntMap.AddItem(nID);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddFloat( XChecksum nID )
{
	m_oFloatMap.AddItem(nID);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddBool( XChecksum nID )
{
	m_oBoolMap.AddItem(nID);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddString( XChecksum nID )
{
	m_oStringMap.AddItem(nID);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddChecksum( XChecksum nID )
{
	m_oChecksumMap.AddItem(nID);
}

///------------------------------------------------------------------------

void CDataTree::CHeader::AddKey( XChecksum nKey )
{
	m_oKeyMap.AddItem(nKey);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetIntIndex( XChecksum nID ) const
{
	return m_oIntMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetFloatIndex( XChecksum nID ) const
{
	return m_oFloatMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetBoolIndex( XChecksum nID ) const
{
	return m_oBoolMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetStringIndex( XChecksum nID ) const
{
	return m_oStringMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetChecksumIndex( XChecksum nID ) const
{
	return m_oChecksumMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetKeyIndex( XChecksum nID ) const
{
	return m_oKeyMap.GetItemIndex(nID);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxInts() const
{
	return m_oIntMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxFloats() const
{
	return m_oFloatMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxBools() const
{
	return m_oBoolMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxStrings() const
{
	return m_oStringMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxChecksums() const
{
	return m_oChecksumMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxKeys() const
{
	return m_oKeyMap.m_nMaxItems;
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetMaxDataTrees() const
{
	return m_nMaxTrees;
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasInt( XChecksum nID ) const
{
	return m_oIntMap.HasItemIndex(nID);
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasFloat( XChecksum nID ) const
{
	return m_oFloatMap.HasItemIndex(nID);
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasBool( XChecksum nID ) const
{
	return m_oBoolMap.HasItemIndex(nID);
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasString( XChecksum nID ) const
{
	return m_oStringMap.HasItemIndex(nID);
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasChecksum( XChecksum nID ) const
{
	return m_oChecksumMap.HasItemIndex(nID);
}

///------------------------------------------------------------------------

bool CDataTree::CHeader::HasKey( XChecksum nKey ) const
{
	return m_oKeyMap.HasItemIndex(nKey);
}

///------------------------------------------------------------------------

int CDataTree::CHeader::GetNumKeys() const
{
	return m_oKeyMap.m_nNumItems;
}

///------------------------------------------------------------------------

XChecksum CDataTree::CHeader::GetKey( int nIndex ) const
{
	return m_oKeyMap.GetItem(nIndex);
}

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CDataTree::CDataTree( CHeader * pHeader, XEngine::XAllocator * pAllocator, CStringTable * pStringTable /* = nullptr */)
{
	m_pHeader = pHeader;
	///I really don't understand why the C++ new operator doesn't return nullptr for size zero arrays.
	if (pHeader->GetMaxInts() > 0)
	{
		m_pInts = XNEW(pAllocator) int[pHeader->GetMaxInts()];
	}
	else
	{
		m_pInts = nullptr;
	}

	if (pHeader->GetMaxFloats() > 0)
	{
		m_pFloats = XNEW(pAllocator) float[pHeader->GetMaxFloats()];
	}
	else
	{
		m_pFloats = nullptr;
	}	
	
	if (pHeader->GetMaxBools() > 0)
	{
		m_pBools = XNEW(pAllocator) bool[pHeader->GetMaxBools()];
	}
	else
	{
		m_pBools = nullptr;
	}

	if (pHeader->GetMaxStrings() > 0)
	{
		m_pStringIndices = XNEW(pAllocator) int[pHeader->GetMaxStrings()];
		XASSERT_PTR(pStringTable);
		m_pStrings = pStringTable;
	}
	else
	{
		m_pStringIndices = nullptr;
		m_pStrings = nullptr;
	}
	
	if (pHeader->GetMaxChecksums() > 0)
	{
		m_pChecksums = XNEW(pAllocator) XChecksum[pHeader->GetMaxChecksums()];
	}
	else
	{
		m_pChecksums = nullptr;
	}

	if (pHeader->GetMaxDataTrees() > 0)
	{
		m_ppDataTrees = XNEW(pAllocator) CDataTree *[pHeader->GetMaxDataTrees()];	
		if (pHeader->GetMaxKeys() > 0)
		{
			m_pDataTreeMaps = XNEW(pAllocator) SIDIndexMap[pHeader->GetMaxKeys()];
			for (int i = 0; i < pHeader->GetMaxKeys(); i++)
			{
				m_pDataTreeMaps[i].Setup(pHeader->GetMaxDataTrees(), pAllocator);
			}
		}
		else
		{
			m_pDataTreeMaps = nullptr;
		}
	}
	else
	{
		XASSERT(pHeader->GetMaxKeys() == 0);
		m_ppDataTrees = nullptr;
		m_pDataTreeMaps = nullptr;
	}
	///Either way
	m_nNumDataTrees = 0;
}

///------------------------------------------------------------------------

CDataTree::~CDataTree()
{
	if (m_pInts != nullptr)
	{
		XDELETE_ARRAY(m_pInts);
		m_pInts = nullptr;
	}

	if (m_pFloats != nullptr)
	{
		XDELETE_ARRAY(m_pFloats);
		m_pFloats = nullptr;
	}

	if (m_pBools != nullptr)
	{
		XDELETE_ARRAY(m_pBools);
		m_pBools = nullptr;
	}

	if (m_pStrings != nullptr)
	{
		XDELETE_ARRAY(m_pStringIndices);
		m_pStringIndices = nullptr;
		XDELETE(m_pStrings);
		m_pStrings  = nullptr;
	}

	if (m_pChecksums != nullptr)
	{
		XDELETE_ARRAY(m_pChecksums);
		m_pChecksums = nullptr;
	}

	if (m_pDataTreeMaps != nullptr)
	{
		XDELETE_ARRAY(m_pDataTreeMaps);
		m_pDataTreeMaps = nullptr;
	}

	if (m_ppDataTrees != nullptr)
	{
		///At present, a default DataTree does not delete it's children on destruction.
		///WriteableDataTrees (tool version) does delete it's children, and due to the 
		///lack of polymorphism it MUST if the entire destructor chain is to be called
		///correctly.  This may not be a great design, but it's the best I can think of
		///since XResource doesn't support v-tables.
		XDELETE_ARRAY(m_ppDataTrees);
		m_ppDataTrees = nullptr;
		m_nNumDataTrees = 0;
	}

}

///------------------------------------------------------------------------

bool CDataTree::HasInt( XChecksum nID ) const
{
	return m_pHeader->HasInt(nID);
}

///------------------------------------------------------------------------

int CDataTree::GetInt( XChecksum nID ) const
{
	XASSERT(HasInt(nID));
	return m_pInts[m_pHeader->GetIntIndex(nID)];
}

///------------------------------------------------------------------------

void CDataTree::SetInt( XChecksum nID, int nValue )
{
	if (!HasInt(nID))
	{
		m_pHeader->AddInt(nID);
	}
	m_pInts[m_pHeader->GetIntIndex(nID)] = nValue;
}

///------------------------------------------------------------------------

bool CDataTree::HasFloat( XChecksum nID ) const
{
	return m_pHeader->HasFloat(nID);
}

///------------------------------------------------------------------------

float CDataTree::GetFloat( XChecksum nID ) const
{
	XASSERT(HasFloat(nID));
	return m_pFloats[m_pHeader->GetFloatIndex(nID)];
}

///------------------------------------------------------------------------

void CDataTree::SetFloat( XChecksum nID, float nValue )
{
	if (!HasFloat(nID))
	{
		m_pHeader->AddFloat(nID);
	}
	m_pFloats[m_pHeader->GetFloatIndex(nID)] = nValue;
}

///------------------------------------------------------------------------

bool CDataTree::HasBool( XChecksum nID ) const
{
	return m_pHeader->HasBool(nID);
}

///------------------------------------------------------------------------

bool CDataTree::GetBool( XChecksum nID ) const
{
	XASSERT(HasBool(nID));
	return m_pBools[m_pHeader->GetBoolIndex(nID)];
}

///------------------------------------------------------------------------

void CDataTree::SetBool( XChecksum nID, bool nValue )
{
	if (!HasFloat(nID))
	{
		m_pHeader->AddBool(nID);
	}
	m_pFloats[m_pHeader->GetFloatIndex(nID)] = nValue;
}

///------------------------------------------------------------------------

bool CDataTree::HasString( XChecksum nID ) const
{
	return m_pHeader->HasString(nID);
}

///------------------------------------------------------------------------

char const * CDataTree::GetString( XChecksum nID ) const
{
	XASSERT(HasString(nID));
	return m_pStrings->GetString(m_pStringIndices[m_pHeader->GetStringIndex(nID)]);
}

///------------------------------------------------------------------------

void CDataTree::AddString( XChecksum nID, char const * pString )
{
	if (!HasString(nID))
	{
		m_pHeader->AddString(nID);
	}
	m_pStringIndices[m_pHeader->GetStringIndex(nID)] = m_pStrings->AddString(pString);
}

///------------------------------------------------------------------------

bool CDataTree::HasChecksum( XChecksum nID ) const
{
	return m_pHeader->HasChecksum(nID);
}

///------------------------------------------------------------------------

XChecksum CDataTree::GetChecksum( XChecksum nID ) const
{
	XASSERT(HasChecksum(nID));
	return m_pChecksums[m_pHeader->GetChecksumIndex(nID)];
}

///------------------------------------------------------------------------

void CDataTree::AddChecksum( XChecksum nID, XChecksum nValue)
{
	if (!m_pHeader->HasChecksum(nID))
	{
		m_pHeader->AddChecksum(nID);
	}
	XASSERT(m_pChecksums[m_pHeader->GetChecksumIndex(nID)] == CRC(0x00000000, ""));
	m_pChecksums[m_pHeader->GetChecksumIndex(nID)] = nValue;
}

///------------------------------------------------------------------------

bool CDataTree::HasKey( XChecksum nKey ) const
{
	return m_pHeader->HasKey(nKey);
}

///------------------------------------------------------------------------

void CDataTree::AddKey( XChecksum nKey ) const
{
	///These asserts aren't strictly necessary, but they prevent the key from being partially added before you realize you're screwed.
#ifdef XENABLE_ASSERTS
	///Ensure that all current child trees can support this key
	for (int i = 0; i < m_nNumDataTrees; i++)
	{
		XASSERT(m_ppDataTrees[i]->HasChecksum(nKey));
	}
	///And that it is unique for all current child trees
	for (int i = 0; i < m_nNumDataTrees; i++)
	{
		XChecksum nCurrent = m_ppDataTrees[i]->GetChecksum(nKey);
		for (int j = i + 1; j < m_nNumDataTrees; j++)
		{
			XASSERT(nCurrent != m_ppDataTrees[j]->GetChecksum(nKey));
		}
	}
#endif
	if (!m_pHeader->HasKey(nKey))
	{
		m_pHeader->AddKey(nKey);
	}
	SIDIndexMap & pMap = m_pDataTreeMaps[m_pHeader->GetKeyIndex(nKey)];
	for (int i = 0; i < m_nNumDataTrees; i++)
	{
		pMap.AddItem(m_ppDataTrees[i]->GetChecksum(nKey));
	}
}

///------------------------------------------------------------------------

int CDataTree::GetNumDataTrees() const
{
	return m_nNumDataTrees;
}

///------------------------------------------------------------------------

CDataTree * CDataTree::GetDataTree( int nIndex ) const
{
	XASSERT(IsBounded(nIndex, 0, m_nNumDataTrees - 1));
	return m_ppDataTrees[nIndex];
}

///------------------------------------------------------------------------

bool CDataTree::HasDataTree( XChecksum nKey, XChecksum nID ) const
{
	XASSERT(HasKey(nKey));
	int nKeyIndex = m_pHeader->GetKeyIndex(nKey);
	XASSERT(m_pDataTreeMaps[nKeyIndex].HasItemIndex(nID));
	return m_ppDataTrees[m_pDataTreeMaps[nKeyIndex].GetItemIndex(nID)] != nullptr;
}

///------------------------------------------------------------------------

CDataTree * CDataTree::GetDataTree( XChecksum nKey, XChecksum nID ) const
{
	XASSERT(HasDataTree(nKey, nID));
	return m_ppDataTrees[m_pDataTreeMaps[m_pHeader->GetKeyIndex(nKey)].GetItemIndex(nID)];
}

void CDataTree::AddDataTree( CDataTree * pDataTree )
{
	///Again, these asserts are here just to catch the problem before it starts to change the DataTree
#ifdef XENABLE_ASSERTS
	///Ensure the new child supports all keys
	for (int i = 0; i < m_pHeader->GetNumKeys(); i++)
	{
		XChecksum nCurrentKey = m_pHeader->GetKey(i);
		XASSERT(pDataTree->HasChecksum(nCurrentKey));
		XASSERT(!m_pDataTreeMaps[m_pHeader->GetKeyIndex(nCurrentKey)].HasItemIndex(pDataTree->GetChecksum(nCurrentKey)));
	}
#endif
	m_ppDataTrees[m_nNumDataTrees++] = pDataTree; ///Take that Pre-increment!
	for (int i = 0; i < m_pHeader->GetNumKeys(); i++)
	{
		XChecksum nCurrentKey = m_pHeader->GetKey(i);
		m_pDataTreeMaps[m_pHeader->GetKeyIndex(nCurrentKey)].AddItem(pDataTree->GetChecksum(nCurrentKey));
	}
}
///------------------------------------------------------------------------
