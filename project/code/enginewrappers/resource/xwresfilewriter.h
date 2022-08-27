#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/tools/resource/xresfilewriter.h"
#pragma managed 

using namespace XEngine;

namespace XWEngine
{
	namespace XWTools
	{
		public ref class XWResourceFileWriter
		{
		public:
			XWResourceFileWriter();
			XEngine::XTools::XResourceFileWriter * GetWrappedResFileWriter();
		private:
			XEngine::XTools::XResourceFileWriter *	m_pWrappedResFileWriter;
		};
	}
}