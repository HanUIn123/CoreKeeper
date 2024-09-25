#pragma once
#include "GameObject.h"
#include "Item.h"


BEGIN(Engine)

class CAnimTex;
class CTransform;
class CState;
class CTexture;
class CAnimator;
class CCalculator;
class CCollider;

END

class CMonster : public Engine::CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

protected:
	void			Apply_Billboard();
	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2); }

	void			Pattern_Idle(const _float& fTimeDelta);
	void			Pattern_Chase(const _float& fTimeDelta);
	void			Pattern_Attack(const _float& fTimeDelta);
	void			Pattern_Dead();

	void			Check_Hitted();
	void			KnockBack(const _float& fTimeDelta, const _float& fDist);
	void			Drop_Item();

private:
	void			JumpY(const _float& fTimeDelta);
	void			FallDir(const _float& fTimeDelta);

protected:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CState* m_pStateCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;
	Engine::CCollider* m_pColliderCom;

	Engine::MONSTERTYPE m_eType;
	Engine::STATE m_eState;

	_float				m_fIdleY;
	_bool				m_bIdling;
	_int				m_iIdleStack;

	_float				m_fJumpY;
	_bool				m_bJumping;
	_float				m_fJumpHeight;
	_float				m_fJumpSpeed;
	_float				m_fJumpTime;
	_float				m_fJumpFrame;

	_vec3				m_vStartPoint;

	_float				m_fAttackDistance;
	_float				m_fAttackTime;
	_vec3				m_vAttackPoint;
	_bool				m_bAttackSuccess;
	_bool				m_bAttackFailed;

	_bool				m_bKnockBackStart;
	_bool				m_bKnockBackEnd;

	_bool				m_bFallStart;
	_vec3				m_vFallDir;

	_int				m_iDir;
	Engine::DIRECTION	m_eDir;
	_float				m_fSpeed;
	_float				m_fDiagSpeed;
	_float				m_fSpeedWeight;

	_bool				m_bStopDraw;

	vector<ITEMNUM>		m_vecDropItem;

	static int			m_iTagNumber;

public:
	static CMonster*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void		Free();
};
