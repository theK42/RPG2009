
#pragma once

#include "engine/resource/xio.h"

namespace XEngine
{
	class XAllocator;

	namespace XTools
	{

		class XMemIO : public XSeekIO
		{
		public:
			XMemIO();
			XMemIO( XMemIO const & rOther );

			~XMemIO();

			XMemIO &		operator =( XMemIO const & rOther );

			void			SetAlignment( uaddr nAlignment );

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
			struct Buffer;

			void	Reserve( uaddr nSize );

			static void		AddReference( Buffer * pBuffer );
			static void		RemoveReference( Buffer * pBuffer );
			static Buffer *	CreateBuffer( uaddr nCapacity, uaddr nAlignment );

		private:
			static uaddr const DEFAULT_BUFFER_ALIGNMENT	= 32;
			static uaddr const INITIAL_BUFFER_SIZE		= 1024;

			struct Buffer
			{
				uaddr			m_nCapacity;
				uaddr			m_nAlignment;
				uaddr			m_nSize;
				union
				{
					void *		m_pMemory;
					uaddr		m_nMemoryAddress;
				};
				unsigned		m_nReferenceCount;
			};

			Buffer *		m_pBuffer;
			uaddr			m_nPosition;
			uaddr			m_nAlignment;
		};

	} /// namespace XTools

} /// namepsace XEngine

inline XEngine::XTools::XMemIO::XMemIO()
{
	m_pBuffer		= nullptr;
	m_nPosition		= 0;
	m_nAlignment	= DEFAULT_BUFFER_ALIGNMENT;
}

inline void * XEngine::XTools::XMemIO::GetData()
{
	return (m_pBuffer == nullptr) ? nullptr : m_pBuffer->m_pMemory;
}

inline void const * XEngine::XTools::XMemIO::GetData() const
{
	return (m_pBuffer == nullptr) ? nullptr : m_pBuffer->m_pMemory;
}

inline XEngine::uaddr XEngine::XTools::XMemIO::GetDataSize() const
{
	return (m_pBuffer == nullptr) ? 0u : m_pBuffer->m_nSize;
}
