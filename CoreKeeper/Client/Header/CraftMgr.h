#pragma once
#include "Base.h"
#include "Engine_Define.h"
#include "Include.h"

BEGIN(Engine)

class CInventory;

END

class CCraftMgr : public CBase
{
	DECLARE_SINGLETON(CCraftMgr)

public:
	struct IngredientInfo
	{
		ITEMNUM eIngredient;
		int iQuantity;
	};

	struct Recipe
	{
		vector<IngredientInfo> vecIngredients;
	};

private:
	explicit CCraftMgr();
	virtual ~CCraftMgr();

public:
	HRESULT		Ready_Craft(LPDIRECT3DDEVICE9 pGraphicDev);

	bool		Craftable(CInventory* _pInventory, ITEMNUM _eItemNum, MATERIAL _eMaterial = MATERIAL_END);
	CItem*		Craft(CInventory* _pInventory, ITEMNUM _eItemNum, MATERIAL _eMaterial = MATERIAL_END);

	CItem*      CraftExp(ITEMNUM _eItemNum, MATERIAL _eMaterial = MATERIAL_END);

	CItem*		Cook(ITEMNUM _eItemNum1, ITEMNUM _eItemNum2);
private:
	void		Set_Recipe();
	void		Set_Name();
private:
	virtual void		Free();

private:
	LPDIRECT3DDEVICE9					m_pGraphicDev;

	wstring								strCraftName[500];
	map<ITEMNUM, wstring>				m_mapName;
	int									m_iCraftCount;

	map<pair<ITEMNUM, MATERIAL>, Recipe> m_mapRecipes;
};