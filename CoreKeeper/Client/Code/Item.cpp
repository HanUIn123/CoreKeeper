#include "pch.h"
#include "..\Header\Item.h"
#include "Export_System.h"
#include "Export_Utility.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iTextureNumber(0), m_fFirstY(0.f), m_fTimeAcc(0.f), m_bActive(true)
{
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->m_vScale = { 0.2f, 0.2f, 0.2f };
	m_pShadowTransformCom->m_vScale = { 0.2f, 0.2f, 0.2f };

	m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, m_pTransformCom->m_vInfo->y + 0.7f, m_pTransformCom->m_vInfo->z);
	m_pShadowTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, 0.1f, m_pTransformCom->m_vInfo->z);

	// 원래의 Y 위치 저장
	m_fFirstY = m_pTransformCom->m_vInfo->y + 0.7f;

	return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
	// 둥실거리는 효과를 위한 Y 위치 변동
	const _float fAmplitude = 0.2f;  // 둥실거리는 높이
	const _float fFrequency = 3.0f;  // 둥실거리는 속도

	m_fTimeAcc += fTimeDelta;

	float fNewy = m_fFirstY + fAmplitude * sinf(m_fTimeAcc * fFrequency);
	m_pTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, fNewy, m_pTransformCom->m_vInfo->z);

	m_pShadowTransformCom->Set_Pos(m_pTransformCom->m_vInfo->x, 0.1f, m_pTransformCom->m_vInfo->z);

	Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

	// 충돌하면 FALSE, 충돌안하면 TRUE
	m_bActive = !(m_pColliderCom->Check_Collision(pPlayerCollider));
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CItem::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CItem::Render_GameObject()
{
	// 카메라를 바라보게 하면서 스케일 유지
	//Apply_Billboard();  

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);
	
	if (m_bActive)
	{
		m_pBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bActive)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pColliderCom->Render_Collider();

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CItem::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(Engine::Clone_Proto(L"Proto_RcTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ItemTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ItemCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

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

void CItem::Apply_Billboard()
{
	D3DXMATRIX matWorld, matView, matBill, matScale;

	m_pTransformCom->Get_WorldMatrix(&matWorld);

	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);

	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);

	// 스케일 행렬을 따로 계산
	D3DXMatrixScaling(&matScale, m_pTransformCom->m_vScale.x, m_pTransformCom->m_vScale.y, m_pTransformCom->m_vScale.z);
	
	D3DXMATRIX matInverseScale;
	D3DXMatrixInverse(&matInverseScale, 0, &matScale);

	// 최종 월드 행렬: 스케일 적용 후 빌보드 회전 적용
	D3DXMATRIX matFinal = matScale * matBill * matInverseScale * matWorld;

	m_pTransformCom->Set_WorldMatrix(&matFinal);
}

CItem* CItem::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CItem* pPlayer = new CItem(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CItem::Free()
{
	Engine::CGameObject::Free();
}
