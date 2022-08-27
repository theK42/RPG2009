
#pragma once

#include "engine/resource/xio.h"

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XFixedMemIO : public XSeekIO
	{
	public:
		void	Init( uaddr nSize, void * pMemory );
		void	Deinit();

		void *			GetData();
		void const *	GetData() const;
		uaddr			GetDataSize() const;

		/// XSyncIO interface
		virtual uaddr	Read( uaddr nSize, void * pBuffer );
		virtual uaddr	Write( uaddr nSize, void const * pBuffer );

		/// XSeekIO interface
		virtual void	Seek( s64 nOffset, int nOrigin );
		virtual u64		Tell() const;
		virtual u64		GetSize();

	private:
		union
		{
			void *	m_pMemory;
			uaddr	m_nMemoryAddress;
		};
		uaddr	m_nSize;
		uaddr	m_nPosition;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline void XEngine::XFixedMemIO::Init( uaddr nSize, void * pMemory )
{
	m_pMemory	= pMemory;
	m_nSize		= nSize;
	m_nPosition	= 0;
}

///----------------------------------------------------------------------------

inline void XEngine::XFixedMemIO::Deinit()
{
}

///----------------------------------------------------------------------------

inline void * XEngine::XFixedMemIO::GetData()
{
	return m_pMemory;
}

///----------------------------------------------------------------------------

inline void const * XEngine::XFixedMemIO::GetData() const
{
	return m_pMemory;
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XFixedMemIO::GetDataSize() const
{
	return m_nSize;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
