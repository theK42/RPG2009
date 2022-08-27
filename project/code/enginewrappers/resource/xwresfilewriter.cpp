#include "stdafx.h"
#include "enginewrappers/resource/xwresfilewriter.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWTools::XWResourceFileWriter::XWResourceFileWriter()
{
	m_pWrappedResFileWriter = new XEngine::XTools::XResourceFileWriter;
}

///----------------------------------------------------------------------------

XEngine::XTools::XResourceFileWriter * XWEngine::XWTools::XWResourceFileWriter::GetWrappedResFileWriter()
{
	return m_pWrappedResFileWriter;
}

///----------------------------------------------------------------------------