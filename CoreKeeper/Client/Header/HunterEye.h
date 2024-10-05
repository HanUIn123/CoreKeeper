#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;
class CCalculator;

END
class CHunter;
class CHunterEye : public CGameObject
{
private:
	explicit CHunterEye(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHunterEye();
public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Follow_Hunter();
	void			Set_Cast();

public:
	void			Set_Hunter(CGameObject* pHunter);

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CCalculator* m_pCalculatorCom;

	CHunter*	m_pHunter;
	CTransform* m_pHunterTransform;
	CAnimator*	m_pHunterAnimator;

	CTransform* m_pPlayerTransform;

public:
	static CHunterEye* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

