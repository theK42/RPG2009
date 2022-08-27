#include "game/shared/gamepch.h"
#include "game/basegame/heap.h"
#ifdef XENABLE_ASSERTS ///for Dump
#include <cstdio>
#endif

///------------------------------------------------------------------------
///------------------------------------------------------------------------

CHeapAllocator::CHeapAllocator( XEngine::XAllocator * pParent, size_t nSize, int nNodes )
{
	m_pHeapNodes = XNEW(pParent) SHeapNode[nNodes];
	for (int i = 0; i < nNodes; i++)
	{
		m_oFreeNodes.PushFront(&m_pHeapNodes[i]);
	}
	m_pMemory = XNEW(pParent) u8[nSize];
	SHeapNode * pNode = m_oFreeNodes.PopFront();
	pNode->m_bUsed = false;
	pNode->m_pMemory = m_pMemory;
	pNode->m_nSize = nSize;
	Register( m_pMemory, m_pMemory + nSize);
}

///------------------------------------------------------------------------

CHeapAllocator::~CHeapAllocator()
{
#ifdef XENABLE_ASSERTS
	if (!IsEmpty())
	{
		XASSERT_MSG(false, "Memory leak!");
		Dump();
	}
#endif
	XDELETE_ARRAY(m_pMemory);
	m_oFreeNodes.Reset();
	m_oUsedNodes.Reset();
	XDELETE_ARRAY(m_pHeapNodes);
	Unregister();
}

///------------------------------------------------------------------------

void * CHeapAllocator::Allocate( uaddr nSize, uaddr nAlignment, uaddr nAlignmentOffset, char const * pFile, int nLine )
{
	///Use first fit to find a node to use or split
	SHeapNode * pNode = m_oUsedNodes.GetFront();
	bool bFound = !pNode->m_bUsed && pNode->m_nSize >= nSize;
	size_t nAlignmentOverhead = 0;
	while (!bFound && pNode != m_oUsedNodes.GetEndMarker())
	{
		pNode = pNode->GetNext<0>();
		if (!pNode->m_bUsed)
		{
			nAlignmentOverhead = XEngine::AlignUp(pNode->m_pMemory + nAlignmentOffset, nAlignment) - nAlignmentOffset - pNode->m_pMemory;
			bFound = pNode->m_nSize >= nSize + nAlignmentOffset;
		}
	}

	if (!bFound)
	{
		XASSERT_MSG(bFound, "Not enough contiguous memory for allocation of %d bytes.", nSize);
		Dump();
		return nullptr;
	}
	SHeapNode * pAllocationNode;
	if (pNode->m_nSize == nSize)
	{
		pAllocationNode = pNode;
	}
	else
	{
		if (m_oFreeNodes.IsEmpty())
		{
			XASSERT_MSG(false, "Not enough free nodes in heap.");
			Dump();
			return nullptr;
		}
		pAllocationNode = m_oFreeNodes.PopFront();
		m_oUsedNodes.InsertBefore(pAllocationNode, pNode);
		pAllocationNode->m_pMemory = pNode->m_pMemory;
		pAllocationNode->m_nSize = nSize + nAlignmentOverhead;
		pNode->m_pMemory += nSize + nAlignmentOverhead;
		pNode->m_nSize -= nSize + nAlignmentOverhead;
	}
	pAllocationNode->m_bUsed = true;
	pAllocationNode->m_pFile = pFile;
	pAllocationNode->m_nLine = nLine;
	pAllocationNode->m_nAlignmentOverhead = nAlignmentOverhead;
	return pAllocationNode->m_pMemory + pAllocationNode->m_nAlignmentOverhead;
}

///------------------------------------------------------------------------

void * CHeapAllocator::Reallocate( void * p, uaddr nNewSize, uaddr nNewAlignment, uaddr nAlignmentOffset, char const * pFile, int nLine )
{
	XASSERT_MSG(false, "Yeah, I don't really think this is a good idea, but if someone wants to implement this function they can be my guest.");
	return nullptr;
}

///------------------------------------------------------------------------

void CHeapAllocator::Free( void * p )
{
	SHeapNode * pNode = FindNode(p);
	XASSERT_PTR(pNode);
	XASSERT(pNode->m_bUsed);
	SHeapNode * pPrecedingNode = pNode->GetPrev<0>();
	SHeapNode * pAntecedingNode = pNode->GetNext<0>();
	if (pPrecedingNode != m_oUsedNodes.GetEndMarker() && pPrecedingNode->m_bUsed == false)
	{
		XASSERT(pPrecedingNode->m_pMemory + pPrecedingNode->m_nSize == pNode->m_pMemory);
		pPrecedingNode->m_nSize += pNode->m_nSize;
		if (pAntecedingNode != m_oUsedNodes.GetEndMarker() && pAntecedingNode->m_bUsed == false)
		{
			XASSERT(pNode->m_pMemory + pNode->m_nSize == pAntecedingNode->m_pMemory);
			pPrecedingNode->m_nSize += pAntecedingNode->m_nSize;
			m_oUsedNodes.Remove(pAntecedingNode);
			pAntecedingNode->m_nSize = 0;
			pAntecedingNode->m_pMemory = 0;
			m_oFreeNodes.PushFront(pAntecedingNode);
		}
	}
	else if (pAntecedingNode != m_oUsedNodes.GetEndMarker()&& pAntecedingNode->m_bUsed == false)
	{
		XASSERT(pNode->m_pMemory + pNode->m_nSize == pAntecedingNode->m_pMemory);
		pAntecedingNode->m_nSize += pNode->m_nSize;
		pAntecedingNode->m_pMemory = pNode->m_pMemory;
	}
	m_oUsedNodes.Remove(pNode);
	pNode->m_nSize = 0;
	pNode->m_pMemory = nullptr;
	pNode->m_pFile = nullptr;
	pNode->m_nLine = -1;
	m_oFreeNodes.PushFront(pNode);
}

///------------------------------------------------------------------------

CHeapAllocator::SHeapNode * CHeapAllocator::FindNode( void * pPointer )
{
	SHeapNode * pHeapNode = m_oUsedNodes.GetFront();
	while (pHeapNode != m_oUsedNodes.GetEndMarker())
	{
		if (pHeapNode->m_pMemory + pHeapNode->m_nAlignmentOverhead == pPointer)
		{
			break;
		}
		pHeapNode = pHeapNode->GetNext<0>();
	}
	if (pHeapNode == m_oUsedNodes.GetEndMarker())
	{
		return nullptr;
	}
	return pHeapNode;
}

///------------------------------------------------------------------------

void CHeapAllocator::Dump()
{
	///Perhaps another define is more appropriate...but I think this is fine for now.
#ifdef XENABLE_ASSERTS
	std::printf( "Heap Dump:" );
	SHeapNode * pNode = m_oUsedNodes.GetFront();
	while(pNode != m_oUsedNodes.GetEndMarker())
	{
		if (pNode->m_bUsed)
		{
			std::printf("%10d bytes used:	%s	line %3d", pNode->m_nSize, pNode->m_pFile, pNode->m_nLine);
		}
		else
		{
			std::printf("%10d bytes free", pNode->m_nSize);
		}
	}
#endif
}

///------------------------------------------------------------------------

bool CHeapAllocator::IsEmpty()
{
	SHeapNode * pNode = m_oUsedNodes.GetFront();
	return !pNode->m_bUsed && pNode->m_nSize == m_nSize;

}
///------------------------------------------------------------------------
///------------------------------------------------------------------------

CHeapAllocator::SHeapNode::SHeapNode()
{
	m_pMemory = nullptr;
	m_nSize = nullptr;
	m_bUsed = false;
	m_pFile = nullptr;
	m_nLine = -1;
}