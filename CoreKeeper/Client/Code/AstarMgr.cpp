#include "pch.h"
#include "..\Header\AstarMgr.h"
#include "..\Header\Terrain.h"

IMPLEMENT_SINGLETON(AstarMgr)

AstarMgr::AstarMgr() : m_iStartIndex(0)
{
}

AstarMgr::~AstarMgr()
{
	Release();
}

void AstarMgr::Start_AStar(_vec3* _vStart, _vec3* _vGoal)
{
	Release();

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	vector<bool> vecUnreachable = pTerrain->Get_Unreachable();

	m_iStartIndex = Get_Index(_vStart);
	int iGoalIdx = Get_Index(_vGoal);

	if (0 > m_iStartIndex || m_iStartIndex > VTXCNTX * VTXCNTZ)
		return;

	if (0 > iGoalIdx || iGoalIdx > VTXCNTX * VTXCNTZ)
		return;

	if (m_iStartIndex == iGoalIdx)
		return;

	if (true == Make_Route(m_iStartIndex, iGoalIdx))
		Make_BestList(m_iStartIndex, iGoalIdx);
}

void AstarMgr::Release()
{
	m_OpenList.clear();
	m_CloseList.clear();
	m_BestList.clear();
}

bool AstarMgr::Make_Route(int _iStart, int _iGoal)
{
	CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	vector<bool> vecUnreachableIndex = pTerrain->Get_Unreachable();

	//if (!m_OpenList.empty())
	//	m_OpenList.pop_front();

	//m_CloseList.push_back(_iStart);

	//for (auto& pTile : vecAdj[_iStart])
	//{
	//	if (_iGoal == pTile->iIndex)
	//	{
	//		pTile->iParentIdx = _iStart;
	//		return true;
	//	}

	//	if (false == CheckOpen(pTile->iIndex) &&
	//		false == CheckClose(pTile->iIndex))
	//	{
	//		pTile->iParentIdx = _iStart;
	//		m_OpenList.push_back(pTile->iIndex);
	//	}
	//}

	//if (m_OpenList.empty())
	//	return false;

	//int	iStarIdx = _iStart;

	//m_OpenList.sort([&vecTile, &_iGoal, &iStarIdx](int Dst, int Src)->bool
	//	{
	//		D3DXVECTOR3	vPCost1 = vecTile[iStarIdx]->vPos - vecTile[Dst]->vPos;
	//		D3DXVECTOR3	vPCost2 = vecTile[iStarIdx]->vPos - vecTile[Src]->vPos;

	//		D3DXVECTOR3	vGCost1 = vecTile[_iGoal]->vPos - vecTile[Dst]->vPos;
	//		D3DXVECTOR3	vGCost2 = vecTile[_iGoal]->vPos - vecTile[Src]->vPos;

	//		float	fCost1 = D3DXVec3Length(&vPCost1) + D3DXVec3Length(&vGCost1);
	//		float	fCost2 = D3DXVec3Length(&vPCost2) + D3DXVec3Length(&vGCost2);

	//		return fCost1 < fCost2;
	//	});

	return Make_Route(m_OpenList.front(), _iGoal);
}

void AstarMgr::Make_BestList(int _iStart, int _iGoal)
{
	//CTerrain* pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));
	//vector<bool> vecUnreachable = pTerrain->Get_Unreachable();


	//vector<TILE*>& vecTile = dynamic_cast<CMyTerrain*>(pMyTerrain)->Get_vecTile();

	//m_BestList.push_front(vecTile[_iGoal]);

	//int	iRouteIdx = vecTile[_iGoal]->iParentIdx;

	//while (true)
	//{
	//	if (_iStart == iRouteIdx)
	//		break;

	//	m_BestList.push_front(vecTile[iRouteIdx]);

	//	iRouteIdx = vecTile[iRouteIdx]->iParentIdx;
	//}
}

int AstarMgr::Get_Index(_vec3* _vPos)
{
	return _int(_vPos->z + 0.5f * VTXITV) * (VTXCNTX - 1) + (_vPos->x + 0.5f * VTXITV);
}

void AstarMgr::Free()
{
}
