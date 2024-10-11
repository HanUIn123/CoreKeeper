#pragma once
#include "GameObject.h"
#include "Item.h"
#include "Terrain.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CState;
class CTexture;
class CAnimator;
class CCalculator;
class CCollider;
class CHit;

END

class CPlayer;
class CMonster : public Engine::CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMonster();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos) PURE;
	virtual			_int			Update_GameObject(const _float& fTimeDelta) PURE;
	virtual			void			LateUpdate_GameObject() PURE;
	virtual			void			Render_GameObject() PURE;

protected:
	virtual			void			Pattern_Idle(const _float& fTimeDelta) PURE;
	virtual			void			Pattern_Chase(const _float& fTimeDelta) PURE;
	virtual			void			Pattern_Attack(const _float& fTimeDelta) PURE;
	virtual			void			Pattern_Dead() PURE;
	virtual			STATE			State_Change() PURE;

	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) * 0.5f); }
	
	void			Apply_Billboard();
	void			Flip();

	void			JumpY(const _float& fTimeDelta);
	void			FallDir(const _float& fTimeDelta);

	void			Check_Hitted();
	void			KnockBack(const _float& fTimeDelta, const _float& fDist);

	void			Drop_Item();
	void			Drop_All_Item();

	void			Set_Stop(const _float& fTimeDelta, _vec3* vDir1, _float fDirSpeed1, _vec3* vDir2 = nullptr, _float fDirSpeed2 = 0.f);
	void			Set_StuckFree(const _float& fTimeDelta);

	_bool			Check_Wall();
	void			Set_Cast();

	void			Respawn(const _float& fTimeDelta);
	void			Set_RespawnTimer(const _float& fTimeDelta);

	void			Set_SoundVolumeByDistance();

public:
	_bool			Get_StopDraw() { return m_bStopDraw; }
	void			Set_StopDraw(_bool bDraw) { m_bStopDraw = bDraw; }


private:
	void			Check_WallWithPlayer();

protected:
	Engine::CAnimTex*		m_pBufferCom;
	Engine::CState*			m_pStateCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CAnimator*		m_pAnimatorCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CCollider*      m_pColliderCom;

	Engine::MONSTERTYPE		m_eType;
	Engine::STATE			m_eState;

	Engine::CHit*           m_pHitParticleCom;

	_bool                   m_bHit;

	_bool					m_bFlip;

	_float					m_fIdleY;
	_bool					m_bIdling;
	_float					m_fIdleTime;
	_float					m_fIdleTimeLimit;

	_float					m_fJumpY;
	_bool					m_bJumping;
	_float					m_fJumpHeight;
	_float					m_fJumpSpeed;
	_float					m_fJumpTime;
	_float					m_fJumpFrame;

	_vec3					m_vStartPoint;

	_float					m_fAttackDistance;
	_float					m_fAttackTime;
	_vec3					m_vAttackPoint;
	_bool					m_bAttackSuccess;
	_bool					m_bAttackFailed;

	_bool					m_bKnockBackStart;
	_bool					m_bKnockBackEnd;

	_bool					m_bFallStart;
	_vec3					m_vFallDir;

	// 실제 이동 방향 : 8방향
	_int					m_iDir;
	// 텍스쳐 방향 : 4방향
	Engine::DIRECTION		m_eDir;
	_float					m_fSpeed;
	_float					m_fDiagSpeed;
	_float					m_fSpeedWeight;

	_float					m_fAggroDistance;

	_bool					m_bStopDraw;

	_float					m_fImmuneTime;
	_float					m_fImmuneTimeLimit;
	_bool					m_bImmuneEnd;

	vector<ITEMNUM>			m_vecDropItem;
	vector<wstring>			m_vecItemName;

	_int					m_iSpeedWeight;

	CTerrain*				m_pTerrain;
	CPlayer*				m_pPlayer;
	CTransform*				m_pPlayerTransform;
	CState*					m_pPlayerState;
	CCollider*				m_pPlayerCollider;
	_bool					m_bCheckWall;

	_vec3					m_vRespawnPoint;
	_bool					m_bRespawned;

	_float					m_fRespawnTimer;

	_float					m_fSoundVolume;

	static int				m_iTagNumber;

public:
	vector<const _tchar*>         m_vecFont;

	const _tchar* m_Font[10];

	_int m_iFontNum;



private:
	virtual void		Free();
};
