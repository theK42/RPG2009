
#include "engine/core/xcore.h"
#include "engine/graphics/xgl.h"

#include <cstdio>

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

static inline char const * GetGLErrorString( GLenum nError )
{
	switch( nError )
	{
	case GL_NO_ERROR:
		return "NO_ERROR";
	case GL_INVALID_ENUM:
		return "INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "INGALID_OPERATION";
	case GL_STACK_OVERFLOW:
		return "STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:
		return "STACK_UNDERFLOW";
	case GL_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	default:
		return "(unknown code)";
	}
}

///----------------------------------------------------------------------------

bool XEngine::XAssertGL( char const * pFile, int nLine )
{
	static int const MAX_ERROR_STRING = 1024;
	static char pBuffer[ MAX_ERROR_STRING ];

	bool bBreak = false;

	int nSanity = 10;

	while( true )
	{
		GLenum nError = glGetError();

		if( nError == GL_NO_ERROR )
		{
			break;
		}

		std::sprintf( pBuffer, "OpenGL returned error code %s (0x%04X)", GetGLErrorString( nError ), nError );

		bBreak |= XEngine::XAssertInternal(
			false,
			pFile,
			nLine,
			"(OpenGL Error)",
			pBuffer );

		if( --nSanity == 0 )
		{
			bBreak |= XEngine::XAssertInternal(
				false,
				pFile,
				nLine,
				"(OpenGL Error)",
				"Too many errors, bailing out..." );
			break;
		}
	}

	return bBreak;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
