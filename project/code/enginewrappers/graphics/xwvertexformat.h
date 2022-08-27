#pragma once
#pragma unmanaged
#include "engine/graphics/xvertexformat.h"
#pragma managed

using namespace XEngine;

namespace XWEngine
{
	public enum class XWVertexAttributeId
	{
		Position = XVertexAttributeId::Position,
		Normal = XVertexAttributeId::Normal,
		Color = XVertexAttributeId::Color,
		TexCoord0 = XVertexAttributeId::TexCoord0,
		TexCoord1 = XVertexAttributeId::TexCoord1,
		Weights = XVertexAttributeId::Weights,
		MatrixIndices = XVertexAttributeId::MatrixIndices,
		Max = XVertexAttributeId::Max
	};

	///------------------------------------------------------------------------

	public enum class XWVertexAttributeMask
	{
		Position		= XVertexAttributeMask::Position,
		Normal			= XVertexAttributeMask::Normal,
		Color			= XVertexAttributeMask::Color,
		TexCoord0		= XVertexAttributeMask::TexCoord0,
		TexCoord1		= XVertexAttributeMask::TexCoord1,
		Weights			= XVertexAttributeMask::Weights,
		MatrixIndices	= XVertexAttributeMask::MatrixIndices
	};

	///------------------------------------------------------------------------

	public enum class XWVertexAttributeType
	{
		S8 = XVertexAttributeType::S8,
		U8 = XVertexAttributeType::U8,
		S16 = XVertexAttributeType::S16,
		U16 = XVertexAttributeType::U16,
		S32 = XVertexAttributeType::S32,
		U32 = XVertexAttributeType::U32,
		F32 = XVertexAttributeType::F32,
		Max = XVertexAttributeType::Max
	};
}