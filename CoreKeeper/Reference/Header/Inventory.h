#pragma once
#include "Component.h"
#include "../Client/Header/Item.h"

BEGIN(Client)

class CItem;

END

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

	void			Add_Item(CItem* _pItem);
	//void			Change_Item(int _iIndex, CItem* _pPushItem, CItem* _pPullItem);
	void			Remove_Item(ITEMNUM _eItemNum);

	bool            Check_Empty(_int iIndex);
	vector<CItem*>   Get_VecItem()           { return m_vecItems; }
	_int             Get_Slot()              { return m_iSlotCount; }
	void            Set_SlotCount(_int iCount) { m_iSlotCount = iCount; }

public:
	static	CInventory*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iSlotCount);
	virtual CInventory*		Clone();
	
private:
	virtual void Free();

private:
	vector<CItem*> m_vecItems;
    //vector<ItemInfo> m_vecItems;
    int m_iSlotCount;
};

END