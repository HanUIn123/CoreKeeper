#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CInventory : public CComponent
{
public:
	struct ItemInfo
	{
		ITEMNUM eItemNum;
		int iCount;
		IDirect3DBaseTexture9* pItemTexture;
	};

private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	HRESULT			Ready_Inventory(int _iSlotCount);

	void			Add_Item(ITEMNUM _eItemNum, int _iCount, IDirect3DBaseTexture9* _pItemTexture);
	void			Remove_Item(ITEMNUM _eItemNum);

public:
	static	CInventory*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iSlotCount);
	virtual CInventory*		Clone();

private:
	virtual void Free();

private:
    vector<ItemInfo> m_vecItems;
    int m_iSlotCount;
};

END