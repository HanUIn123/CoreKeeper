#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CAnimator;
class CCollider;
class CCalculator;
class CState;

END

class CSlime : public Engine::CGameObject
{
	explicit CSlime(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSlime();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	void Slime_Animation(STATE st);
	void Focus_Camera();
	HRESULT Drop_Items(LPDIRECT3DDEVICE9 pGraphicDev);
	void Ego_Behavior(_float fTimeDelta);	//지혼자 움직이는 거
	_float On_Damaged(_float damage);
	_bool Is_Dead();
	void SetRandomDestination();
	void MoveTowardDestination(const _float& fTimeDelta);

private:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;
	Engine::CCollider* m_pColliderCom;
	Engine::CState* m_pStateCom;

	Engine::STATE			m_eState;
	Engine::STAT*			iHp;
	Engine::STAT*			iMaxHp;
	Engine::STAT*			iAttack;
	Engine::STAT*			iDefence;

	Engine::STAT			m_tStat;

	DWORD m_dwDeadTime = GetTickCount();
	DWORD m_dwIdleTime = GetTickCount();
	DWORD m_dwWalkTime = GetTickCount();
	DWORD m_dwAttackTime = GetTickCount();

	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vDestPos;
	_float			m_fSpeed;
	_bool			m_bIsWalking;
	_bool			m_bDead;

	struct ITEMDESC {
		D3DXVECTOR3				fPosition; 
		const wchar_t*			pTextureComponent; 
		const wchar_t*			pTexturePrototype; 
		int						eItemName; 
	};

	//Engine::STAT			iHp;
	//Engine::STAT			iAttack;
	//Engine::STAT			iDefense;

public:
	static CSlime* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

