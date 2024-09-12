#pragma once
#include "GameObject.h"
#include "Item.h"

BEGIN(Engine)

END

class CInventory : public Engine::CGameObject
{
protected:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CInventory();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

public:
	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void	Free();

public:
	void			Add_Item(CItem* pItem);
	void			Remove_Item(CItem* pItem);

	//UI용인데 수정해도 됨
	void			Show_Inventory();

private:
	vector<map<ITEMNUM, int>>	m_vecItems;
	int						m_iSlotCount;
};
