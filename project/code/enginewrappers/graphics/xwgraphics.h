#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/graphics/xgraphics.h"
#pragma managed 

#include "enginewrappers/graphics/xwvertexformat.h"
#include "enginewrappers/graphics/xwindexbuffer.h"


using namespace XEngine;

namespace XWEngine
{
	ref class XWColor;

	public enum class XWScreenClearTarget
	{
		Color	= XScreenClearTarget::Color,
		Depth	= XScreenClearTarget::Depth,
		Stencil	= XScreenClearTarget::Stencil,
		Mask	= XScreenClearTarget::Mask
	};

	public ref class XWGraphics
	{
	public:
		XWGraphics();
		~XWGraphics();
		void	Init();
		void	Deinit();

		void	PushRenderState();
		void	PopRenderState();

		void	SetClearColor( XWColor ^ rColor );
		void	ClearScreen( XWScreenClearTarget nClearTargetMask );

		void	SetColor( XWColor ^ rColor );

		///Probably shouldn't be exposed, assumes an array of float when the type is by no means fixed.
		void	SetVertexAttributePointer(
			XWVertexAttributeId eAttributeId,
			XWVertexAttributeType eType,
			int nNumComponents,
			int nVertexStride,
			int nVertexBufferBinding,
			array<float> ^ pBuffer );

		///Probably shouldn't be exposed, assumes an array of u16 when the type is by no means fixed.
		void	DrawElements(
			XWIndexType eIndexType,
			XWPrimitiveType ePrimitiveType,
			int nNumIndices,
			int nIndexBufferBinding,
			array<u16> ^ pBuffer );

		XGraphics *	GetWrappedGraphics();

	private:
		XGraphics *	m_pWrappedGraphics;
	};
}