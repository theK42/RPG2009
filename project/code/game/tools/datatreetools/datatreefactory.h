#pragma once

//#include "enginewrappers/core/xwallocator.h"

using namespace XWEngine;

namespace DataTreeTools
{
	class CWriteableDataTree;
	
	public ref class CWriteableDataTreeFactory
	{
	public:
		CWriteableDataTree * CreateFromXML(System::IO::TextReader ^ rReader, XWAllocator ^ rAllocator);
		CWriteableDataTree * CreateFromCSV(System::IO::TextReader ^ rReader, XWAllocator ^ rAllocator);
		CWriteableDataTree * CreateFromXML(System::IO::TextReader ^ rReader, XEngine::XAllocator * pAllocator);
		CWriteableDataTree * CreateFromCSV(System::IO::TextReader ^ rReader, XEngine::XAllocator * pAllocator);
	private:
		CWriteableDataTree * CreateFromXMLNode(System::Xml::XmlNode ^ rNode, XEngine::XAllocator * pAllocator);
		XChecksum CalculateChecksumFromString(System::String ^ rString);
	};

}