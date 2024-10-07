#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
//class CCalculator;
//class CAnimator;

END

class CUICraftButton : public Engine::CGameObject
{
private:
	explicit CUICraftButton(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICraftButton();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
	void            Set_Window(TABLETYPE _eType = TABLE_PLAYER, MATERIAL _eMaterial = MATERIAL_WOOD) {
		
			m_bWindow = true;

			if (_eType == TABLE_ANVIL)
				eTableMaterial = (MATERIAL)(_eMaterial - 1);
			else
				eTableMaterial = _eMaterial;

			m_eTableType = _eType;
	}

	void            Set_DisableWindow() {
		m_bWindow = false;
	}

	_bool           UPRECT_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_UpRect, _screenPos);
	}


	_bool           DOWNRECT_Picked(POINT _screenPos) {
		return  ::PtInRect(&m_DownRect, _screenPos);
	}
	
private:
	HRESULT			Add_Component();

	void            Set_Slot();

private:
	_vec2 m_vPos;
	_bool m_bWindow;

	MATERIAL eTableMaterial;
	TABLETYPE m_eTableType;

	RECT m_UpRect;
	RECT m_DownRect;

	_bool m_bUpCollision;
	_bool m_bDownCollision;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CRcTex* m_pUArrowBufferCom;
	Engine::CRcTex* m_pDArrowBufferCom;
	Engine::CRcTex* m_pUColBufferCom;
	Engine::CRcTex* m_pDColBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pColTextureCom;
	Engine::CTexture* m_pArrowTextureCom;

public:
	static CUICraftButton* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
