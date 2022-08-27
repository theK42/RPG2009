#include "game/shared/gamepch.h"
#include "game/tools/datatreetools/csvtable.h"

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

void DataTreeTools::CCSVTable::Load( System::IO::TextReader ^ rReader )
{
	///This is obviously the most naive CSV parser in existence
	System::String ^ rLine;
	m_rCells = gcnew array<array<System::String ^> ^>(0);
	array<System::Char> ^ rSeparators = gcnew array<System::Char>{
		','
	};
	while ((rLine = rReader->ReadLine()) != nullptr) ///Weee! Assignment returns the value assigned!
	{
		XASSERT_MSG(!rLine->Contains("\""), "CSVTable does not currently support anything tricky.");
		array<System::String ^> ^ rSplitLine =  rLine->Split(rSeparators);
		array<array<System::String ^> ^>::Resize(m_rCells, m_rCells->Length + 1);
		m_rCells[m_rCells->Length - 1] = rSplitLine;
	}
}

///----------------------------------------------------------------------------

int DataTreeTools::CCSVTable::GetNumRows()
{
	return m_rCells->Length;
}

///----------------------------------------------------------------------------

int DataTreeTools::CCSVTable::GetNumColumns()
{
	if (m_rCells->Length >= 1)
	{
		return m_rCells[0]->Length;
	}
	else
	{
		return 0;
	}
}

///----------------------------------------------------------------------------

System::String ^ DataTreeTools::CCSVTable::GetCell( int nRow, int nColumn )
{
	XASSERT(IsBounded(nRow, 0, m_rCells->Length - 1));
	XASSERT(IsBounded(nColumn, 0, m_rCells[nRow]->Length - 1));
	return m_rCells[nRow][nColumn];
}

///----------------------------------------------------------------------------