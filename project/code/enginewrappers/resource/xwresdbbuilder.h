#pragma once
#pragma unmanaged
#include "engine/core/xcore.h"
#include "engine/tools/resource/xresdbbuilder.h"
#pragma managed 


using namespace XEngine;

namespace XWEngine
{
	ref class XWSyncFile;

	namespace XWTools
	{
		///Very thin wrapper interface
		public interface class XWResourceDatabaseBuilderObject
		{
			XEngine::XTools::XResourceDatabaseBuilderObject * GetWrappedResDBObject();
		};

		public ref class XWResourceDatabaseBuilder
		{
		public:
			XWResourceDatabaseBuilder();
			void	AddObject( XWResourceDatabaseBuilderObject ^ rObject );
			void	XWFinalize( XWSyncFile ^ rFile, bool bWriteStringTable );
		private:
			XEngine::XTools::XResourceDatabaseBuilder * m_pWrappedResDBBuilder;
		};
	}
}