#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;
class CAnimTex;
class CAnimator;

END

class CUICraftSlot : public Engine::CGameObject
{
public:
	enum CSLOTTYPE { SLOT_HELM, SLOT_NECKLACE, SLOT_RING1, SLOT_CHEST, SLOT_RING2, SLOT_LEGGINGS, SLOT_WEAPON, SLOT_LANTTERN, SLOT_BAG, SLOT_PET, SLOT_END };

private:
	explicit CUICraftSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICraftSlot();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize, _int iIndex);
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
	_int m_iIndex;

	_vec2 m_vPos;

	RECT m_BRect;

	_bool m_bWindow;
	_bool m_bCollapse;
	_bool m_bFirst;

	CSLOTTYPE m_eSlotType;

private:
	Engine::CAnimTex* m_pSlotBufferCom;
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pItemTextureCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTexture* m_pSlotTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUICraftSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize, _int iIndex);

private:
	virtual void		Free();
};
