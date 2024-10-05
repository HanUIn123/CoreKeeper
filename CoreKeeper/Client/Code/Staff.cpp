#include "pch.h"
#include "..\Header\Staff.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Magic.h"

CStaff::CStaff(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	m_tStat.iAttack = 50;

	m_eItemNum = ITEM_STAFF;

	m_bShot = false;
	m_pMagic = nullptr;
}

CStaff::~CStaff()
{
}

HRESULT CStaff::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 3);

	return S_OK;
}

_int CStaff::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_pMagic)
	{
		m_pMagic = dynamic_cast<CMagic*>(Get_GameObject(L"Layer_GameLogic", L"Magic"));
		m_pTransformMagic = dynamic_cast<CTransform*>(m_pMagic->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	}
	else
		m_pColliderCubeCom->Update_Collider(m_pTransformMagic->Get_WorldMatrix());

	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		// Swing(0, 5, 2);
		Shoot(MAGIC);
		m_bActive = true;
		m_bDrop = false;
		if (m_eDir == LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.5f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	}

	if (m_bProjectileAttackSuccess || m_pMagic->Get_ProjectileAttackSuccess())
	{
		m_pMagic->Set_ProjectileAttackSuccess(false);
		m_bProjectileAttackSuccess = false;
		m_bFired = false;
		m_bShot = false;
		m_pMagic->Set_Active(false);
		m_pMagic->Get_Transform()->Set_Pos(vPos.x, -100.f, vPos.z);
	}
	if (m_bFired)
	{
		if (!m_bShot)
		{
			m_bShot = true;
			m_pMagic->Set_Active(true);
			m_pMagic->Set_Direction(m_eDir);
			m_pMagic->Get_Transform()->Set_Pos(vPos.x, 0.5f, vPos.z);
			// m_pMagic->Set_TextureNumber((MATERIAL)(m_eMaterial));
			dynamic_cast<CMagic*>(m_pMagic)->Set_Dir(m_vProjectileDir);
		}
	}



	if (m_bDrop)
	{
		// 아이템 움직임
		CItem::Wave(fTimeDelta);

		Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
			(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

		// 플레이어와 충돌
		if (m_pColliderCom->Check_Collision(pPlayerCollider))
		{
			In_Inventory();
		}
	}
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CStaff::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CStaff::Render_GameObject()
{
	// 카메라를 바라보게 하면서 스케일 유지
	//CItem::Apply_Billboard();  

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());

	if (m_bActive)
	{
		m_pBufferCom->Render_Buffer();
	}

	// 콜라이더 렌더링 추가
	if (m_bActive)
	{
		m_pColliderCom->Render_Collider();
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CStaff::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_StaffAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_StaffTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ItemCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	pComponent = m_pColliderCubeCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_ShootCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ColliderCube", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Animator", pComponent });

	pComponent = m_pShadowBufferCom = dynamic_cast<CShadowTex*>(Engine::Clone_Proto(L"Proto_ShadowTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowCom", pComponent });

	pComponent = m_pShadowTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShadowTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTexture", pComponent });

	pComponent = m_pShadowTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_ShadowTransform", pComponent });

	return S_OK;
}

CStaff* CStaff::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CStaff* pStaff = new CStaff(pGraphicDev);

	if (FAILED(pStaff->Ready_GameObject(vPos)))
	{
		Safe_Release(pStaff);
		MSG_BOX("pStaff Create Failed");
		return nullptr;
	}

	return pStaff;
}

void CStaff::Free()
{
	Engine::CGameObject::Free();
}
