#pragma once

#include "engine/graphics/xindexbuffer.h"

using namespace XEngine;

namespace XWEngine
{
	public enum class XWIndexType
	{
		U8 = XIndexType::U8,
		U16 = XIndexType::U16,
		Max = XIndexType::Max
	};

	public enum class XWPrimitiveType
	{
		TriangleList = XPrimitiveType::TriangleList,
		TriangleStrip = XPrimitiveType::TriangleStrip,
		TriangleFan = XPrimitiveType::TriangleFan,
		Max = XPrimitiveType::Max
	};
}