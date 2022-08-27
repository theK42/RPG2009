
#pragma once

#include "engine/core/xcore.h"
#include "engine/resource/xfileio.h"
#include "engine/threading/xmutex.h"
#include "engine/algorithms/xlist.h"

#include <cstring>

#if defined( TARGET_MSWIN )
#	include "engine/core/xmswin.h"
#endif

#if defined( TARGET_POSIX )
#	include "engine/core/xposix.h"
#	include <unistd.h>
#	include <aio.h>
#	include <pthread.h>
#endif

#if defined( TARGET_MSWIN )

struct AsyncTransfer
{
	HANDLE		m_hFile;
	OVERLAPPED	m_oOverlapped;
};

#endif

#if defined( TARGET_POSIX )

struct AsyncTransfer
{
	volatile bool	m_bComplete;
	aiocb			m_oControlBlock;
};

#endif

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

class AsyncTransferElement : public XEngine::XListElement< AsyncTransferElement, 1 >
{
public:
	AsyncTransfer const &	GetItem() const;
	AsyncTransfer &			GetItem();

private:
	AsyncTransfer	m_oItem;
};

///----------------------------------------------------------------------------

inline AsyncTransfer const & AsyncTransferElement::GetItem() const
{
	return m_oItem;
}

///----------------------------------------------------------------------------

inline AsyncTransfer & AsyncTransferElement::GetItem()
{
	return m_oItem;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

template< int N >
class AsyncTransfersPool
{
public:
	void					Init();
	void					Deinit();

	AsyncTransferElement *	Allocate();
	void					Free( AsyncTransferElement * pElement );

	AsyncTransferElement *	GetElements();

private:
	XEngine::XMutex								m_oMutex;
	XEngine::XList< AsyncTransferElement, 0 >	m_oFreeList;
	AsyncTransferElement						m_pElements[ N ];
};

///----------------------------------------------------------------------------

template< int N >
inline void AsyncTransfersPool< N >::Init()
{
	m_oFreeList.Reset();
	for( int i = 0; i < N; ++i )
	{
		m_oFreeList.PushBack( m_pElements + i );
	}
	m_oMutex.Init();
}

///----------------------------------------------------------------------------

template< int N >
inline void AsyncTransfersPool< N >::Deinit()
{
	m_oMutex.Deinit();
	m_oFreeList.Reset();
}

///----------------------------------------------------------------------------

template< int N >
inline AsyncTransferElement * AsyncTransfersPool< N >::Allocate()
{
	m_oMutex.Lock();
	AsyncTransferElement * pElement = m_oFreeList.PopFront();
	pElement = (pElement == m_oFreeList.GetEndMarker()) ? XEngine::nullptr : pElement;
	m_oMutex.Unlock();
	return pElement;
}

///----------------------------------------------------------------------------

template< int N >
inline void AsyncTransfersPool< N >::Free( AsyncTransferElement * pElement )
{
	m_oMutex.Lock();
	m_oFreeList.PushFront( pElement );
	m_oMutex.Unlock();
}

///----------------------------------------------------------------------------

template< int N >
inline AsyncTransferElement * AsyncTransfersPool< N >::GetElements()
{
	return m_pElements;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

static int const MAX_CONCURRENT_ASYNC_TRANSFERS	= 64;
static AsyncTransfersPool< MAX_CONCURRENT_ASYNC_TRANSFERS > gs_oTransfersPool;

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

#if defined( TARGET_POSIX )

static pthread_mutex_t	gs_oStreamMutex;
static pthread_cond_t	gs_oStreamConditionVar;

static void StreamAsyncTransferCallback( void * pParam )
{
	AsyncTransfer * pOp = reinterpret_cast< AsyncTransfer * >( pParam );

	int nError = pthread_mutex_lock( &gs_oStreamMutex );
	XASSERT_POSIX( nError == 0, nError );

	pOp->m_bComplete = true;

	nError = pthread_mutex_unlock( &gs_oStreamMutex );
	XASSERT_POSIX( nError == 0, nError );

	nError = pthread_cond_broadcast( &gs_oStreamConditionVar );
	XASSERT_POSIX( nError == 0, nError );
}

#endif

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XFileDescriptor::Close()
{
#if defined( TARGET_MSWIN )
	BOOL bResult	= ::CloseHandle( reinterpret_cast< HANDLE >( m_hStream ) );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );
#endif

#if defined( TARGET_POSIX )
	int nAsyncFileDescriptor	= static_cast<int>( reinterpret_cast<uaddr>( m_hStream ) );

	int nResult	= ::close( nAsyncFileDescriptor );
	int nError	= ::errno;

	XASSERT_POSIX( nResult == 0, nError );
#endif
}

///----------------------------------------------------------------------------

XEngine::u64 XEngine::XFileDescriptor::GetStreamSize() const
{
	using namespace XEngine;

	u64 nSize = 0;

#if defined( TARGET_MSWIN )
	HANDLE hFile = reinterpret_cast< HANDLE >( m_hStream );
	LARGE_INTEGER oSize;

	BOOL bResult	= ::GetFileSizeEx( hFile, &oSize );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	nSize = static_cast< u64 >( oSize.QuadPart );
#endif

#if defined( TARGET_POSIX )
	int filedes		= static_cast< int >( reinterpret_cast< uaddr >( m_hStream ) );
	off_t offset	= 0;
	int whence		= SEEK_CUR;

	off_t nPrevPos	= ::lseek( filedes, offset, whence );
	int nError		= ::errno;

	XASSERT_POSIX( (nPrevPos != (off_t)-1), nError );

	offset			= 0;
	whence			= SEEK_END;

	off_t nEndPos	= ::lseek( filedes, offset, whence );
	int nError		= ::errno;

	XASSERT_POSIX( (nEndPos != (off_t)-1), nError );

	offset			= nPrevPos;
	whence			= SEEK_SET;

	off_t nResult	= ::lseek( filedes, offset, whence );
	int nError		= ::errno;

	XASSERT_POSIX( (nResult != (off_t)-1), nError );

	XASSERT( nResult == nPrevPos );

	nSize = static_cast< u64 >( nEndPos );
#endif

	return nSize;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::handle XEngine::XAsyncFileDescriptorIO::BeginAsyncRead( u64 nOffset, uaddr nSize, void * pBuffer )
{
	AsyncTransferElement * pElement = gs_oTransfersPool.Allocate();
	XASSERT_PTR( pElement );

	AsyncTransfer & rTransfer = pElement->GetItem();

#if defined( TARGET_MSWIN )
	BOOL bResult	= ::ResetEvent( rTransfer.m_oOverlapped.hEvent );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	HANDLE hFile							= reinterpret_cast< HANDLE >( GetHandle() );
	rTransfer.m_hFile						= hFile;
	rTransfer.m_oOverlapped.Offset			= static_cast< DWORD >( nOffset );
	rTransfer.m_oOverlapped.OffsetHigh		= static_cast< DWORD >( nOffset >> 32ull );
	rTransfer.m_oOverlapped.Internal		= 0;
	rTransfer.m_oOverlapped.InternalHigh	= 0;

	bResult	= ::ReadFile(
		hFile,
		reinterpret_cast< LPVOID >( pBuffer ),
		static_cast< DWORD >( nSize ),
		NULL,
		&rTransfer.m_oOverlapped );

	dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult || (dwError == ERROR_IO_PENDING), dwError );
#endif

#if defined( TARGET_POSIX )
	rTransfer.m_bComplete						= false;
	rTransfer.m_oControlBlock.aio_filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	rTransfer.m_oControlBlock.aio_buf			= pBuffer;
	rTransfer.m_oControlBlock.aio_nbytes		= reinterpret_cast< size_t >( nSize );
	rTransfer.m_oControlBlock.aio_offset		= static_cast< off_t >( nOffset );
	rTransfer.m_oControlBlock.aio_reqprio		= 0;
	rTransfer.m_oControlBlock.aio_lio_opcode	= 0;
	rTransfer.m_oControlBlock.aio_flags			= 0;

	int nResult	= ::aio_read( &rTransferm_oControlBlock. );
	int nError	= ::errno;

	XASSERT_POSIX( nResult == 0, nError );
#endif

	return reinterpret_cast< handle >( pElement );
}

///----------------------------------------------------------------------------

XEngine::handle XEngine::XAsyncFileDescriptorIO::BeginAsyncWrite( u64 nOffset, uaddr nSize, void const * pBuffer )
{
	AsyncTransferElement * pElement = gs_oTransfersPool.Allocate();
	XASSERT_PTR( pElement );

	AsyncTransfer & rTransfer = pElement->GetItem();

#if defined( TARGET_MSWIN )
	BOOL bResult	= ::ResetEvent( rTransfer.m_oOverlapped.hEvent );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	HANDLE hFile							= reinterpret_cast< HANDLE >( GetHandle() );
	rTransfer.m_hFile						= hFile;
	rTransfer.m_oOverlapped.Offset			= static_cast< DWORD >( nOffset );
	rTransfer.m_oOverlapped.OffsetHigh		= static_cast< DWORD >( nOffset >> 32ull );
	rTransfer.m_oOverlapped.Internal		= 0;
	rTransfer.m_oOverlapped.InternalHigh	= 0;

	bResult	= ::WriteFile(
		hFile,
		reinterpret_cast< LPCVOID >( pBuffer ),
		static_cast< DWORD >( nSize ),
		NULL,
		&rTransfer.m_oOverlapped );

	dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult || (dwError == ERROR_IO_PENDING), dwError );
#endif

#if defined( TARGET_POSIX )
	rTransfer.m_bComplete						= false;
	rTransfer.m_oControlBlock.aio_filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	rTransfer.m_oControlBlock.aio_buf			= pBuffer;
	rTransfer.m_oControlBlock.aio_nbytes		= reinterpret_cast< size_t >( nSize );
	rTransfer.m_oControlBlock.aio_offset		= static_cast< off_t >( nOffset );
	rTransfer.m_oControlBlock.aio_reqprio		= 0;
	rTransfer.m_oControlBlock.aio_lio_opcode	= 0;
	rTransfer.m_oControlBlock.aio_flags			= 0;

	int nResult	= ::aio_write( &rTransfer.m_oControlBlock );
	int nError	= ::errno;

	XASSERT_POSIX( nResult == 0, nError );
#endif

	return reinterpret_cast< handle >( pElement );
}

///----------------------------------------------------------------------------

void XEngine::XAsyncFileDescriptorIO::CancelAsyncTransfer( handle hTransfer )
{
	AsyncTransferElement * pElement = reinterpret_cast< AsyncTransferElement * >( hTransfer );
	XASSERT_PTR( pElement );

	AsyncTransfer & rTransfer = pElement->GetItem();

#if 0 && defined( TARGET_MSWIN )
	/* TODO: Enable this for Windows Vista? */
	BOOL bResult	= ::CancelIoEx( rTransfer.m_hFile, &rTransfer.m_oOverlapped );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );
#endif

#if defined( TARGET_POSIX )
	int nResult	= ::aio_cancel( rTransfer.m_oControlBlock.aio_fileds, &rTransfer.m_oControlBlock );
	int nError	= ::errno;

	XASSERT_POSIX( nResult != -1, nError );
#endif

}

///----------------------------------------------------------------------------

bool XEngine::XAsyncFileDescriptorIO::IsAsyncTransferComplete( handle hTransfer ) const
{
	AsyncTransferElement * pElement = reinterpret_cast< AsyncTransferElement * >( hTransfer );
	XASSERT_PTR( pElement );

	AsyncTransfer & rTransfer = pElement->GetItem();

#if defined( TARGET_MSWIN )
	DWORD dwNumberOfBytesTransferred;

	BOOL bResult = ::GetOverlappedResult(
		rTransfer.m_hFile,
		&rTransfer.m_oOverlapped,
		&dwNumberOfBytesTransferred,
		FALSE );

	DWORD dwError = ::GetLastError();

	return (bResult != FALSE) || (dwError != ERROR_IO_INCOMPLETE);
#endif

#if defined( TARGET_POSIX )
	int nResult	= ::aio_error( &rTransfer.m_oControlBlock );
	int nError	= ::errno;

	XASSERT_POSIX( nResult != -1, nError );

	return (nResult != EINPROGRESS);
#endif
}

///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XAsyncFileDescriptorIO::FinishAsyncTransferInternal( handle hTransfer, bool bBlocking )
{
	AsyncTransferElement * pElement = reinterpret_cast< AsyncTransferElement * >( hTransfer );
	XASSERT_PTR( pElement );

	AsyncTransfer & rTransfer = pElement->GetItem();

	uaddr nTransferred = 0;

#if defined( TARGET_MSWIN )
	DWORD dwNumberOfBytesTransferred;

	BOOL bResult = ::GetOverlappedResult(
		rTransfer.m_hFile,
		&rTransfer.m_oOverlapped,
		&dwNumberOfBytesTransferred,
		bBlocking ? TRUE : FALSE );

	DWORD dwError = ::GetLastError();

	XASSERT_MSWIN( bResult || (dwError == ERROR_OPERATION_ABORTED), dwError );

	nTransferred = static_cast< saddr >( dwNumberOfBytesTransferred );
#endif

#if defined( TARGET_POSIX )
	if( bBlocking )
	{
		int nError = ::pthread_mutex_lock( &gs_oStreamMutex );
		XASSERT_POSIX( nError == 0, nError );

		while( !rTransfer.m_bComplete )
		{
			nError = ::pthread_cond_wait( &gs_oStreamConditionVar, &gs_oStreamMutex );
			XASSERT_POSIX( nError == 0, nError );
		}

		nError = ::pthread_mutex_unlock( &gs_oStreamMutex );
		XASSERT_POSIX( nError == 0, nError );
	}

	int nResult	= ::aio_error( &rTransfer.m_oControlBlock );
	nError		= ::errno;

	XASSERT_POSIX( nResult != -1, nError );
	XASSERT_POSIX( (nResult == 0) || (nResult == ECANCELED), nResult );

	ssize_t nTransferredBytes	= ::aio_return( &rTransfer.m_oControlBlock );
	nError						= ::errno;

	XASSERT_POSIX( nTransferredBytes >= 0, nError );

	nTransferred = static_cast< uaddr >( nTransferredBytes );
#endif

	gs_oTransfersPool.Free( pElement );

	return nTransferred;
}

///----------------------------------------------------------------------------

void XEngine::XAsyncFileDescriptorIO::InitModule()
{
	gs_oTransfersPool.Init();

	AsyncTransferElement * pElements = gs_oTransfersPool.GetElements();

	for( int i = 0; i < MAX_CONCURRENT_ASYNC_TRANSFERS; ++i )
	{
		AsyncTransfer & rTransfer				= pElements[i].GetItem();

#if defined( TARGET_MSWIN )
		HANDLE hEvent	= ::CreateEvent( NULL, TRUE, FALSE, NULL );
		DWORD dwError	= ::GetLastError();

		XASSERT_MSWIN( hEvent != NULL, dwError );

		ZeroMemory( &rTransfer, sizeof( AsyncTransfer ) );
		rTransfer.m_oOverlapped.hEvent	= hEvent;
#endif

#if defined( TARGET_POSIX )
		::bzero( &rTransfer, sizeof( AsyncTransfer ) );
		rTransfer.m_oControlBlock.aio_sigevent.sigev_notify				= SIGEV_CALLBACK;
		rTransfer.m_oControlBlock.aio_sigevent.sigev_notifyinfo.nifunc	= &StreamAsyncTransferCallback;
#endif
	}

#if defined( TARGET_POSIX )
	int nError;

	::pthread_mutexattr_t oMutexAttr;
	::pthread_condattr_t oCondAttr;

	nError = ::pthread_mutexattr_init( &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );

	/* TODO: Set mutex attributes */

	nError = ::pthread_mutex_init( &gs_oStreamMutex, &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );

	nError = ::pthread_mutexattr_destroy( &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );

	nError = ::pthread_condattr_init( &oCondAttr );
	XASSERT_POSIX( nError == 0, nError );

	nError = ::pthread_cond_init( &gs_oStreamConditionVar, &oCondAttr );
	XASSERT_POSIX( nError == 0, nError );
	
	nError = ::pthread_condattr_destroy( &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );
#endif
}

///----------------------------------------------------------------------------

void XEngine::XAsyncFileDescriptorIO::DeinitModule()
{
#if defined( TARGET_MSWIN )
	AsyncTransferElement * pElements = gs_oTransfersPool.GetElements();

	for( int i = 0; i < MAX_CONCURRENT_ASYNC_TRANSFERS; ++i )
	{
		BOOL bResult	= ::CloseHandle( pElements[i].GetItem().m_oOverlapped.hEvent );
		DWORD dwError	= ::GetLastError();

		XASSERT_MSWIN( bResult, dwError );
	}
#endif

#if defined( TARGET_POSIX )
	int nError;

	nError = ::pthread_mutex_destroy( &gs_oStreamMutex );
	XASSERT_POSIX( nError == 0, nError );

	nError = ::pthread_cond_destroy( &gs_oStreamConditionVar );
	XASSERT_POSIX( nError == 0, nError );
#endif

	gs_oTransfersPool.Deinit();
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XSyncFileDescriptorIO::Read( uaddr nSize, void * pBuffer )
{
	uaddr nTransferred = 0;

#if defined( TARGET_MSWIN )
	HANDLE hFile	= reinterpret_cast< HANDLE >( GetHandle() );
	LPVOID lpBuffer	= reinterpret_cast< LPVOID >( pBuffer );
	DWORD dwSize	= static_cast< DWORD >( nSize );

	DWORD dwNumberOfBytesTransferred;

	BOOL bResult = ::ReadFile(
		hFile,
		lpBuffer,
		dwSize,
		&dwNumberOfBytesTransferred,
		NULL );

	DWORD dwError = ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	nTransferred = static_cast< uaddr >( dwNumberOfBytesTransferred );
#endif

#if defined( TARGET_POSIX )
	int filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	size_t count	= static_cast< size_t >( nSize );

	ssize_t nTransferredBytes	= ::read( filedes, count, buf );
	int nError					= ::errno;

	XASSERT_POSIX( nTransferredBytes >= 0, nError );

	nTransferred = static_cast< uaddr >( nTransferredBytes );
#endif

	return nTransferred;
}

///----------------------------------------------------------------------------

XEngine::uaddr XEngine::XSyncFileDescriptorIO::Write( uaddr nSize, void const * pBuffer )
{
	uaddr nTransferred = 0;

#if defined( TARGET_MSWIN )
	HANDLE hFile		= reinterpret_cast< HANDLE >( GetHandle() );
	LPCVOID lpBuffer	= reinterpret_cast< LPCVOID >( pBuffer );
	DWORD dwSize		= static_cast< DWORD >( nSize );

	DWORD dwNumberOfBytesTransferred;

	BOOL bResult = ::WriteFile(
		hFile,
		lpBuffer,
		dwSize,
		&dwNumberOfBytesTransferred,
		NULL );

	DWORD dwError = ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	nTransferred = static_cast< uaddr >( dwNumberOfBytesTransferred );
#endif

#if defined( TARGET_POSIX )
	int filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	size_t count	= static_cast< size_t >( nSize );

	ssize_t nTransferredBytes	= ::write( filedes, count, buf );
	int nError					= ::errno;

	XASSERT_POSIX( nTransferredBytes >= 0, nError );

	nTransferred = static_cast< uaddr >( nTransferredBytes );
#endif

	return nTransferred;
}

///----------------------------------------------------------------------------

void XEngine::XSyncFileDescriptorIO::Seek( s64 nOffset, int nOrigin )
{
#if defined( TARGET_MSWIN )
	HANDLE hFile		= reinterpret_cast< HANDLE >( GetHandle() );
	DWORD dwMoveMethod	= 0;

	LARGE_INTEGER oDistance;
	oDistance.QuadPart = static_cast< LONGLONG >( nOffset );

	switch( nOrigin )
	{
	case XSeekOrigin::Begin:
		dwMoveMethod = FILE_BEGIN;
		break;

	case XSeekOrigin::Current:
		dwMoveMethod = FILE_CURRENT;
		break;

	case XSeekOrigin::End:
		dwMoveMethod = FILE_END;
		break;

	default:
		XASSERT(0);
	}

	BOOL bResult	= ::SetFilePointerEx( hFile, oDistance, NULL, dwMoveMethod );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );
#endif

#if defined( TARGET_POSIX )
	int filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	off_t offset	= static_cast< off_t >( nOffset );
	int whence		= 0;

	switch( nOrigin )
	{
	case XSeekOrigin::Begin:
		whence = SEEK_SET;
		break;

	case XSeekOrigin::Current:
		whence = SEEK_CUR;
		break;

	case XSeekOrigin::End:
		whence = SEEK_END;
		break;

	default:
		XASSERT(0);
	}

	off_t nResult	= ::lseek( filedes, offset, whence );
	int nError		= ::errno;

	XASSERT_POSIX( (nResult != (off_t)-1), nError );
#endif
}

///----------------------------------------------------------------------------

XEngine::u64 XEngine::XSyncFileDescriptorIO::Tell() const
{
	u64 nPosition = 0;

#if defined( TARGET_MSWIN )
	HANDLE hFile		= reinterpret_cast< HANDLE >( GetHandle() );
	DWORD dwMoveMethod	= FILE_CURRENT;

	LARGE_INTEGER oDistance;
	LARGE_INTEGER oFilePointer;

	oDistance.QuadPart = 0;

	BOOL bResult	= ::SetFilePointerEx( hFile, oDistance, &oFilePointer, dwMoveMethod );
	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );

	nPosition = static_cast< u64 >( oFilePointer.QuadPart );
#endif

#if defined( TARGET_POSIX )
	int filedes		= static_cast< int >( reinterpret_cast< uaddr >( GetHandle() ) );
	off_t offset	= 0;
	int whence		= SEEK_CUR;

	off_t nResult	= ::lseek( filedes, offset, whence );
	int nError		= ::errno;

	XASSERT_POSIX( (nResult != (off_t)-1), nError );

	nPosition = static_cast< u64 >( nResult );
#endif

	return nPosition;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XAsyncFile::Open( char const * pFilename, int nFlags, int nMode )
{
	bool bTruncate = (nFlags & XFileFlags::Truncate) != 0;

#if defined( TARGET_MSWIN )
	DWORD dwAccess				= 0;
	DWORD dwShareMode			= FILE_SHARE_READ;
	DWORD dwCreationDisposition	= 0;
	DWORD dwFlags				= FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;

	switch( nFlags & XFileFlags::ReadWriteMask )
	{
	case XFileFlags::Read:
		dwAccess				= GENERIC_READ;
		dwCreationDisposition	= bTruncate ? TRUNCATE_EXISTING : OPEN_EXISTING;
		break;

	case XFileFlags::Write:
		dwAccess				= GENERIC_WRITE;
		dwCreationDisposition	= bTruncate ? CREATE_ALWAYS : OPEN_ALWAYS;
		break;

	case XFileFlags::ReadWrite:
		dwAccess				= GENERIC_READ | GENERIC_WRITE;
		dwCreationDisposition	= bTruncate ? CREATE_ALWAYS : OPEN_ALWAYS;
		break;
	}

	HANDLE hFile = ::CreateFile(
		pFilename,
		dwAccess,
		dwShareMode,
		NULL,
		dwCreationDisposition,
		dwFlags | FILE_FLAG_OVERLAPPED,
		NULL );

	DWORD dwError = ::GetLastError();

	XASSERT_MSWIN( hFile != INVALID_HANDLE_VALUE, dwError );

	SetHandle( hFile );
#endif

#if defined( TARGET_POSIX )
	int nOpenFlags = bTruncate ? O_TRUNC : 0;

	switch( nFlags & XFileFlags::ReadWriteMask )
	{
	case XFileFlags::Read:
		nOpenFlags |= O_RDONLY;
		break;

	case XFileFlags::Write:
		nOpenFlags |= O_WRONLY | O_CREAT;
		break;

	case XFileFlags::ReadWrite:
		nOpenFlags |= O_RDWR | O_CREAT;
		break;
	}

	int nFileDescriptor	= ::open( pFilename, nOpenFlags, nMode );
	int nError			= ::errno;

	XASSERT_POSIX( nFileDescriptor >= 0, nError );

	SetStream( reinterpret_cast< handle >( static_cast< uaddr >( nFileDescriptor ) ) );
#endif
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XSyncFile::Open( char const * pFilename, int nFlags, int nMode )
{
	bool bTruncate = (nFlags & XFileFlags::Truncate) != 0;

#if defined( TARGET_MSWIN )
	DWORD dwAccess				= 0;
	DWORD dwShareMode			= FILE_SHARE_READ;
	DWORD dwCreationDisposition	= 0;
	DWORD dwFlags				= FILE_ATTRIBUTE_NORMAL;

	switch( nFlags & XFileFlags::ReadWriteMask )
	{
	case XFileFlags::Read:
		dwAccess				= GENERIC_READ;
		dwCreationDisposition	= bTruncate ? TRUNCATE_EXISTING : OPEN_EXISTING;
		break;

	case XFileFlags::Write:
		dwAccess				= GENERIC_WRITE;
		dwCreationDisposition	= bTruncate ? CREATE_ALWAYS : OPEN_ALWAYS;
		break;

	case XFileFlags::ReadWrite:
		dwAccess				= GENERIC_READ | GENERIC_WRITE;
		dwCreationDisposition	= bTruncate ? CREATE_ALWAYS : OPEN_ALWAYS;
		break;
	}

	HANDLE hFile = ::CreateFile(
		pFilename,
		dwAccess,
		dwShareMode,
		NULL,
		dwCreationDisposition,
		dwFlags,
		NULL );

	DWORD dwError = ::GetLastError();

	XASSERT_MSWIN( hFile != INVALID_HANDLE_VALUE, dwError );

	SetHandle( hFile );
#endif

#if defined( TARGET_POSIX )
	int nOpenFlags = bTruncate ? O_TRUNC : 0;

	switch( nFlags & XFileFlags::ReadWriteMask )
	{
	case XFileFlags::Read:
		nOpenFlags |= O_RDONLY;
		break;

	case XFileFlags::Write:
		nOpenFlags |= O_WRONLY | O_CREAT;
		break;

	case XFileFlags::ReadWrite:
		nOpenFlags |= O_RDWR | O_CREAT;
		break;
	}

	int nFileDescriptor	= ::open( pFilename, nOpenFlags, nMode );
	int nError			= ::errno;

	XASSERT_POSIX( nFileDescriptor >= 0, nError );

	SetStream( reinterpret_cast< handle >( static_cast< uaddr >( nFileDescriptor ) ) );
#endif
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
