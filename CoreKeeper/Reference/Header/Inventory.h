#pragma once
#include "Component.h"
#include "../Client/Header/Item.h"

BEGIN(Engine)

class ENGINE_DLL CInventory : public CComponent
{
private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	HRESULT				Ready_Inventory(int _iSlotCount);

	void				Sort_Item(_int iIndex = 10);
	bool				Add_Item(CItem* _pItem);
	bool				Minus_Item(ITEMNUM _eItemNum, int _iCount = 1);

	void				Swap_Item(CItem** _ppItem1, CItem** _ppItem2);
	void				Remove_Item(int _iIndex);
	bool				Enough_Item(ITEMNUM _eItemNum, int _iCount);
	void				Put_Same_Item(CInventory* _Inventory);

	void				Move_All_Item(CInventory* _pInventory);

	CItem*				Get_HandedItem(_int iHandNum);
	bool				Check_Empty(_int iIndex);

	vector<CItem*>		Get_VecItem()           { return m_vecItems; }
	vector<CItem*>*		Get_VecItemP()          { return &m_vecItems; }
	CItem*				Get_Item(_int iIndex)                { return m_vecItems[iIndex]; }

	_int				Get_SlotCount()              { return m_iSlotCount; }
	void				Set_SlotCount(_int iCount);
public:
	static	CInventory*		Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iSlotCount);
	virtual CComponent*		Clone();
	
private:
	virtual void Free();

private:
	vector<CItem*> m_vecItems;
    int m_iSlotCount;
};

END