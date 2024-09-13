#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;
class CAnimTex;
class CAnimator;

END

class CUIScreenInv : public Engine::CGameObject
{
private:
	explicit CUIScreenInv(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIScreenInv();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _int _iIndex);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_CurInv(_int _iCurInv) { m_iCurInv = _iCurInv; }

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;
	_int m_iIndex;

	RECT m_BRect;

	_int m_iCurInv;

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
//	Engine::CTexture* m_pItemTextureCom;
	Engine::CInventory* m_pInventoryCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CRcTex* m_pRcTextureCom;

public:
	static CUIScreenInv* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex);

private:
	virtual void		Free();
};
