#include "pch.h"
#include "../Header/BossSpawnPoint.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Core.h"

CBossSpawnPoint::CBossSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bActive(false)
{
}

CBossSpawnPoint::~CBossSpawnPoint()
{
}

HRESULT CBossSpawnPoint::Ready_GameObject(_vec3 vPos, _int _iTypeNum, const wstring _pickedSPName)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vSpawnPos = vPos;

	m_iSpawnTextureNumber = _iTypeNum;

	m_strPickedObjectName = _pickedSPName;

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CBossSpawnPoint::Update_GameObject(const _float& fTimeDelta)
{
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CBossSpawnPoint::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBossSpawnPoint::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iSpawnTextureNumber);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (m_bActive)
	{
		m_pEmissiveTextureCom->Set_Texture(m_iSpawnTextureNumber);
		m_pEmissiveBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBossSpawnPoint::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_BossSpawnPointTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BossSpawnPointTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pEmissiveBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_BossSpawnPointTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_EmissiveBuffer", pComponent });

	pComponent = m_pEmissiveTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BossSpawnPointEmissiveTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_EmissiveTexture", pComponent });

	return S_OK;
}

CBossSpawnPoint* CBossSpawnPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum, const wstring _pickedSPName)
{
	CBossSpawnPoint* pCore = new CBossSpawnPoint(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos, _iTypeNum, _pickedSPName)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CBossSpawnPoint::Free()
{
	Engine::CGameObject::Free();
}
