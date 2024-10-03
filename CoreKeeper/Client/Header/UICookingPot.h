#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;
//class CCalculator;
//class CAnimator;

END

class CUICookingPot : public Engine::CGameObject
{
private:
	explicit CUICookingPot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICookingPot();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

	_bool m_bRender;

	RECT m_BRect[3];

	RECT m_Rect;
	RECT m_rFirstRect;

	_float m_fTime;

	_bool m_bRectPicked[3];

private:
	_bool           USlot_Picked(POINT _screenPos) {
		return ::PtInRect(&m_BRect[0], _screenPos);
	}

	_bool           DSlot_Picked(POINT _screenPos) {
		return ::PtInRect(&m_BRect[1], _screenPos);
	}

	_bool           RSlot_Picked(POINT _screenPos) {
		return ::PtInRect(&m_BRect[2], _screenPos);
	}

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pUSlotBufferCom;
	Engine::CRcTex* m_pDSlotBufferCom;
	Engine::CRcTex* m_pRSlotBufferCom;
	Engine::CRcTex* m_pSciBufferCom;
	Engine::CRcTex* m_pColBufferCom;
	Engine::CRcTex* m_pFoodBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTexture* m_pFoodTextureCom;
	Engine::CInventory* m_pUSlotInventoryCom;
	Engine::CInventory* m_pDSlotInventoryCom;
	Engine::CInventory* m_pRSlotInventoryCom;

public:
	void Set_Render() { 
		if (m_bRender)
			m_bRender = false;
		else
			m_bRender = true; }

	void Set_Disable() { m_bRender = false; }

public:
	static CUICookingPot* Create(LPDIRECT3DDEVICE9 pGraphicDe, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
