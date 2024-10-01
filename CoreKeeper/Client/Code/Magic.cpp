#include "pch.h"
#include "..\Header\Magic.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Terrain.h"

CMagic::CMagic(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	m_fSpeed = 32.f;
	m_bActive = false;
	m_fTime = 0.f;
}

CMagic::~CMagic()
{
}

HRESULT CMagic::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(0.25f, 0.25f, 0.25f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	//m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	//m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);
	//m_vDir = vDir;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 3);

	return S_OK;
}

_int CMagic::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_pTerrain)
		m_pTerrain = dynamic_cast<CTerrain*>(Engine::Get_GameObject(L"Layer_Environment", L"Terrain"));

	if (m_bActive)
	{
		m_fTime += fTimeDelta;

		if (m_eDir == LEFT)
			m_pTransformCom->Set_Scale(-0.5f, 0.5f, 0.5f);
		else
			m_pTransformCom->Set_Scale(0.5f, 0.5f, 0.5f);

		if (m_fTime >= 0.5f || m_iSpeedWeight == 0)
		{
			m_fTime = 0;
			m_iSpeedWeight = 0;
			m_bProjectileAttackSuccess = true;
			m_bActive = false;
		}
		Set_Stop(&m_vDir, 1.f);
		m_pTransformCom->Move_Pos(&m_vDir, fTimeDelta, m_fSpeed * m_iSpeedWeight);
		
		m_pAnimatorCom->Set_CurState(SHOOT, 0, 6, 4);

		_vec3 vPos;
		m_pTransformCom->Get_Info(INFO_POS, &vPos);
		m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

		m_pAnimatorCom->Update_Animation();
		Add_RenderGroup(RENDER_ALPHA, this);
	}
	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CMagic::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CMagic::Render_GameObject()
{
	// 카메라를 바라보게 하면서 스케일 유지
	//CItem::Apply_Billboard();  
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(m_pAnimatorCom->Get_MotionIndex());

	if (m_bActive)
	{
		m_pBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CMagic::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_MagicAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_MagicTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

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

CMagic* CMagic::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMagic* pArrow = new CMagic(pGraphicDev);

	if (FAILED(pArrow->Ready_GameObject()))
	{
		Safe_Release(pArrow);
		MSG_BOX("pArrow Create Failed");
		return nullptr;
	}

	return pArrow;
}


void CMagic::Free()
{
	Engine::CGameObject::Free();
}
