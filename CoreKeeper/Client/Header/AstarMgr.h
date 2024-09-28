#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "Define.h"
#include "GraphicDev.h"
#include "Export_Utility.h"

class AstarMgr : public CBase
{
	DECLARE_SINGLETON(AstarMgr)

private:
	explicit AstarMgr();
	virtual ~AstarMgr();

public:
	list<int>* GetBestList() { return &m_BestList; }
public:
	void Start_AStar(_vec3* _vStart, _vec3* _vGoal);
	void Release();
private:
	bool Make_Route(int _iStart, int _iGoal);
	void Make_BestList(int _iStart, int _iGoal);

	int Get_Index(_vec3* _vStart);

private:
	list<int> m_CloseList;
	list<int> m_OpenList;

	list<int> m_BestList;
	int m_iStartIndex;

public:
	void Free() override;

};