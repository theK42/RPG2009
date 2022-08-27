class CGameModule
{
public:
	virtual void Initialize(XEngine::XAllocator * pMainAllocator, XEngine::XAllocator * pDiscardableAllocator = nullptr);
	void Update(float fTime);
	virtual void Render();
	void Unload();
	virtual void Deinit();

protected:
	enum EState
	{
		eState_Unknown = -1,
		eState_Unloaded,
		eState_PendingLoad,
		eState_Loading,
		eState_Loaded,
		eState_Count
	};
	virtual int GetNumChildModules() const;
	virtual CGameModule * GetChildModule(int nWhich);
	virtual int GetNumExternalDependencies() const;
	virtual CGameModule * GetExternalDependency(int nWhich);
	
	bool m_bIsPrimary;



};