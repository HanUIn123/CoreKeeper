#pragma once
#include "GameObject.h"
#include "Define.h"
#include "..\Header\Item.h"
#include "..\Header\UIStatue.h"
#include "..\Header\UICursor.h"
#include "Export_System.h"
#include "Export_Utility.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;

END

class CUIStatueCraft: public Engine::CGameObject
{
private:
	explicit CUIStatueCraft(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIStatueCraft();

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

		CUIStatue* pStatue = dynamic_cast<CUIStatue*>(Engine::Get_GameObject(L"Layer_UI", L"UI_Statue"));

		m_iIndex = pStatue->Get_StatueType();
	}

	void            Set_Index(_int iIndex) { m_iIndex = iIndex; }

private:
	HRESULT			Add_Component();

private:
	_vec2 m_vPos;

	RECT m_BRect;

	_bool m_bWindow;
	_bool m_bCollapse;
	_bool m_bFirst;

	_bool m_bEnough;

	CItem* m_pItem;

	_int   m_iIndex;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CInventory* m_pInventory;

public:
	static CUIStatueCraft* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
