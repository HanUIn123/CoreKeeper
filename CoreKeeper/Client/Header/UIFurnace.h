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

class CUIFurnace : public Engine::CGameObject
{
private:
	explicit CUIFurnace(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIFurnace();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(CInventory* pInventory1, CInventory* pInventory2, _float* fTime) {
		if (m_bWindow)
			m_bWindow = false;
		else
		{
			m_bWindow = true;

			m_pInventoryCom = pInventory1;
			m_pSecondInventoryCom = pInventory2;

			m_fTime = fTime;
		}
	}

	void            Set_Diable() { m_bWindow = false; }

	_bool           LSlot_Picked(POINT _screenPos) {
		return ::PtInRect(&m_BLRect, _screenPos);
	}
	_bool           RSlot_Picked(POINT _screenPos) {
		return ::PtInRect(&m_BRRect, _screenPos);
	}

private:
	void            Set_BarTexture(Engine::CRcTex* _pBufferCom, _int _iIndex);

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;
	_bool m_bWindow;

	RECT m_BLRect;
	RECT m_BRRect;

	RECT m_Rect;

	_float* m_fTime;

	RECT m_FirstRect;

	_bool m_bLSlot;
	_bool m_bRSlot;
private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pCBarBufferCom;
	Engine::CRcTex* m_pIBarBufferCom;
	Engine::CRcTex* m_pSBarBufferCom;
	Engine::CRcTex* m_pLSlotBufferCom;
	Engine::CRcTex* m_pRSlotBufferCom;
	Engine::CRcTex* m_pSelBufferCom;
	Engine::CRcTex* m_pSciBufferCom;
	Engine::CRcTex* m_pBackBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pBarTextureCom;
	Engine::CInventory* m_pInventoryCom;
	Engine::CInventory* m_pSecondInventoryCom;
	Engine::CTexture* m_pSelTextureCom;

public:
	static CUIFurnace* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
