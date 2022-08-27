
#include "engine/core/xcore.h"
#include "engine/core/xassert.h"

#include <cstdio>
#include <cstdarg>

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

bool XEngine::XAssertInternal( bool bCondition, char const * pFile, int nLine, char const * pExpression, char const * pMessage, ... )
{
	if( bCondition )
	{
		return false;
	}

	std::printf(
		"Assertion failed!\n"
		"File:       %s\n"
		"Line:       %d\n"
		"Expression: %s\n",
		pFile, nLine, pExpression );

	if( pMessage != nullptr )
	{
		std::va_list pArgs;
		va_start( pArgs, pMessage );

		std::printf( "Message:    " );
		vprintf( pMessage, pArgs );
		std::printf( "\n" );

		va_end( pArgs );
	}

	return true;
}

///----------------------------------------------------------------------------

bool XEngine::XWarnInternal( bool bCondition, char const * pFile, int nLine, char const * pExpression, char const * pMessage, ... )
{
	if( bCondition )
	{
		return false;
	}

	std::printf(
		"Warning!\n"
		"File:       %s\n"
		"Line:       %d\n"
		"Expression: %s\n",
		pFile, nLine, pExpression );

	if( pMessage != nullptr )
	{
		std::va_list pArgs;
		va_start( pArgs, pMessage );

		std::printf( "Message:    " );
		vprintf( pMessage, pArgs );
		std::printf( "\n" );

		va_end( pArgs );
	}

	return false;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
