#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"
#include "..\Header\UIStatue.h"
#include "..\Header\UICursor.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;

END

class CUIJemSlot : public Engine::CGameObject
{
private:
	explicit CUIJemSlot(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIJemSlot();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
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

	CItem* m_pItem;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CInventory* m_pInventory;
	Engine::CTexture* m_pArrowTextureCom;

public:
	static CUIJemSlot* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
