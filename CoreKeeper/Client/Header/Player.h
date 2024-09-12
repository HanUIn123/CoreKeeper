#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CCalculator;
class CAnimator;
class CCollider;
class CState;
class CInventory;

END

class CPlayer : public Engine::CGameObject
{
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

	// ºä °øÅë
	void			Mouse_Click();

	// Å¾ºä ÇÔ¼ö
	void			Key_Position(const _float& fTimeDelta);
	void			Mouse_Direction();
	void			Animation_SetUp(STATE st, DIRECTION dir);

	// ¼ñ´õºä ÇÔ¼ö
	void			ShoulderView_Control(const _float& fTimeDelta);

private:
	Engine::CAnimTex*		m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CAnimator*		m_pAnimatorCom;
	Engine::CCollider*		m_pColliderCom;
	Engine::CState*			m_pStateCom;
	Engine::CInventory*		m_pInventoryCom;

	Engine::DIRECTION		m_eDir;
	Engine::STATE			m_eState;
	float					m_fSpeed;
	float					m_fDiagSpeed;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
