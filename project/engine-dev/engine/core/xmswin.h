
#pragma once

#if defined( TARGET_MSWIN )

#	define _WIN32_WINNT	0x0403
#	define WIN32_LEAN_AND_MEAN
#	define STRICT
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#	undef STRICT

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	bool XAssertMSWin( BOOL bCondition, char const * pFile, int nLine, char const * pExpression, DWORD dwError );

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

#	if defined( XENABLE_ASSERTS )
#		define XASSERT_MSWIN(b, e)	do { if( XUNLIKELY( XEngine::XAssertMSWin( (b), __FILE__, __LINE__, #b, (e) ) ) ) XBREAK; } while( false )
#	else
#		define XASSERT_MSWIN(b, e)	do { XASSUME((b)); XASSUME((e) == ERROR_SUCCESS); } while( false )
#	endif


#endif
