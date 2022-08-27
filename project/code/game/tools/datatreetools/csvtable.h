namespace DataTreeTools
{
	public ref class CCSVTable
	{
	public:
		virtual void Load(System::IO::TextReader ^ rReader);
		virtual int GetNumRows();
		virtual int GetNumColumns();
		virtual System::String ^ GetCell(int nRow, int nColumn);
	private:
		array<array<System::String ^> ^> ^ m_rCells;  ///Wow, that's a fun declaration!  Will it even work?
	};

}