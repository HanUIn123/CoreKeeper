#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;
class CCalculator;

END
class CPet;
class CPetTail : public CGameObject
{
private:
	explicit CPetTail(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPetTail();
public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Follow_Pet();

public:
	void			Set_Pet(CGameObject* pHunter);

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CAnimator* m_pAnimatorCom;

	CPet* m_pPet;
	CTransform* m_pPetTransform;
	CAnimator* m_pPetAnimator;

public:
	static CPetTail* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
