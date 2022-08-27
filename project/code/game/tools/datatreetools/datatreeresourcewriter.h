#pragma once

#include "game/datatree/datatree.h"
#include "game/datatree/stringtable.h"
#include "engine/tools/resource/xresdbbuilder.h"


namespace DataTreeTools
{
	public class CWriteableDataTreeHeader : public CDataTree::CHeader
	{
	public:
		CWriteableDataTreeHeader();
		virtual void	Write( XEngine::XTools::XResourceFileWriter & rWriter );
	private:	
		bool m_bWritten;
	};

	public class CWriteableStringTable : public CStringTable
	{
	public:
		CWriteableStringTable(int nSize, int nNumStrings, XEngine::XAllocator * pAllocator);
		virtual void	Write( XEngine::XTools::XResourceFileWriter & rWriter );
	private:
		bool m_bWritten;
	};

	public class CWriteableDataTree : public CDataTree, public XEngine::XTools::XResourceDatabaseBuilderNamedObject
	{
	public:
		CWriteableDataTree(CWriteableDataTreeHeader * pHeader, XEngine::XAllocator * pAllocator, bool bOwnsHeaderAndStringTable, CWriteableStringTable * pStrings = nullptr);
		~CWriteableDataTree();
		virtual u32				GetType();
		virtual void			Write( XEngine::XTools::XResourceDatabaseBuilder & rBuilder, XEngine::XTools::XResourceFileWriter & rWriter, handle hLink );
		void					AddDataTree(CWriteableDataTree * pDataTree); ///shadows base class version.  On purpose, but slightly evil.  Like inheritance but without the polymorphism!

	private:
		CWriteableDataTreeHeader *	m_pWritableHeader;
		CWriteableStringTable *		m_pWriteableStrings;
		CWriteableDataTree **		m_ppWriteableDataTrees;
		bool 						m_bOwnsHeaderAndStringTable;
		bool 						m_bWritten;
	};

	public ref class CWriteableDataTreeWrapper : public XWEngine::XWTools::XWResourceDatabaseBuilderObject
	{
	public:
		CWriteableDataTreeWrapper(CWriteableDataTree * pWriteableDataTree);
		~CWriteableDataTreeWrapper();
		CWriteableDataTree * GetWriteableDataTree();
		virtual XEngine::XTools::XResourceDatabaseBuilderObject * GetWrappedResDBObject();
	private:
		CWriteableDataTree *		m_pWrappedWriteableDataTree;

	};

}