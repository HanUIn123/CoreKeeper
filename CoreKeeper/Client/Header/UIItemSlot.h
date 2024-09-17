#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;
//class CCalculator;
//class CAnimator;

END

class CUIItemSlot : public Engine::CGameObject
{
public:
	enum SLOTTYPE { SLOT_HELM, SLOT_NECKLACE, SLOT_RING1, SLOT_CHEST, SLOT_RING2, SLOT_LEGGINGS, SLOT_WEAPON, SLOT_LANTTERN, SLOT_BAG, SLOT_PET, SLOT_END };

private:
	explicit CUIItemSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIItemSlot();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, SLOTTYPE _eType);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_Window() {
		if (m_bWindow)
			m_bWindow = false;
		else
			m_bWindow = true;
	}

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;

	RECT m_BRect;

	_bool m_bWindow;
	_bool m_bCollapse;
	_bool m_bFirst;

	SLOTTYPE m_eSlotType;

	vector<CItem*> m_vecItem;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;

public:
	static CUIItemSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, SLOTTYPE _eType);

private:
	virtual void		Free();
};
