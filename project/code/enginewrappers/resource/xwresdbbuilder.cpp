#include "stdafx.h"
#include "enginewrappers/resource/xwresdbbuilder.h"
#include "enginewrappers/resource/xwfileio.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

XWEngine::XWTools::XWResourceDatabaseBuilder::XWResourceDatabaseBuilder()
{
	m_pWrappedResDBBuilder = new XEngine::XTools::XResourceDatabaseBuilder;
}

///----------------------------------------------------------------------------

void XWEngine::XWTools::XWResourceDatabaseBuilder::AddObject( XWResourceDatabaseBuilderObject ^ rObject )
{
	m_pWrappedResDBBuilder->AddObject(rObject->GetWrappedResDBObject());
}

///----------------------------------------------------------------------------

void XWEngine::XWTools::XWResourceDatabaseBuilder::XWFinalize( XWSyncFile ^ rFile, bool bWriteStringTable )
{
	m_pWrappedResDBBuilder->Finalize(*rFile->GetWrappedSyncFile(), bWriteStringTable);
}

///----------------------------------------------------------------------------