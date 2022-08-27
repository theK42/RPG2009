#include "stdafx.h"
#include "enginewrappers/graphics/xwgraphics.h"
#include "enginewrappers/graphics/xwcolor.h"
#pragma unmanaged
#include "engine/graphics/xcolor.h"
#pragma managed

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWGraphics::XWGraphics()
{
	m_pWrappedGraphics = new XGraphics;
}

///----------------------------------------------------------------------------

XWEngine::XWGraphics::~XWGraphics()
{
	delete m_pWrappedGraphics;
	m_pWrappedGraphics = nullptr;
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::Init()
{
	m_pWrappedGraphics->Init();
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::Deinit()
{
	m_pWrappedGraphics->Deinit();
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::PushRenderState()
{
	m_pWrappedGraphics->PushRenderState();
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::PopRenderState()
{
	m_pWrappedGraphics->PopRenderState();
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::SetClearColor( XWColor ^ rColor )
{
	XColor oColor(rColor->Packed);
	m_pWrappedGraphics->SetClearColor(oColor);
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::ClearScreen( XWEngine::XWScreenClearTarget nClearTargetMask )
{
	m_pWrappedGraphics->ClearScreen((int)nClearTargetMask);
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::SetColor( XWColor ^ rColor )
{
	XColor oColor(rColor->Packed);
	m_pWrappedGraphics->SetColor(oColor);
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::SetVertexAttributePointer(
								  XWVertexAttributeId eAttributeId,
								  XWVertexAttributeType eType,
								  int nNumComponents,
								  int nVertexStride,
								  int nVertexBufferBinding,
								  array<float> ^pBuffer )
{
	pin_ptr<float> pPinnedBuffer = &pBuffer[0];
	m_pWrappedGraphics->SetVertexAttributePointer((int)eAttributeId, (int)eType, nNumComponents, nVertexStride, nVertexBufferBinding, pPinnedBuffer);
}

///----------------------------------------------------------------------------

void XWEngine::XWGraphics::DrawElements(
					 XWIndexType eIndexType,
					 XWPrimitiveType ePrimitiveType,
					 int nNumIndices,
					 int nIndexBufferBinding,
					 array<u16>^ pBuffer )
{
	pin_ptr<u16> pPinnedBuffer = &pBuffer[0];
	m_pWrappedGraphics->DrawElements((int)eIndexType, (int)ePrimitiveType, nNumIndices, nIndexBufferBinding, pPinnedBuffer);
}

///----------------------------------------------------------------------------

XGraphics * XWEngine::XWGraphics::GetWrappedGraphics()
{
	return m_pWrappedGraphics;
}

///----------------------------------------------------------------------------