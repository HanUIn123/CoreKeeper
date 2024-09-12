#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CInventory : public CComponent
{
private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	HRESULT			Ready_Inventory(int _iSlotCount);

	void			Add_Item(ITEMNUM _eItemNum);
	void			Remove_Item(ITEMNUM _eItemNum);

public:
	static	CInventory*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iSlotCount);
	virtual CInventory*		Clone();

private:
	virtual void Free();

private:
    vector<map<ITEMNUM, int>> m_vecItems;
    int m_iSlotCount;
};

END