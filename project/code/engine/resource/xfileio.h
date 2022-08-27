
#pragma once

#include "engine/resource/xio.h"

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XFileDescriptor
	{
	public:
		void	Close();

	protected:
		void	SetHandle( handle hStream );
		handle	GetHandle() const;

		u64		GetStreamSize() const;

	private:
		handle	m_hStream;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XAsyncFileDescriptorIO : public XFileDescriptor, public XAsyncIO
	{
	public:
		/// XAsyncIO interface
		virtual handle	BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer );
		virtual handle	BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer );

		virtual void	CancelAsyncTransfer( handle hTransfer );
		virtual bool	IsAsyncTransferComplete( handle hTransfer ) const;
		virtual uaddr	FinishAsyncTransfer( handle hTransfer );
		virtual uaddr	FinishAsyncTransferNonBlocking( handle hTransfer );

		virtual u64		GetEndOffset() const;

	public:
		static void	InitModule();
		static void	DeinitModule();

	private:
		static uaddr	FinishAsyncTransferInternal( handle hTransfer, bool bBlocking );
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	/// CAUTION: If you duplicate XSyncFileDescriptorIO (e.g. by using the
	/// copy constructor or assignment operator), all duplicate streams share
	/// a file position pointer.

	class XSyncFileDescriptorIO : public XFileDescriptor, public XSeekIO
	{
	public:
		/// XSyncIO interface
		virtual uaddr	Read( uaddr nSize, void * pBuffer );
		virtual uaddr	Write( uaddr nSize, void const * pBuffer );

		/// XSeekIO interface
		virtual void	Seek( s64 nOffset, int nOrigin );
		virtual u64		Tell() const;
		virtual u64		GetSize();
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XFileFlags
	{
		enum
		{
			Read			= 0x01,
			Write			= 0x02,
			ReadWrite		= Read | Write,
			ReadWriteMask	= Read | Write,
			Truncate		= 0x04
		};
	};

	///------------------------------------------------------------------------

	struct XFileAccessMode
	{
		enum
		{
			Read			= 04,
			Write			= 02,
			Execute			= 01,

			OwnerShift		= 6,
			GroupShift		= 3,
			AnonShift		= 0,

			OwnerRead		= Read << OwnerShift,
			OwnerWrite		= Write << OwnerShift,
			OwnerExecute	= Execute << OwnerShift,

			GroupRead		= Read << GroupShift,
			GroupWrite		= Write << GroupShift,
			GroupExecute	= Execute << GroupShift,

			AnonRead		= Read << AnonShift,
			AnonWrite		= Write << AnonShift,
			AnonExecute		= Execute << AnonShift,

			AllRead			= OwnerRead | GroupRead | AnonRead,
			AllWrite		= OwnerWrite | GroupWrite | AnonWrite,
			AllExecute		= OwnerExecute | GroupExecute | AnonExecute,

			Default			= AllRead | OwnerWrite
		};
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XAsyncFile : public XAsyncFileDescriptorIO
	{
	public:
		void	Open( char const * pFilename, int nFlags, int nMode = XFileAccessMode::Default );
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XSyncFile : public XSyncFileDescriptorIO
	{
	public:
		void	Open( char const * pFilename, int nFlags, int nMode = XFileAccessMode::Default );
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline void XEngine::XFileDescriptor::SetHandle( handle hStream )
{
	m_hStream = hStream;
}

///----------------------------------------------------------------------------

inline XEngine::handle XEngine::XFileDescriptor::GetHandle() const
{
	return m_hStream;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XAsyncFileDescriptorIO::FinishAsyncTransfer( handle hTransfer )
{
	return FinishAsyncTransferInternal( hTransfer, true );
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XAsyncFileDescriptorIO::FinishAsyncTransferNonBlocking( handle hTransfer )
{
	return FinishAsyncTransferInternal( hTransfer, false );
}

///----------------------------------------------------------------------------

inline XEngine::u64 XEngine::XAsyncFileDescriptorIO::GetEndOffset() const
{
	return GetStreamSize();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::u64 XEngine::XSyncFileDescriptorIO::GetSize()
{
	return GetStreamSize();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
