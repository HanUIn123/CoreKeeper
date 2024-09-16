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

	// 뷰 공통
	void			Mouse_Click();								// 클릭 시 스윙, 스윙 끝 판단
	void			Walk_Y(const _float& fTimeDelta);

	// 탑뷰 함수
	void			Key_Position(const _float& fTimeDelta);		// 플레이어 이동
	void			Mouse_Direction();							// 마우스 위치에 따른 캐릭터 방향 설정
	void			Animation_SetUp(STATE st, DIRECTION dir);	// 애니메이션 설정

	// 숄더뷰 함수
	void			ShoulderView_Control(const _float& fTimeDelta); // 숄더 뷰 시점 플레이어 조작(회전, 스윙 제외)
	void			ShoulderView_Swing();

	void			Show_Equipment();
	void			Swing_Equipment();

	void            Set_UI();
public:
	// 손에 든 인벤토리 번호 가져오는 함수
	_int            Get_iHandNum()          { return m_iHandNum; }
	void            Set_iHandNum(_int iNum) { m_iHandNum = iNum; }

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
	bool					m_bSwing;

	_int                    m_iHandNum;

	CGameObject*			m_pWeapon;
	float					m_fFirstY;
	float					m_fTimeAcc;
	float					m_fWalkYSpeed;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
