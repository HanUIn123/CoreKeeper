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

class CUIChestInv : public Engine::CGameObject
{
private:
	explicit CUIChestInv(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIChestInv();

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

	void            Set_Show(CInventory* _pInventory) {
		if (m_bShow)
			m_bShow = false;
		else
		{
			m_bShow = true;

			m_pChestInv = _pInventory;
		}
	}

	void            Set_Disable() { m_bShow = false; }
private:
	HRESULT			Add_Component();

private:
	_int m_iIndex;

	RECT m_BRect;

	_int m_iCurInv;

	_bool m_bCollapse;

	_bool m_bShow;

	_bool m_bStay;

	CItem* pItem;

	CInventory* m_pChestInv;

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pNumTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTexture* m_pItemNumTextureCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CRcTex* m_pRcTextureCom;

public:
	static CUIChestInv* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _int _iIndex);

private:
	virtual void		Free();
};
