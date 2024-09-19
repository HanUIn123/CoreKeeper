#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CInventory;

END

class CUICursor : public Engine::CGameObject
{
private:
	explicit CUICursor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUICursor();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_vec3 vUIPos;
	_matrix m_UIMatrix;

	bool m_bDisable;

public:
	void Set_Cursor_Disable() {
		if (m_bDisable)
		{
			m_bDisable = false;
		}
		else
			m_bDisable = true;
	}

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CInventory* m_pInventory;

public:
	static CUICursor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
