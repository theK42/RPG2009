
#include "engine/core/xcore.h"
#include "engine/graphics/xgl.h"

extern "C" {

#define XGLEXTFN(type, name, params)	\
	ExtFn_##name ,

enum
{
#include "engine/graphics/xglext.h"
	ExtFnCount
};

#undef XGLEXTFN

static void const * gs_pFnPointers[ ExtFnCount ];

#define XGLEXTFN(type, name, params)	\
	__declspec( naked ) type __stdcall name params { __asm{ jmp gs_pFnPointers[ExtFn_##name * TYPE gs_pFnPointers] } }

#include "engine/graphics/xglext.h"

#undef XGLEXTFN

} /// extern "C"

#define XGLEXTFN(type, name, params)							\
	gs_pFnPointers[ExtFn_##name] = wglGetProcAddress(#name);	\
	//XASSERT_MSWIN( gs_pFnPointers[ExtFn_##name] != NULL, GetLastError() );

void XEngine::XGLExtInit()
{
#include "engine/graphics/xglext.h"
}

#undef XGLEXTFN

void XEngine::XGLExtDeinit()
{
}
