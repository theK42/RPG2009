
#pragma once

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	/// XAsyncIO facilitates asynchronous I/O. To start transferring data to or
	/// from an open stream, call BeginAsyncWrite() or BeginAsyncRead(),
	/// respectively. These methods return a handle to the async transfer
	/// operation that you requested. You can query the completion status of an
	/// async transfer operation by its handle with IsAsyncTransferComplete().
	/// To complete the transfer and free the handle, call
	/// FinishAsyncTransfer(). You must call FinishAsyncTransfer() to free the
	/// handle, even if IsAsyncTransferComplete() has returned 'true'.
	/// FinishAsyncTransfer() will block until the async transfer operation is
	/// complete if necessary. The reults of calling IsAsyncTransferComplete()
	/// on a handle that has been freed by FinishAsyncTransfer() are undefined.
	/// FinishAsyncTransferNonBlocking() is similar to FinisAsyncTransfer(),
	/// but it should only be called if the transfer is complete, and may cause
	/// an assertion failure otherwise.

	/// XAsyncIO supports cancelling in-flight async transfer operations on
	/// streams that provide that capability. CancelAsyncTransfer() itself
	/// operates asynchronously, and may not cancel an async transfer operation
	/// immediately. After calling CancelAsyncTransfer() you may still call
	/// IsAsyncTransferComplete() to determine when the async transfer
	/// operation has been successfully canceled (or, perhaps, completed), and
	/// you must still call FinishAsyncTransfer() to free the handle. The
	/// results of calling CancelAsyncTransfer() on a handle that has been
	/// freed by FinishAsyncTransfer() are undefined.

	/// Prospective features (I'll implement these if requested):
	/// . Callbacks for async transfer operation completion

	class XAsyncIO
	{
	public:
		virtual ~XAsyncIO();

		/// BeginAsync(Read|Write) are optional. The default implementations
		/// assert with "NOT IMPLEMENTED"
		virtual handle	BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer );
		virtual handle	BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer );

		/// CancelAsyncTransfer is optional. The default implementation does
		/// nothing.
		virtual void	CancelAsyncTransfer( handle hTransfer );

		virtual bool	IsAsyncTransferComplete( handle hTransfer ) const = 0;
		virtual uaddr	FinishAsyncTransfer( handle hTransfer ) = 0;
		virtual uaddr	FinishAsyncTransferNonBlocking( handle hTransfer ) = 0;

		virtual u64		GetEndOffset() const = 0;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XSyncIO
	{
	public:
		virtual ~XSyncIO();

		/// Read and Write are optional. The default implementations assert
		/// with "NOT IMPLEMENTED"
		virtual uaddr	Read( uaddr nSize, void * pBuffer );
		virtual uaddr	Write( uaddr nSize, void const * pBuffer );

		virtual u64		Tell() const = 0;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	struct XSeekOrigin
	{
		enum
		{
			Begin,
			Current,
			End,
		};
	};

	///------------------------------------------------------------------------

	class XSeekIO : public XSyncIO
	{
	public:
		virtual void	Seek( s64 nOffset, int nOrigin ) = 0;

		/// GetSize is optional. The default implementation uses Seek and Tell
		/// to calculate the size.
		virtual u64		GetSize();
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XSeekIOWrapper : public XSeekIO
	{
	public:
		XSeekIOWrapper( XAsyncIO & rAsyncIO );

		virtual uaddr	Read( uaddr nSize, void * pBuffer );
		virtual uaddr	Write( uaddr nSize, void const * pBuffer );

		virtual void	Seek( s64 nOffset, int nOrigin );
		virtual u64		Tell() const;
		virtual u64		GetSize();

	private:
		XAsyncIO *	m_pAsyncIO;
		u64			m_nPosition;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	class XAsyncIOWrapper : public XAsyncIO
	{
	public:
		XAsyncIOWrapper( XSeekIO & rSyncIO );

		virtual handle	BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer );
		virtual handle	BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer );
		virtual bool	IsAsyncTransferComplete( handle hTransfer ) const;
		virtual uaddr	FinishAsyncTransfer( handle hTransfer );
		virtual uaddr	FinishAsyncTransferNonBlocking( handle hTransfer );

		virtual u64		GetEndOffset() const;

	private:
		XSeekIO *	m_pSyncIO;
	};

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XAsyncIO::~XAsyncIO()
{
}

///----------------------------------------------------------------------------

inline XEngine::handle XEngine::XAsyncIO::BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer )
{
	XASSERT_MSG( 0, "Not Implemented" );
	return nullptr;
}

///----------------------------------------------------------------------------

inline XEngine::handle XEngine::XAsyncIO::BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer )
{
	XASSERT_MSG( 0, "Not Implemented" );
	return nullptr;
}

///----------------------------------------------------------------------------

inline void XEngine::XAsyncIO::CancelAsyncTransfer( handle hTransfer )
{
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XSyncIO::~XSyncIO()
{
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XSyncIO::Read( uaddr nSize, void * pBuffer )
{
	XASSERT_MSG( 0, "Not Implemented" );
	return 0;
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XSyncIO::Write( uaddr nSize, void const * pBuffer )
{
	XASSERT_MSG( 0, "Not Implemented" );
	return 0;
}

///----------------------------------------------------------------------------

inline XEngine::u64 XEngine::XSeekIO::GetSize()
{
	u64 nSavedPosition = Tell();
	Seek( 0, XSeekOrigin::End );
	u64 nSize = Tell();
	Seek( nSavedPosition, XSeekOrigin::Begin );
	return nSize;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XSeekIOWrapper::XSeekIOWrapper( XAsyncIO & rAsyncIO )
: m_pAsyncIO( &rAsyncIO ), m_nPosition( 0 )
{
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XSeekIOWrapper::Read( uaddr nSize, void * pBuffer )
{
	handle hTransfer = m_pAsyncIO->BeginAsyncRead( m_nPosition, nSize, pBuffer );
	m_nPosition += nSize;
	return m_pAsyncIO->FinishAsyncTransfer( hTransfer );
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XSeekIOWrapper::Write( uaddr nSize, void const * pBuffer )
{
	handle hTransfer = m_pAsyncIO->BeginAsyncWrite( m_nPosition, nSize, pBuffer );
	m_nPosition += nSize;
	return m_pAsyncIO->FinishAsyncTransfer( hTransfer );
}

///----------------------------------------------------------------------------

inline void XEngine::XSeekIOWrapper::Seek( s64 nOffset, int nOrigin )
{
	switch( nOrigin )
	{
	case XSeekOrigin::Begin:
		XASSERT( nOffset >= 0 );
		m_nPosition = nOffset;
		break;

	case XSeekOrigin::End:
		m_nPosition = m_pAsyncIO->GetEndOffset();
		/// Fall through...

	case XSeekOrigin::Current:
		XASSERT( ((nOffset >= 0) && (m_nPosition + nOffset >= m_nPosition)) || ((nOffset < 0) && (m_nPosition + nOffset < m_nPosition)) );
		m_nPosition += nOffset;
		break;

	default:
		XASSERT(0);
	}
}

///----------------------------------------------------------------------------

inline XEngine::u64 XEngine::XSeekIOWrapper::GetSize()
{
	return m_pAsyncIO->GetEndOffset();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

inline XEngine::XAsyncIOWrapper::XAsyncIOWrapper( XSeekIO & rSyncIO )
: m_pSyncIO( &rSyncIO )
{
}

///----------------------------------------------------------------------------

inline XEngine::handle XEngine::XAsyncIOWrapper::BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer )
{
	m_pSyncIO->Seek( nOffset, XSeekOrigin::Begin );
	return (handle)(uaddr) m_pSyncIO->Read( nSize, pBuffer );
}

///----------------------------------------------------------------------------

inline XEngine::handle XEngine::XAsyncIOWrapper::BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer )
{
	m_pSyncIO->Seek( nOffset, XSeekOrigin::Begin );
	return (handle) m_pSyncIO->Write( nSize, pBuffer );
}

///----------------------------------------------------------------------------

inline bool XEngine::XAsyncIOWrapper::IsAsyncTransferComplete( handle hTransfer ) const
{
	return true;
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XAsyncIOWrapper::FinishAsyncTransfer( handle hTransfer )
{
	return (uaddr) hTransfer;
}

///----------------------------------------------------------------------------

inline XEngine::uaddr XEngine::XAsyncIOWrapper::FinishAsyncTransferNonBlocking( handle hTransfer )
{
	return (uaddr) hTransfer;
}

///----------------------------------------------------------------------------

inline XEngine::u64 XEngine::XAsyncIOWrapper::GetEndOffset() const
{
	return m_pSyncIO->GetSize();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
