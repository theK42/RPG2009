
#pragma once

#if defined( TARGET_MSWIN )
#	include "engine/core/xmswin.h"
#endif

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

#if defined( TARGET_IPHONEOS )
#	define TARGET_OPENGLES
#endif

#ifndef APIENTRY
#	define APIENTRY	/* nothing */
#endif

namespace XEngine
{

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

	bool XAssertGL( char const * pFile, int nLine );

	///------------------------------------------------------------------------

	void XGLExtInit();
	void XGLExtDeinit();

	///------------------------------------------------------------------------
	///------------------------------------------------------------------------

} /// namespace XEngine

#if defined( XENABLE_ASSERTS )
#	define XASSERT_GL()	do { if( XUNLIKELY( XEngine::XAssertGL( __FILE__, __LINE__ ) ) ) XBREAK; } while( false )
#else
#	define XASSERT_GL()	do { /* nothing */ } while( false )
#endif
