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
class CFollow;

END

class CTerrain;

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
	HRESULT			Setup_Material();
	void			SetUp_Light();

	// 뷰 공통
	void			Mouse_Click(const _float& fTimeDelta);								// 클릭 시 스윙, 스윙 끝 판단
	void			Walk_Y(const _float& fTimeDelta);
	void			Flip();
	void			Dash(const _float& fTimeDelta);
	void			Lantern();
	void			Bag();
	void			Set_ImmuneByTime(_float fImmuneTime = 1.f);
	void			Set_ImmuneByToggle();

	// 탑뷰 함수
	void			Key_Position(const _float& fTimeDelta);		// 플레이어 이동
	void			Mouse_Direction();							// 마우스 위치에 따른 캐릭터 방향 설정
	void			Animation_SetUp(STATE st, DIRECTION dir);	// 애니메이션 설정

	// 숄더뷰 함수
	void			ShoulderView_Control(const _float& fTimeDelta); // 숄더 뷰 시점 플레이어 조작(회전, 스윙 제외)
	void			ShoulderView_Swing();

	void			Set_Stop(_vec3* vDir1, _float fDirSpeed1, _vec3* vDir2 = nullptr, _float fDirSpeed2 = 0.f);

	void			Set_Clothes();
	void			Set_Equipment();
	void			Show_Equipment();
	void			Swing_Equipment();
	void			Shoot_Equipment();
	void			Set_EquippedStatus();

	void			PickAxe();
	void			Hoe();
	void			Watering();
	void			Plant(ITEMNUM eHandedNum);
	void			Install(ITEMNUM eHandedNum);
	void			Eat(ITEMNUM eHandedNum);

	void			Set_Buff(const _float& fTimeDelta);
	void			Set_Hungry(const _float& fTimeDelta);
	void			Respawn_Progress(const _float& fTimeDelta);

	void			Set_MouseWorldPos();
	void            Set_UI();

	void			Set_WallProjection();

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

	void            Reset_Inventory();
	void            Set_Craft(TABLETYPE eTableType = TABLE_PLAYER, MATERIAL _eMaterial = MATERIAL_WOOD);
	void            Set_Inventory();
	void            Set_Map();
	void            Set_Status();
	void            Set_ChestInventory(CInventory* _pInventory = nullptr);
	void            Set_GraveInventory(CInventory* pInventory = nullptr);
	void            Set_Statue(_int _StatueNum = 0, _bool _bInteractioned = false);
	void            Set_Furnace();
	void            Set_CookingPot();

	_bool           Get_GraveUI() { return m_bGraveInventory; }
	_bool           Get_FurnaceUI() { return m_bFurnace; }
	_bool           Get_CookingUI() { return m_bCookingPot; }
	_bool           Get_StatueUI() { return m_bStatue; }
	_bool           Get_CraftUI() { return m_bCraft; }

	void            UI_Disable();

	void            Set_DisMove() { m_bNoMove = true; }
	void            Set_EnaMove() { m_bNoMove = false; }

	void            Particle_Update(_float fTimeDelta);

	void			Set_KnockBack(_vec3 vEnemyPos, _int iDamage, _float fDist = 3.f, PLAYERHITTYPE eHit = HIT_NORMAL);
	void			Set_Respawn() { m_bRespawned = false; }
	void			Set_BuffState(BUFFTYPE eType, _bool bBuff) { m_arrBuffState[eType] = bBuff; };
	_bool			Get_BuffState(BUFFTYPE eType) { return m_arrBuffState[eType]; }
	void			Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; m_fDiagSpeed = sqrt(pow(m_fSpeed, 2) / 2); }

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
	Engine::CFall*			m_pFireParticleCom; // 파티클 컴퍼넌트
	Engine::CFollow*        m_pFollowParticleCom; // 파티클 컴퍼넌트
	Engine::CFall*          m_pDirtParticleCom;


	Engine::DIRECTION		m_eDir;
	Engine::STATE			m_eState;
	_float					m_fNormalSpeed;
	_float					m_fSpeed;
	_float					m_fDiagSpeed;
	_int					m_iSpeedWeight;

	_bool					m_bSwing;
	_bool					m_bShoot;
	_float					m_fClickTime;
	_bool					m_bShootOnce;

	_bool					m_bFlip;
	_bool					m_bNude;

	CItem*					m_pClothes[5];
	CItem*					m_pHandedItem;
	Engine::CTransform*		m_pHandedTransformCom;
	_int                    m_iHandNum; // 선택된 아이템 번호
	STAT					m_tBasicStat;
	STAT					m_tEquipmentStat;
	STAT					m_tBuffStat;
	_bool					m_arrBuffState[BUFFTYPE_END];

	_float					m_fFirstY;
	_float					m_fTimeAcc;
	_float					m_fWalkYSpeed;

	_bool                   m_bInventory; // Inventory 창 확인용
	_bool                   m_bCraft;     // Craft 창 확인용
	_bool                   m_bMap;       // Map 창 확인용
	_bool                   m_bStatus;
	_bool                   m_bChestInventory;
	_bool                   m_bGraveInventory;
	_bool                   m_bTableCraft;
	_bool                   m_bAnvil;
	_bool                   m_bStatue;
	_bool                   m_bFurnace;
	_bool                   m_bCookingPot;

	_matrix                 m_bPickaxeMatrix;

	_bool                   m_bNoMove;    // UI건드릴때 캐릭터 움직이지 않게하는 용도

	_bool                   m_bDestroyWall;

	_bool                   m_bBleed;
	_float                  m_fBleedTime;
	_bool					m_bFire;
	_float					m_fFireTickTime;

	_vec3					m_vStartPoint;
	_vec3					m_vKnockBackDir;
	_bool					m_bKnockBackStart;
	_bool					m_bKnockBackEnd;
	_float					m_fKnockBackDist;

	_vec3					m_vRespawnPoint;
	_bool					m_bRespawned;

	CTerrain*				m_pTerrain;
	_vec3					m_vMouseWorldPos;
	_bool					m_bDash;

	_float					m_fDashTime;
	_float					m_fDashTimeAcc;
	_bool					m_bDashCool;
	_float					m_fDashCoolTime;

	_bool					m_bImmune;
	_bool					m_bImmuneByTime;
	_float					m_fImmuneTimeAcc;
	_float					m_fImmuneTime;

	vector<wstring>			m_vecInstallObjectName;
	_int					m_iInstallNumber;

	// 랜턴
	_int					m_iLightNum;
	_float					m_fLightRange;

	_bool					m_bRespawnFirstFrame;
	_float					m_fRespawnProgress;


	_float					m_fHungerTime;

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
