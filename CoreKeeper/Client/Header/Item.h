#pragma once
#include "GameObject.h"
#include "Define.h"

BEGIN(Engine)

class CAnimTex;
class CTransform;
class CTexture;
class CCollider;
class CColliderCube;
class CAnimator;
class CShadowTex;

END

class CItem : public Engine::CGameObject
{
protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CItem();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT			Add_Component();
	//void			SetUp_Light();

protected:
	void			Wave(const _float& fTimeDelta);
	void			Swing(int start, int end, int Count);
	void			Follow_Player();
	void			In_Inventory();
public:
	void			Walk_Equipped(const _float& fTimeDelta);

protected:
	Engine::CAnimTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCollider* m_pColliderCom;
	Engine::CColliderCube* m_pColliderCubeCom;
	Engine::CAnimator* m_pAnimatorCom;

	Engine::CShadowTex* m_pShadowBufferCom;
	Engine::CTransform* m_pShadowTransformCom;
	Engine::CTexture* m_pShadowTextureCom;

public:
	static CItem* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos = {0, 0, 0}, MATERIAL _eMaterial = MATERIAL_END);

protected:
	virtual void		Free();

public:
	bool				Get_Active() { return m_bActive; }
	Engine::ITEMNUM		Get_ItemNum() { return m_eItemNum; }
	int					Get_Count() { return m_iCount; }
	STAT*				Get_Stat() { return &m_tStat; }

	void				Add_Count(int _iAddCount) {m_iCount += _iAddCount; }
	void				Minus_Count(int _iMinusCount) { m_iCount -= _iMinusCount; }
	// 임시
	void				Set_Active(bool bActive) { m_bActive = bActive; }
	void				Set_Drop(bool bDrop) { m_bDrop = bDrop; }
	void				Set_Use(bool bUse) { m_bUse = bUse; }
	void				Set_Swing(DIRECTION eDir, bool bSwing) { m_eDir = eDir; m_bSwing = bSwing; }
	void				Set_Follow(bool bFollow = true) { m_bFollow = bFollow; }

	//아이템 설명창
	const wstring*      Get_Explain() { return m_wItemExplain; }
	const wstring       Get_Meterial() { return m_wItemMetrial; }
	_bool               Get_UseMet() { return m_bMeterial; }

public: // UI에서 가져갈 Component들
 	CAnimTex*			Get_Buffer() { return m_pBufferCom; }
	CTexture*			Get_Texture() { return m_pTextureCom; }
	CTransform*			Get_Transform() { return m_pTransformCom; }
	CAnimator*			Get_Animator() { return m_pAnimatorCom; }

protected:
	int			m_iTextureNumber;
	int			m_iCount;		// 개수
	float		m_fFirstY;
	float		m_fTimeAcc;
	float		m_fSpeed;
	float		m_fWalkYSpeed;
	float		m_fAngle;		// 휘두를때 회전 각도
	float		m_fAngleX;
	float       m_fAngleY;

	bool		m_bActive;
	bool		m_bDrop;		// 땅에 떨어진 상태일 때
	bool		m_bDropSelf;	// 플레이어가 떨궜을때
	bool		m_bUse;			// 플레이어가 사용중
	bool		m_bSwing;
	bool		m_bHasRotated;	//Swing에서 회전했는지 안했는지 체크용

	bool		m_bFollow;

	STAT		m_tStat;
	ITEMNUM		m_eItemNum;
	DIRECTION	m_eDir;
	MATERIAL	m_eMaterial;

	//아이템 설명
	wstring m_wItemExplain[30];
	wstring m_wItemMetrial;

	_bool   m_bMeterial;
	_bool	m_bCraftable;
};