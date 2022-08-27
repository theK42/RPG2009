
#include "engine/core/xcore.h"

#if defined(TARGET_MSWIN)

#	include "engine/core/xmswin.h"

void XEngine::XThreadLocalVariable::Alloc()
{
	XASSERT( m_nTlsIndex == TLS_OUT_OF_INDEXES );
	m_nTlsIndex = TlsAlloc();
	DWORD dwError = GetLastError();
	XASSERT_MSWIN( (m_nTlsIndex != TLS_OUT_OF_INDEXES), dwError );
}

void XEngine::XThreadLocalVariable::Free()
{
	XASSERT( m_nTlsIndex != TLS_OUT_OF_INDEXES );
	BOOL bResult = TlsFree( m_nTlsIndex );
	DWORD dwError = GetLastError();
	XASSERT_MSWIN( (bResult != FALSE), dwError );
	m_nTlsIndex = TLS_OUT_OF_INDEXES;
}

void * XEngine::XThreadLocalVariable::Get() const
{
	XASSERT( m_nTlsIndex != TLS_OUT_OF_INDEXES );
	LPVOID lpValue = TlsGetValue( m_nTlsIndex );
	DWORD dwError = GetLastError();
	XASSERT_MSWIN( (lpValue != 0) || (dwError == ERROR_SUCCESS), dwError );
	return reinterpret_cast<void *>( lpValue );
}

void XEngine::XThreadLocalVariable::Set( void * pValue )
{
	XASSERT( m_nTlsIndex != TLS_OUT_OF_INDEXES );
	BOOL bResult = TlsSetValue( m_nTlsIndex, reinterpret_cast<LPVOID>( pValue ) );
	DWORD dwError = GetLastError();
	XASSERT_MSWIN( (bResult != FALSE), dwError );
}

#endif