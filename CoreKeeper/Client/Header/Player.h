#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"
#include "Item.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CCalculator;
class CAnimator;
class CCollider;
class CState;
class CInventory;
class CFall;

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
	void			Flip();

	// 탑뷰 함수
	void			Key_Position(const _float& fTimeDelta);		// 플레이어 이동
	void			Mouse_Direction();							// 마우스 위치에 따른 캐릭터 방향 설정
	void			Animation_SetUp(STATE st, DIRECTION dir);	// 애니메이션 설정

	// 숄더뷰 함수
	void			ShoulderView_Control(const _float& fTimeDelta); // 숄더 뷰 시점 플레이어 조작(회전, 스윙 제외)
	void			ShoulderView_Swing();

	void			Set_Stop(_vec3* vDir1, _float fDirSpeed1, _vec3* vDir2 = nullptr, _float fDirSpeed2 = 0.f);

	void			Set_Equipment();
	void			Show_Equipment();
	void			Swing_Equipment();

	void			Set_EquippedStatus();
	void			Set_Clothes();

	void            Set_UI();

public:
	CItem*			Get_HandedItem()		{ return m_pHandedItem; }
	STATE			Get_CurState()			{ return m_eState; }
	// 손에 든 인벤토리 번호 가져오는 함수
	_int            Get_iHandNum()          { return m_iHandNum; }
	void            Set_iHandNum(_int iNum) { m_iHandNum = iNum; }
	_bool           Get_InvWindow()			{ return m_bInventory; }
	void            Set_InvWindow();
	void            Set_CraftWindow(); 
	void            Set_MapWindow();

	void            Set_Craft();
	void            Set_Inventory();
	void            Set_Map();
	void            Set_Status();
	void            Set_ChestInventory();
	void            Set_Statue();

	void            Set_DisMove() { m_bNoMove = true; }
	void            Set_EnaMove() { m_bNoMove = false; }

	void            Particle_Update(_float fTimeDelta);

	void			Set_KnockBack(_vec3 vEnemyPos, _int iDamage, _float fDist = 3.f);
	void			Set_Respawn() { m_bRespawned = false; }

private:
	void			KnockBack(const _float& fTimeDelta);

private:
	Engine::CAnimTex*		m_pBufferCom;
	Engine::CTransform*		m_pTransformCom;
	Engine::CTexture*		m_pTextureCom;
	Engine::CCalculator*	m_pCalculatorCom;
	Engine::CAnimator*		m_pAnimatorCom;
	Engine::CCollider*		m_pColliderCom;
	Engine::CState*			m_pStateCom;
	Engine::CInventory*		m_pInventoryCom;

	Engine::CInventory*     m_pEquipInventoryCom; // 장비 인벤토리

	Engine::DIRECTION		m_eDir;
	Engine::STATE			m_eState;
	float					m_fSpeed;
	float					m_fDiagSpeed;
	_int					m_iSpeedWeight;

	bool					m_bSwing;

	bool					m_bFlip;

	CItem*					m_pClothes[5];
	CItem*					m_pHandedItem;
	Engine::CTransform*		m_pHandedTransformCom;
	_int                    m_iHandNum; // 선택된 아이템 번호
	STAT					m_tBasicStat;
	STAT					m_tEquipmentStat;

	float					m_fFirstY;
	float					m_fTimeAcc;
	float					m_fWalkYSpeed;

	_bool                   m_bInventory; // Inventory 창 확인용
	_bool                   m_bCraft;     // Craft 창 확인용
	_bool                   m_bMap;       // Map 창 확인용
	_bool                   m_bStatus;
	_bool                   m_bChestInventory;
	_bool                   m_bTableCraft;
	_bool                   m_bAnvil;
	_bool                   m_bStatue;

	_bool                   m_bNoMove;    // UI건드릴때 캐릭터 움직이지 않게하는 용도

	_bool                   m_bBleed;
	_float                  m_fBleedTime;

	_vec3					m_vStartPoint;
	_vec3					m_vKnockBackDir;
	_bool					m_bKnockBackStart;
	_bool					m_bKnockBackEnd;
	_float					m_fKnockBackDist;

	_bool					m_bNude;

	Engine::CFall* m_pFireParticleCom; // 파티클 컴퍼넌트

	_vec3					m_vRespawnPoint;
	_bool					m_bRespawned;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
