
#include "engine/core/xcore.h"
#include "engine/threading/xmutex.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void XEngine::XMutex::Init()
{
#if defined( TARGET_MSWIN )
	BOOL bResult	= ::InitializeCriticalSectionAndSpinCount(
		&m_oCriticalSection,
		1000 );

	DWORD dwError	= ::GetLastError();

	XASSERT_MSWIN( bResult, dwError );
#endif

#if defined( TARGET_POSIX )
	int nError;

	pthread_mutexattr_t oMutexAttr;
	nError = ::pthread_mutexattr_init( &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );

	/* TODO: Set mutex attributes */

	nError = ::pthread_mutex_init( &m_oMutex, &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );
	
	nError = ::pthread_mutexattr_destroy( &oMutexAttr );
	XASSERT_POSIX( nError == 0, nError );
#endif
}

///----------------------------------------------------------------------------

void XEngine::XMutex::Deinit()
{
#if defined( TARGET_MSWIN )
	::DeleteCriticalSection( &m_oCriticalSection );
#endif

#if defined( TARGET_POSIX )
	int nError = ::pthread_mutex_destroy( &m_oMutex );
	XASSERT_POSIX( nError == 0, nError );
#endif
}

///----------------------------------------------------------------------------

void XEngine::XMutex::Lock()
{
#if defined( TARGET_MSWIN )
	::EnterCriticalSection( &m_oCriticalSection );
#endif

#if defined( TARGET_POSIX )
	int nError = ::pthread_mutex_lock( &m_oMutex );
	XASSERT_POSIX( nError == 0, nError );
#endif
}

///----------------------------------------------------------------------------

void XEngine::XMutex::Unlock()
{
#if defined( TARGET_MSWIN )
	::LeaveCriticalSection( &m_oCriticalSection );
#endif

#if defined( TARGET_POSIX )
	int nError = ::pthread_mutex_unlock( &m_oMutex );
	XASSERT_POSIX( nError == 0, nError );
#endif
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
