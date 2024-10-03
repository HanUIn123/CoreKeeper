#include "pch.h"
#include "../Header/Plant.h"
#include "Export_System.h"
#include "Export_Utility.h"

CPlant::CPlant(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev)
{
}

CPlant::~CPlant()
{
}

HRESULT CPlant::Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CPlant::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CPlant::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CPlant::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CPlant::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_PlantTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pAnimBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_PlantAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pAnimatorCom = dynamic_cast<CAnimator*>(Engine::Clone_Proto(L"Proto_Animator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Animator", pComponent });

	return S_OK;
}

CPlant* CPlant::Create(LPDIRECT3DDEVICE9 pGraphicDev,ITEMNUM _eItemNum, _vec3 vPos)
{
	CPlant* pPlant = new CPlant(pGraphicDev);

	if (FAILED(pPlant->Ready_GameObject(_eItemNum, vPos)))
	{
		Safe_Release(pPlant);
		MSG_BOX("pPlant Create Failed");
		return nullptr;
	}

	return pPlant;
}

void CPlant::Free()
{
	Engine::CGameObject::Free();
}
