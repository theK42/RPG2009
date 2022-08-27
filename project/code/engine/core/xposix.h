
#pragma once

#if defined( TARGET_POSIX )

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	bool XAssertPosix( bool bCondition, char const * pFile, int nLine, char const * pExpression, int nError );

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

#	if defined( XENABLE_ASSERTS )
#		define XASSERT_POSIX(b, e)	do { if( XUNLIKELY( XEngine::XAssertPosix( (b), __FILE__, __LINE__, #b, (e) ) ) ) XBREAK; } while( false )
#	else
#		define XASSERT_POSIX(b, e)	do { XASSUME((b)); XASSUME((e) == 0); } while( false )
#	endif

#endif
