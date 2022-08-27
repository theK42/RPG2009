#include "game/shared/gamepch.h"
#include "game/tools/datatreetools/datatreeresourcewriter.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTreeHeader::CWriteableDataTreeHeader()
{
	m_bWritten = false;
}

///----------------------------------------------------------------------------

void DataTreeTools::CWriteableDataTreeHeader::Write( XEngine::XTools::XResourceFileWriter & rWriter )
{
	if (!m_bWritten)
	{
		rWriter.ResolveLink(this);
		///Simple stuff
		rWriter.WriteInt32(m_nMaxTrees);
		///Map Objects
		///IntMap
		rWriter.WriteInt32(m_oIntMap.m_nMaxItems);
		rWriter.WriteInt32(m_oIntMap.m_nNumItems);
		rWriter.WriteLink(m_oIntMap.m_pIDIndexPairs);
		///FloatMap
		rWriter.WriteInt32(m_oFloatMap.m_nMaxItems);
		rWriter.WriteInt32(m_oFloatMap.m_nNumItems);
		rWriter.WriteLink(m_oFloatMap.m_pIDIndexPairs);
		///BoolMap
		rWriter.WriteInt32(m_oBoolMap.m_nMaxItems);
		rWriter.WriteInt32(m_oBoolMap.m_nNumItems);
		rWriter.WriteLink(m_oBoolMap.m_pIDIndexPairs);
		///StringMap
		rWriter.WriteInt32(m_oStringMap.m_nMaxItems);
		rWriter.WriteInt32(m_oStringMap.m_nNumItems);
		rWriter.WriteLink(m_oStringMap.m_pIDIndexPairs);
		///ChecskumMap
		rWriter.WriteInt32(m_oChecksumMap.m_nMaxItems);
		rWriter.WriteInt32(m_oChecksumMap.m_nNumItems);
		rWriter.WriteLink(m_oChecksumMap.m_pIDIndexPairs);
		///KeyMap
		rWriter.WriteInt32(m_oKeyMap.m_nMaxItems);
		rWriter.WriteInt32(m_oKeyMap.m_nNumItems);
		rWriter.WriteLink(m_oKeyMap.m_pIDIndexPairs);

		///Map dynamic data
		///IntMap
		rWriter.ResolveLink(m_oIntMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oIntMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oIntMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oIntMap.m_pIDIndexPairs[i].m_nIndex);
		}
		///FloatMap
		rWriter.ResolveLink(m_oFloatMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oFloatMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oFloatMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oFloatMap.m_pIDIndexPairs[i].m_nIndex);
		}
		///BoolMap
		rWriter.ResolveLink(m_oBoolMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oBoolMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oBoolMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oBoolMap.m_pIDIndexPairs[i].m_nIndex);
		}
		///StringMap
		rWriter.ResolveLink(m_oStringMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oStringMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oStringMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oStringMap.m_pIDIndexPairs[i].m_nIndex);
		}
		///ChecksumMap
		rWriter.ResolveLink(m_oChecksumMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oChecksumMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oChecksumMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oChecksumMap.m_pIDIndexPairs[i].m_nIndex);
		}
		///KeyMap
		rWriter.ResolveLink(m_oKeyMap.m_pIDIndexPairs);
		for (int i = 0; i < m_oKeyMap.m_nMaxItems; i++)
		{
			rWriter.WriteUInt32(m_oKeyMap.m_pIDIndexPairs[i].m_nChecksum);
			rWriter.WriteInt32(m_oKeyMap.m_pIDIndexPairs[i].m_nIndex);
		}
		m_bWritten = true;
	}
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

DataTreeTools::CWriteableStringTable::CWriteableStringTable( int nSize, int nNumStrings, XEngine::XAllocator * pAllocator ) :
CStringTable(nSize, nNumStrings, pAllocator)
{
	m_bWritten = false;
}

///----------------------------------------------------------------------------

void DataTreeTools::CWriteableStringTable::Write( XEngine::XTools::XResourceFileWriter & rWriter )
{
	if (!m_bWritten)
	{
		rWriter.ResolveLink(this);
		rWriter.WriteUInt32(m_nSize);
		rWriter.WriteUInt32(m_nUsedSize);
		rWriter.WriteInt32(m_nMaxNumStrings);
		rWriter.WriteInt32(m_nNumStrings);
		rWriter.WriteLink(m_pIndices);
		rWriter.WriteLink(m_pStrings);
		for (int i = 0 ; i < m_nMaxNumStrings; i++)
		{
			rWriter.WriteUInt32(m_pIndices[i]);
		}
		for (uint i = 0; i < m_nSize; i++)
		{
			rWriter.WriteInt8(m_pStrings[i]);
		}
	}
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree::CWriteableDataTree(CWriteableDataTreeHeader * pHeader, XEngine::XAllocator * pAllocator, bool bOwnsHeaderAndStringTable, CWriteableStringTable * pStrings /* = nullptr */) :
CDataTree(pHeader, pAllocator, pStrings)
{
	m_pWritableHeader = pHeader;
	m_pWriteableStrings = pStrings;
	if (pHeader->GetMaxDataTrees() > 0)
	{
		m_ppWriteableDataTrees = XNEW(pAllocator) CWriteableDataTree *[pHeader->GetMaxDataTrees()]; 
	}
	else
	{
		m_ppWriteableDataTrees = nullptr;
	}
	m_bWritten = false;
	m_bOwnsHeaderAndStringTable = bOwnsHeaderAndStringTable;

}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree::~CWriteableDataTree()
{
	///WriteableDataTrees own their children
	for (int i = 0; i < m_nNumDataTrees; i++)
	{
		XDELETE(m_ppWriteableDataTrees[i]);
		m_ppWriteableDataTrees[i] = nullptr;
		m_ppDataTrees[i] = nullptr;
	}
	///And may or may not own their headers and string tables
	if (m_bOwnsHeaderAndStringTable)
	{
		XASSERT_PTR(m_pWritableHeader);
		XDELETE(m_pWritableHeader);
		m_pHeader = nullptr;
		m_pWritableHeader = nullptr;
		if (m_pWriteableStrings)
		{
			XDELETE(m_pWriteableStrings);
			m_pStrings = nullptr;
			m_pWriteableStrings = nullptr;
		}
	}
}

///----------------------------------------------------------------------------

u32 DataTreeTools::CWriteableDataTree::GetType()
{
	return 0XDA7ABA5E;  ///Gonna ask if we can use checksums for this
}

///----------------------------------------------------------------------------

void DataTreeTools::CWriteableDataTree::Write( XEngine::XTools::XResourceDatabaseBuilder & rBuilder, XEngine::XTools::XResourceFileWriter & rWriter, handle hLink )
{
	if (!m_bWritten)
	{
		rWriter.ResolveLink(hLink);
		rWriter.WriteLink(m_pHeader);
		rWriter.WriteLink(m_pInts);
		rWriter.WriteLink(m_pFloats);
		rWriter.WriteLink(m_pBools);
		rWriter.WriteLink(m_pStringIndices);
		rWriter.WriteLink(m_pStrings);
		rWriter.WriteLink(m_pChecksums);
		rWriter.WriteInt32(m_nNumDataTrees);
		rWriter.WriteLink(m_ppDataTrees);
		rWriter.WriteLink(m_pDataTreeMaps);

		m_pWritableHeader->Write(rWriter);
		rWriter.ResolveLink(m_pInts);
		for (int i = 0; i < m_pHeader->GetMaxInts(); i++)
		{
			rWriter.WriteInt32(m_pInts[i]);
		}
		rWriter.ResolveLink(m_pFloats);
		for (int i = 0; i < m_pHeader->GetMaxFloats(); i++)
		{
			rWriter.WriteFloat(m_pFloats[i]);
		}
		rWriter.ResolveLink(m_pBools);
		for (int i = 0; i < m_pHeader->GetMaxBools(); i++)
		{
			rWriter.WriteInt8(m_pBools[i]); ///There is no WriteBool, here's hoping sizeof(bool) is 1 everywhere!
		}
		rWriter.ResolveLink(m_pStringIndices);
		for (int i = 0; i < m_pHeader->GetMaxStrings(); i++)
		{
			rWriter.WriteInt32(m_pStringIndices[i]);
		}
		rWriter.ResolveLink(m_pStrings);
		if (m_pWriteableStrings != nullptr)
		{	
			m_pWriteableStrings->Write(rWriter);
		}
		rWriter.ResolveLink(m_pChecksums);
		for (int i = 0; i < m_pHeader->GetMaxChecksums(); i++)
		{
			rWriter.WriteUInt32(m_pChecksums[i]);
		}

		rWriter.ResolveLink(m_ppDataTrees);
		for (int i = 0; i < m_pHeader->GetMaxDataTrees(); i++)
		{
			rWriter.WriteLink(m_ppDataTrees[i]);
		}
		for (int i = 0; i < m_pHeader->GetMaxDataTrees(); i++)
		{
			m_ppWriteableDataTrees[i]->Write(rBuilder, rWriter, m_ppWriteableDataTrees[i]);
		}

		rWriter.ResolveLink(m_pDataTreeMaps);
		for (int i = 0; i < m_pHeader->GetMaxKeys(); i++)
		{
			rWriter.WriteInt32(m_pDataTreeMaps[i].m_nMaxItems);
			rWriter.WriteInt32(m_pDataTreeMaps[i].m_nNumItems);
			rWriter.WriteLink(m_pDataTreeMaps[i].m_pIDIndexPairs);
		}

		for (int i = 0; i < m_pHeader->GetMaxKeys(); i++)
		{
			rWriter.ResolveLink(m_pDataTreeMaps[i].m_pIDIndexPairs);
			for (int j = 0; j < m_pDataTreeMaps[i].m_nMaxItems; j++)
			{
				rWriter.WriteUInt32(m_pDataTreeMaps[i].m_pIDIndexPairs[j].m_nChecksum);
				rWriter.WriteInt32(m_pDataTreeMaps[i].m_pIDIndexPairs[j].m_nIndex);
			}
		}
		m_bWritten = true;
	}
}

///----------------------------------------------------------------------------

void DataTreeTools::CWriteableDataTree::AddDataTree( CWriteableDataTree * pDataTree )
{
	CDataTree::AddDataTree(pDataTree);
	m_ppWriteableDataTrees[m_nNumDataTrees - 1] = pDataTree;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTreeWrapper::CWriteableDataTreeWrapper( CWriteableDataTree * pWriteableDataTree )
{
	m_pWrappedWriteableDataTree = pWriteableDataTree;
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTreeWrapper::~CWriteableDataTreeWrapper()
{
	XDELETE(m_pWrappedWriteableDataTree);
	m_pWrappedWriteableDataTree = nullptr;
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeWrapper::GetWriteableDataTree()
{
	return m_pWrappedWriteableDataTree;
}

XEngine::XTools::XResourceDatabaseBuilderObject * DataTreeTools::CWriteableDataTreeWrapper::GetWrappedResDBObject()
{
	return m_pWrappedWriteableDataTree;
}

///----------------------------------------------------------------------------