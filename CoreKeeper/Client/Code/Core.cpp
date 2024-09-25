#include "pch.h"
#include "../Header/Core.h"
#include "Export_System.h"
#include "Export_Utility.h"

CCore::CCore(LPDIRECT3DDEVICE9 pGraphicDev)
	: CBuilding(pGraphicDev)
{
}

CCore::~CCore()
{
}

HRESULT CCore::Ready_GameObject(_float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_strPickedBuildingName = _pickedBuildName;

	m_iBuildingImgNum = iBuildImgNum;

	m_vBuildPosition.x = fX;

	//if (!bReposed)
	//{
	//	m_vBuildPosition.y = fY;
	//	m_vBuildPosition.z = 0;
	//}
	//else
	//{
	//	m_vBuildPosition.y = 0;
	//	m_vBuildPosition.z = fY;
	//}

	m_vBuildPosition.x = fX;
	m_vBuildPosition.y = 0.1f;
	m_vBuildPosition.z = fY;

	m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);

	return S_OK;
}

_int CCore::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_PRIORITY, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CCore::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CCore::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	_matrix matWorld;
	m_pTransformCom->Get_WorldMatrix(&matWorld);

	matWorld._41 = m_vBuildPosition.x;
	matWorld._42 = m_vBuildPosition.y;
	matWorld._43 = m_vBuildPosition.z;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CBuilding::Setup_Material();

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CCore::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SpawnCamp"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	return S_OK;
}

CBuilding* CCore::Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName)
{
	CCore* pCore = new CCore(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(fX ,fY ,bReposed, iBuildImgNum, _pickedBuildName)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CCore::Free()
{
	Engine::CGameObject::Free();
}
