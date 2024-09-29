#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
END

class CUIChestSort : public Engine::CGameObject
{
private:
	explicit CUIChestSort(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIChestSort();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	_bool           Map_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_BRect, _screenPos);
	}

	void            Set_Window(wstring _wCheststring) {
		if (m_bWindow)
			m_bWindow = false;
		else
		{
			m_bWindow = true;

			m_wstring = _wCheststring;
		}
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
	_bool m_bPushed;

	wstring m_wstring;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTransform* m_pTransformCom;

public:
	static CUIChestSort* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
