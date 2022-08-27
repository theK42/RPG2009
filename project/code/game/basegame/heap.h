#pragma once
#include "engine/core/xallocator.h"
#include "engine/algorithms/xlist.h"

#define KB(x) (size_t)(1024 * x)
#define MB(x) (size_t)(1024 * 1024 * x)

class CHeapAllocator : public XEngine::XAllocator
{
public:
	CHeapAllocator(XEngine::XAllocator * pParent, size_t nSize, int nNodes);
	virtual ~CHeapAllocator();

	virtual void *	Allocate(
		uaddr nSize,
		uaddr nAlignment,
		uaddr nAlignmentOffset,
		char const * pFile,
		int nLine );

	virtual void *	Reallocate(
		void * p,
		uaddr nNewSize,
		uaddr nNewAlignment,
		uaddr nAlignmentOffset,
		char const * pFile,
		int nLine );

	virtual void Free( void * p );
	void Dump();
	bool IsEmpty();
private:	
	
	struct SHeapNode : XEngine::XListElement<SHeapNode, 1>
	{
		SHeapNode();
		u8 *			m_pMemory;
		size_t			m_nAlignmentOverhead;
		size_t			m_nSize;
		bool			m_bUsed;
		char const *	m_pFile;
		int				m_nLine;
	};

	SHeapNode *	FindNode(void * pPointer);
	u8 *	m_pMemory;
	size_t	m_nSize;


	SHeapNode *						m_pHeapNodes;
	XEngine::XList<SHeapNode, 0>	m_oUsedNodes;
	XEngine::XList<SHeapNode, 0>	m_oFreeNodes;

};