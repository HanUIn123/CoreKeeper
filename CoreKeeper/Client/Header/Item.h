#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CCollider;
class CShadowTex;

END

class CItem : public Engine::CGameObject
{
protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

protected:
	HRESULT			Add_Component();
	void			Apply_Billboard();

protected:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom;

	Engine::CShadowTex* m_pShadowBufferCom;
	Engine::CTransform* m_pShadowTransformCom;
	Engine::CTexture* m_pShadowTextureCom;

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();

public:
	bool		Get_Active() { return m_bActive; }
	Engine::ITEMNUM		Get_ItemNum() { return m_eItemNum; }
protected:
	int			m_iTextureNumber;
	float		m_fFirstY;
	float		m_fTimeAcc;
	bool		m_bActive;
	bool		m_bDrop;	// 땅에 있는 상태인지

	STAT		m_tStat;
	Engine::ITEMNUM		m_eItemNum;
};
