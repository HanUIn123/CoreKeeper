#pragma once
#include "Base.h"
#include "Engine_Define.h"

BEGIN(Engine)

class CGameObject;

END

class CTerrain;
class CFarmMgr : public CBase
{
	DECLARE_SINGLETON(CFarmMgr)

private:
	explicit CFarmMgr();
	virtual ~CFarmMgr();

public:
	HRESULT		Ready_Farm(LPDIRECT3DDEVICE9 pGraphicDev);
	void		Update_Farm(const _float& fTimeDelta);

public:
	void		Plant_Job(_int iIndex, ITEMNUM eIngredient);
	_bool		Create_Plant(_int iIndex, ITEMNUM eIngredient);
	_bool		Harvest_Plant(_int iIndex);
	void		Watering_Plant(_int iIndex);

public:
	_bool		Get_GrownUp(_int iIndex);

private:
	void		Grow_Plant();
	void		Create_Seed(ITEMNUM eItemNum, _vec3 vPos);
	void		Create_Result(ITEMNUM eItemNum, _vec3 vPos);

private:
	_int		iCreateNumber;

private:
	LPDIRECT3DDEVICE9						m_pGraphicDev;

	map<wstring, CGameObject*>				m_mapPlant; // 인덱스로 게임 오브젝트 확인
	list<wstring>							m_listPlantedName; // 실제 키 값 : Plant_iIndex
	
	vector<POINT>							m_vecPlantedState; // 인덱스로 지난 시간 확인
	POINT									m_tState; // x : 인덱스, y : 지난 시간
	CTerrain*								m_pTerrain;
	_int									m_iGrowTime;

	vector<wstring>							m_vecSeedName;
	vector<wstring>							m_vecResultName;

private:
	virtual void		Free();
};

