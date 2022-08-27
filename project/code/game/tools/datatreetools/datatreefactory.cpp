#include "game/shared/gamepch.h"
#include "game/tools/datatreetools/datatreefactory.h"
#include "game/tools/datatreetools/datatreeresourcewriter.h"
#include "game/tools/datatreetools/csvtable.h"


///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeFactory::CreateFromXML( System::IO::TextReader ^ rReader, XEngine::XAllocator * pAllocator ) 
{
	System::Xml::XmlDocument ^ rXMLDocument = gcnew System::Xml::XmlDocument();
	try
	{
		rXMLDocument->Load(rReader);
	}
	catch (System::Xml::XmlException ^ rException)
	{
		XASSERT(false);
	}
	System::Xml::XmlNode ^ rRootTreeNode = rXMLDocument["datatree"];
	XASSERT(rRootTreeNode != nullptr);
	return CreateFromXMLNode(rRootTreeNode, pAllocator);
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeFactory::CreateFromXMLNode( System::Xml::XmlNode ^ rNode, XEngine::XAllocator * pAllocator ) 
{
	///Collect information needed for construction
	System::Xml::XmlNodeList ^ rIntList = rNode->SelectNodes("child::int");
	System::Xml::XmlNodeList ^ rFloatList = rNode->SelectNodes("child::float");
	System::Xml::XmlNodeList ^ rBoolList = rNode->SelectNodes("child::bool");
	System::Xml::XmlNodeList ^ rStringList = rNode->SelectNodes("child::string");
	System::Xml::XmlNodeList ^ rChecksumList = rNode->SelectNodes("child::checksum");
	System::Xml::XmlNodeList ^ rKeyList = rNode->SelectNodes("child::key");
	System::Xml::XmlNodeList ^ rDataTreeList = rNode->SelectNodes("child::datatree");
	CWriteableDataTreeHeader * pHeader = XNEW(pAllocator) CWriteableDataTreeHeader();
	pHeader->SetupInts(rIntList->Count, pAllocator);
	pHeader->SetupFloats(rFloatList->Count, pAllocator);
	pHeader->SetupBools(rBoolList->Count, pAllocator);
	pHeader->SetupStrings(rStringList->Count, pAllocator);
	pHeader->SetupChecksums(rChecksumList->Count, pAllocator);
	pHeader->SetupTrees(rDataTreeList->Count, rKeyList->Count, pAllocator);
	CWriteableStringTable * pStringTable = nullptr;
	if (rStringList->Count != 0)
	{
		int nSize = 0;
		for (System::Collections::IEnumerator ^ rEnumerator = rStringList->GetEnumerator(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
		{
			System::Xml::XmlElement ^ rStringElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
			System::Xml::XmlAttribute ^ rStringValueAttribute = rStringElement->GetAttributeNode("value");
			XASSERT(rStringValueAttribute != nullptr);
			nSize += rStringValueAttribute->Value->Length;
		}
		pStringTable = XNEW(pAllocator) CWriteableStringTable(nSize, rStringList->Count, pAllocator);
	}

	///Construct DataTree object
	CWriteableDataTree * pDataTree = XNEW(pAllocator) CWriteableDataTree(pHeader, pAllocator, true, pStringTable);

	System::Collections::IEnumerator ^ rEnumerator = nullptr;

	///Collect and add the data
	///ints
	for (rEnumerator = rIntList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rIntElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rIntIdAttribute = rIntElement->GetAttributeNode("name");
		XASSERT(rIntIdAttribute != nullptr);
		System::Xml::XmlAttribute ^ rIntValueAttribute = rIntElement->GetAttributeNode("value");
		XASSERT(rIntValueAttribute != nullptr);
		
		pDataTree->SetInt(CalculateChecksumFromString(rIntValueAttribute->Value), System::Convert::ToInt32(rIntValueAttribute->Value));
	}
	///floats
	for (rEnumerator = rFloatList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rFloatElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rFloatIdAttribute = rFloatElement->GetAttributeNode("name");
		XASSERT(rFloatIdAttribute != nullptr);
		System::Xml::XmlAttribute ^ rFloatValueAttribute = rFloatElement->GetAttributeNode("value");
		XASSERT(rFloatValueAttribute != nullptr);
		pDataTree->SetFloat(CalculateChecksumFromString(rFloatIdAttribute->Value), System::Convert::ToSingle(rFloatValueAttribute->Value));
	}
	///bools
	for (rEnumerator = rBoolList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rBoolElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rBoolIdAttribute = rBoolElement->GetAttributeNode("name");
		XASSERT(rBoolIdAttribute != nullptr);
		System::Xml::XmlAttribute ^ rBoolValueAttribute = rBoolElement->GetAttributeNode("value");
		XASSERT(rBoolValueAttribute != nullptr);
		pDataTree->SetBool(CalculateChecksumFromString(rBoolIdAttribute->Value), System::Convert::ToBoolean(rBoolValueAttribute->Value));
	}
	///strings
	for (rEnumerator = rStringList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rStringElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rStringIdAttribute = rStringElement->GetAttributeNode("name");
		XASSERT(rStringIdAttribute != nullptr);
		System::Xml::XmlAttribute ^ rStringValueAttribute = rStringElement->GetAttributeNode("value");
		XASSERT(rStringValueAttribute != nullptr);
		char * pCString = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(rStringValueAttribute->Value).ToPointer();
		pDataTree->AddString(CalculateChecksumFromString(rStringIdAttribute->Value), pCString);
		System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)pCString);
	}
	///cheksums
	for (rEnumerator = rChecksumList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rChecksumElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rChecksumIdAttribute = rChecksumElement->GetAttributeNode("name");
		XASSERT(rChecksumIdAttribute != nullptr);
		System::Xml::XmlAttribute ^ rChecksumValueAttribute = rChecksumElement->GetAttributeNode("value");
		XASSERT(rChecksumValueAttribute != nullptr);
		pDataTree->AddChecksum(CalculateChecksumFromString(rChecksumIdAttribute->Value), CalculateChecksumFromString(rChecksumValueAttribute->Value));
	}
	///keys
	for (rEnumerator = rKeyList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rKeyElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		System::Xml::XmlAttribute ^ rKeyIdAttribute = rKeyElement->GetAttributeNode("name");
		XASSERT(rKeyIdAttribute != nullptr);
		pDataTree->AddKey(CalculateChecksumFromString(rKeyIdAttribute->Value));
	}
	///datatrees (recursive! what fun!)
	for (rEnumerator = rDataTreeList->GetEnumerator(), rEnumerator->MoveNext(); rEnumerator->Current != nullptr; rEnumerator->MoveNext())
	{
		System::Xml::XmlElement ^ rDataTreeElement = dynamic_cast<System::Xml::XmlElement ^>(rEnumerator->Current);
		pDataTree->AddDataTree(CreateFromXMLNode(rDataTreeElement, pAllocator));
	}
	return pDataTree;
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeFactory::CreateFromXML( System::IO::TextReader ^ rReader, XWAllocator ^ rAllocator )
{
	return CreateFromXML(rReader, rAllocator->GetAllocator());
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeFactory::CreateFromCSV( System::IO::TextReader ^ rReader, XWAllocator ^ rAllocator )
{
	return CreateFromCSV(rReader, rAllocator->GetAllocator());
}

///----------------------------------------------------------------------------

XChecksum DataTreeTools::CWriteableDataTreeFactory::CalculateChecksumFromString( System::String ^ rString ) 
{
	char * pCString = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(rString).ToPointer();
	XChecksum nChecksum = XEngine::XChecksum_CalculateFromString(pCString);
	System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)pCString);
	return nChecksum;
}

///----------------------------------------------------------------------------

DataTreeTools::CWriteableDataTree * DataTreeTools::CWriteableDataTreeFactory::CreateFromCSV( System::IO::TextReader ^ rReader, XEngine::XAllocator * pAllocator )
{
	static const int sc_nTypeRow = 0;
	static const int sc_nNameRow = 1;
	static const int sc_nFirstDataRow = 2;
	///Collect information needed for construction of root
	CCSVTable ^ rTable = gcnew CCSVTable();
	rTable->Load(rReader);
	int nNumBranches = rTable->GetNumRows() - sc_nFirstDataRow;
	XASSERT(nNumBranches > 0);
	int nNumKeys = 0;
	for (int i = 0; i < rTable->GetNumColumns(); i++)
	{
		if (rTable->GetCell(sc_nTypeRow, i)->Equals("key"))
		{
			nNumKeys++;
		}
	}

	///Create header for root
	CWriteableDataTreeHeader * pRootHeader = XNEW(pAllocator) CWriteableDataTreeHeader();
	pRootHeader->SetupTrees(nNumBranches, nNumKeys, pAllocator);
	
	///Create root
	CWriteableDataTree * pRoot = XNEW(pAllocator) CWriteableDataTree(pRootHeader, pAllocator, true);
	
	///Add Keys
	for (int i = 0; i < rTable->GetNumColumns(); i++)
	{
		if (rTable->GetCell(sc_nTypeRow, i)->Equals("key"))
		{
			pRoot->AddKey(CalculateChecksumFromString(rTable->GetCell(sc_nNameRow, i)));
		}
	}
	
	///Collect data for branch header
	int nNumInts = 0;
	int nNumFloats = 0;
	int nNumBools = 0;
	int nNumStrings = 0;
	int nTotalStringLength = 0;
	int nNumChecksums = 0;
	for (int i = 0; i < rTable->GetNumColumns(); i++)
	{
		System::String ^ rColumnType = rTable->GetCell(sc_nTypeRow, i);
		if (rColumnType->Equals("int"))
		{
			nNumInts++;
		}
		else if (rColumnType->Equals("float"))
		{
			nNumFloats++;
		}
		else if (rColumnType->Equals("bool"))
		{
			nNumBools++;
		}
		else if (rColumnType->Equals("string"))
		{
			nNumStrings++;
			for (int j = sc_nFirstDataRow; j < rTable->GetNumRows(); j++)
			{
				nTotalStringLength += rTable->GetCell(j, i)->Length;
			}
		}
		else if (rColumnType->Equals("checksum") || rColumnType->Equals("key"))
		{
			nNumChecksums++;
		}
	}

	///Construct shared branch header
	CWriteableDataTreeHeader * pBranchHeader = XNEW(pAllocator) CWriteableDataTreeHeader();
	pBranchHeader->SetupInts(nNumInts, pAllocator);
	pBranchHeader->SetupFloats(nNumFloats, pAllocator);
	pBranchHeader->SetupBools(nNumBools, pAllocator);
	pBranchHeader->SetupStrings(nNumStrings, pAllocator);
	pBranchHeader->SetupChecksums(nNumChecksums, pAllocator);
	
	///Construct shared string table
	CWriteableStringTable * pSharedStringTable = XNEW(pAllocator) CWriteableStringTable(nTotalStringLength, nNumStrings, pAllocator);

	///Construct and fill in branches
	for (int i = sc_nFirstDataRow; i < rTable->GetNumRows(); i++)
	{
		CWriteableDataTree * pBranch = XNEW(pAllocator) CWriteableDataTree(pBranchHeader, pAllocator, i == sc_nFirstDataRow, pSharedStringTable);
		for ( int j = 0; j < rTable->GetNumColumns(); j++)
		{
			System::String ^ rColumnType = rTable->GetCell(sc_nTypeRow, j);
			System::String ^ rColumnName = rTable->GetCell(sc_nNameRow, j);
			XChecksum nColumnNameChecksum = CalculateChecksumFromString(rColumnName);
			System::String ^ rCellValue = rTable->GetCell(i, j);
			if (rColumnType->Equals("int"))
			{
				pBranch->SetInt(nColumnNameChecksum, System::Convert::ToInt32(rCellValue));
			}
			else if (rColumnType->Equals("float"))
			{
				pBranch->SetFloat(nColumnNameChecksum, System::Convert::ToSingle(rCellValue));
			}
			else if (rColumnType->Equals("bool"))
			{
				pBranch->SetBool(nColumnNameChecksum, System::Convert::ToBoolean(rCellValue));
			}
			else if (rColumnType->Equals("string"))
			{
				char * pCString = (char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(rCellValue).ToPointer();
				pBranch->AddString(nColumnNameChecksum, pCString);
				System::Runtime::InteropServices::Marshal::FreeHGlobal((System::IntPtr)pCString);
			}
			else if (rColumnType->Equals("checksum") || rColumnType->Equals("key"))
			{
				pBranch->AddChecksum(nColumnNameChecksum, CalculateChecksumFromString(rCellValue));
			}
		}
		pRoot->AddDataTree(pBranch);
	}

	return pRoot;
}

///----------------------------------------------------------------------------