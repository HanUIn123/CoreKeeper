#pragma once
#include "GameObject.h"
#include "Define.h"
#include "../Header/Eye.h"
#include "../Header/Shirt.h"
#include "../Header/Pants.h"
#include "../Header/Hair.h"

BEGIN(Engine)

class CRcTex;
class CTransform;
class CTexture;
class CAnimTex;
class CAnimator;
class CInventory;
//class CCalculator;
//class CAnimator;

END

class CUIPlayerStatus : public Engine::CGameObject
{
private:
	explicit CUIPlayerStatus(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUIPlayerStatus();

public:
	virtual			HRESULT			Ready_GameObject(_vec2 vPos, _vec2 vSize);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

public:
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
	_bool m_bWindow;

	RECT m_bRect;

	CEye* m_pEye;

	CShirt* m_pShirt;

	CPants* m_pPants;

	CHair* m_pHair;

	CItem* m_pItem[3];

	CInventory* m_pEquipInv;

	_bool m_bFirst;

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CTexture* m_pSkinTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

public:
	static CUIPlayerStatus* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec2 vPos, _vec2 vSize);

private:
	virtual void		Free();
};
