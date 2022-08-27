#pragma once

class CStringTable;

class CDataTree
{
public:

	struct SIDIndexMap
	{
		SIDIndexMap();
		~SIDIndexMap();
		void Setup(int nMaxItemes, XEngine::XAllocator * pAllocator);
		void AddItem(XChecksum nID);
		bool HasItemIndex(XChecksum nID) const;
		int GetItemIndex(XChecksum nID) const;
		XChecksum GetItem(int nIndex) const;	
		struct SIDIndexPair
		{
			SIDIndexPair();
			XChecksum	m_nChecksum;
			int			m_nIndex;
		};
		int				m_nMaxItems;
		int				m_nNumItems;
		SIDIndexPair *	m_pIDIndexPairs;
	};

	class CHeader
	{
	public:
		CHeader();

		void SetupInts(int nMaxInts, XEngine::XAllocator * pAllocator);
		void SetupFloats(int nMaxFloats, XEngine::XAllocator * pAllocator);
		void SetupBools(int nMaxBools, XEngine::XAllocator * pAllocator);
		void SetupStrings(int nMaxStrings, XEngine::XAllocator * pAllocator);
		void SetupChecksums(int nMaxChecksums, XEngine::XAllocator * pAllocator);
		void SetupTrees(int nMaxTrees, int nMaxKeys, XEngine::XAllocator * pAllocator);

		void AddInt(XChecksum nID);
		void AddFloat(XChecksum nID);
		void AddBool(XChecksum nID);
		void AddString(XChecksum nID);
		void AddChecksum(XChecksum nID);
		void AddKey(XChecksum nKey);

		int GetIntIndex(XChecksum nID) const;
		int GetFloatIndex(XChecksum nID) const;
		int GetBoolIndex(XChecksum nID) const;
		int GetStringIndex(XChecksum nID) const;
		int GetChecksumIndex(XChecksum nID) const;
		int GetKeyIndex(XChecksum nID) const;

		int GetMaxInts() const;
		int GetMaxFloats() const;
		int GetMaxBools() const;
		int GetMaxStrings() const;
		int GetStringTableSize() const;
		int GetMaxChecksums() const;
		int GetMaxKeys() const;
		int GetMaxDataTrees() const;

		bool HasInt(XChecksum nID) const;
		bool HasFloat(XChecksum nID) const;
		bool HasBool(XChecksum nID) const;
		bool HasString(XChecksum nID) const;
		bool HasChecksum(XChecksum nID) const;
		bool HasKey(XChecksum nKey) const;

		int GetNumKeys() const;
		XChecksum GetKey(int nIndex) const;

	protected:
		int				m_nMaxTrees;
		SIDIndexMap		m_oIntMap;
		SIDIndexMap		m_oFloatMap;
		SIDIndexMap		m_oBoolMap;
		SIDIndexMap		m_oStringMap;
		SIDIndexMap		m_oChecksumMap;
		SIDIndexMap		m_oKeyMap;
	};

	/// For creating an empty data-tree, should be used rarely in game code
	CDataTree(CHeader * pHeader, XEngine::XAllocator * pAllocator, CStringTable * pStringTable = nullptr);
	~CDataTree();

	bool HasInt(XChecksum nID) const;
	int GetInt(XChecksum nID) const;
	void SetInt(XChecksum nID, int nValue);

	bool HasFloat(XChecksum nID) const;
	float GetFloat(XChecksum nID) const;
	void SetFloat(XChecksum nID, float fValue);

	bool HasBool(XChecksum nID) const;
	bool GetBool(XChecksum nID) const;
	void SetBool(XChecksum nID, bool bValue);

	bool HasString(XChecksum nID) const;
	char const * GetString(XChecksum nID) const;
	void AddString(XChecksum nID, char const * pString);
	///Strings are immutable due to stringtable implementation, sorry.

	bool HasChecksum(XChecksum nID) const;
	XChecksum GetChecksum(XChecksum nID) const;
	void AddChecksum(XChecksum nID, XChecksum nChecksum);
	///Checksums are also immutable, due to their ability to serve as keys

	bool HasKey(XChecksum nKey) const;
	void AddKey(XChecksum nKey) const;
	///Keys are immutable, although really that just means you can't remove them

	int GetNumDataTrees() const;
	CDataTree * GetDataTree(int nIndex) const;
	bool HasDataTree(XChecksum nKey, XChecksum nID) const;
	CDataTree * GetDataTree(XChecksum nKey, XChecksum nID) const;
	void AddDataTree(CDataTree * pDataTree);
	///Data tree pointers are also immutable, but you can go ahead and modify their contents
	///You know, unless you wanted to change their strings or checksums

protected:
	CHeader *		m_pHeader; ///does not own the header
	int *			m_pInts;
	float *			m_pFloats;
	bool *			m_pBools;	///Could probably toss together a bitfield for this...
	int *			m_pStringIndices;
	CStringTable *	m_pStrings;
	XChecksum *		m_pChecksums;
	int				m_nNumDataTrees;
	CDataTree **	m_ppDataTrees; ///currently does not own children
	SIDIndexMap *	m_pDataTreeMaps;
};