#include "pch.h"
#include "..\Header\Bow.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Arrow.h"

_int	CBow::m_iArrowNum = 0;

CBow::CBow(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	m_tStat.iAttack = 20;
	m_eItemNum = ITEM_BOW;
	m_bShot = false;
	m_pArrow = nullptr;
}

CBow::~CBow()
{
}

HRESULT CBow::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_tStat.iAttack = 20;

	m_wItemName = L"철제 활";
	m_wItemExplain[0] = L"물리 피해 +20";
	m_wItemExplain[1] = L"튼튼한 활로, 멀리 있는 적을 쏠 수 있습니다.";

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 3);

	return S_OK;
}

_int CBow::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bUse && !m_bDrop && !m_bActive)
		return 0;

	if (!m_pArrow)
	{
		CScene* pScene = Engine::Get_Scene();
		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		vPos.y += 2.f;
		CGameObject* pProjectile = CArrow::Create(m_pGraphicDev);
		NULL_CHECK_RETURN(pProjectile, 0);
		m_pArrow = dynamic_cast<CArrow*>(pProjectile);
		m_strArrow = L"Arrow_" + std::to_wstring(m_iArrowNum++);
		FAILED_CHECK_RETURN(pScene->Create_GameObject(L"Layer_GameLogic", pProjectile, m_strArrow.c_str()), 0);
		m_pTransformArrow = dynamic_cast<CTransform*>(m_pArrow->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	}
	else
		m_pColliderCubeCom->Update_Collider(m_pTransformArrow->Get_WorldMatrix());
	
	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		Shoot(ARROW);
		m_bActive = true;
		m_bDrop = false;
		if(m_eDir == LEFT)
			m_pTransformCom->Set_Scale(-1.5f, 1.5f, 1.5f);
		else
			m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	}

	if (m_bShoot)
	{
		if (!m_bShot)
		{
			m_bShot = true;
			m_pArrow->Set_Active(true);
			m_pArrow->Set_Direction(m_eDir);
			m_pArrow->Get_Transform()->Set_Pos(vPos.x, 0.25f, vPos.z);
			// m_pArrow->Set_TextureNumber((MATERIAL)(m_eMaterial + 1));
			m_pArrow->Set_TextureNumber((MATERIAL)1);
			dynamic_cast<CArrow*>(m_pArrow)->Set_Dir(m_vProjectileDir);
		}
		else
		{
			if (m_bProjectileAttackSuccess || m_pArrow->Get_ProjectileAttackSuccess())
			{
				m_pArrow->Set_Active(false);
				m_pArrow->Get_Transform()->Set_Pos(vPos.x, 100.f, vPos.z);
			}
		}
	}
	else
	{
		m_pArrow->Set_ProjectileAttackSuccess(false);
		m_bProjectileAttackSuccess = false;
		m_bShot = false;
		m_pArrow->Set_Active(false);
		m_pArrow->Get_Transform()->Set_Pos(vPos.x, 100.f, vPos.z);
	}

	if (m_bDrop)
	{
		// 아이템 움직임
		Wave(fTimeDelta);

		// 플레이어와 충돌
		Check_Collision();
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CBow::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBow::Render_GameObject()
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
		m_pColliderCubeCom->Render_Collider();
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBow::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_BowAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BowTexture"));
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

CBow* CBow::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CBow* pBow = new CBow(pGraphicDev);

	if (FAILED(pBow->Ready_GameObject(vPos)))
	{
		Safe_Release(pBow);
		MSG_BOX("pBow Create Failed");
		return nullptr;
	}

	return pBow;
}


void CBow::Free()
{
	Engine::CGameObject::Free();
}
