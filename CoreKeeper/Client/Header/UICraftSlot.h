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
	enum UICRAFTITEM { UCITEM_TORCH, UCITEM_WOODENPICK, UCITEM_WOODENSHOVEL, UCITEM_WORKBENCH, UCITEM_CHEST, UCITEM_END };

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
	
	wstring Craftstring[500];
	_int m_iCraftCount;

	_vec2 m_vPos;

	RECT m_BRect;

	_bool m_bWindow;
	_bool m_bCollapse;
	_bool m_bFirst;

	_bool m_bEnough;

	UICRAFTITEM m_eSlotType;
	ITEMNUM     m_eItemType[3];

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
