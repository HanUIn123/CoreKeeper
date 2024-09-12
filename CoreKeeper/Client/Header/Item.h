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

private:
	HRESULT			Add_Component();

protected:
	void			Apply_Billboard();
	void			Wave(const _float& fTimeDelta);

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
	bool				Get_Active() { return m_bActive; }
	Engine::ITEMNUM		Get_ItemNum() { return m_eItemNum; }

	// 임시
	void				Set_Active(bool bActive) { m_bActive = bActive; }
	void				Set_Drop(bool bDrop) { m_bDrop = bDrop; }
	void				Set_Use(bool bUse) { m_bUse = bUse; }

protected:
	int			m_iTextureNumber;
	float		m_fFirstY;
	float		m_fTimeAcc;
	float		m_fSpeed;

	bool		m_bActive;
	bool		m_bDrop;		// 땅에 떨어진 상태일 때
	bool		m_bDropSelf;	// 플레이어가 떨궜을때
	bool		m_bUse;			// 플레이어가 사용중

	STAT		m_tStat;
	Engine::ITEMNUM		m_eItemNum;
};
