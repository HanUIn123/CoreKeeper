#include "pch.h"
#include "..\Header\Shadow.h"
#include "Export_System.h"
#include "Export_Utility.h"

CShadow::CShadow(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev), m_iTextureNumber(0)
{
}

CShadow::~CShadow()
{
}

HRESULT CShadow::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pItem = dynamic_cast<CItem*>(Engine::CManagement::GetInstance()->Get_GameObject(L"Layer_GameLogic", L"Item"));

	m_pTransformCom->m_vScale = { 0.15f, 0.15f, 0.15f };
	
	return S_OK;
}

_int CShadow::Update_GameObject(const _float& fTimeDelta)
{
	Engine::CTransform* pItemTransform = dynamic_cast<Engine::CTransform*>
		(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Item", L"Com_Transform"));
	NULL_CHECK_RETURN(pItemTransform, -1);

	_vec3		vItemPos;
	pItemTransform->Get_Info(INFO_POS, &vItemPos);

	m_pTransformCom->Set_Pos(pItemTransform->m_vInfo->x, pItemTransform->m_vInfo->y, pItemTransform->m_vInfo->z);

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CShadow::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CShadow::Render_GameObject()
{
	// 카메라를 바라보게 하면서 스케일 유지
	//Apply_Billboard();  

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture(1);

	m_pBufferCom->Render_Buffer();
	

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CShadow::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CShadowTex*>(Engine::Clone_Proto(L"Proto_ShadowTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_ShadowTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CShadow::Apply_Billboard()
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

CShadow* CShadow::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CShadow* pPlayer = new CShadow(pGraphicDev);

	if (FAILED(pPlayer->Ready_GameObject()))
	{
		Safe_Release(pPlayer);
		MSG_BOX("pPlayer Create Failed");
		return nullptr;
	}

	return pPlayer;
}

void CShadow::Free()
{
	Engine::CGameObject::Free();
}
