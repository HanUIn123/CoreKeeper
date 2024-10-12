#include "pch.h"
#include "../Header/BossSpawnPoint.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Core.h"

CBossSpawnPoint::CBossSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bActive(false), m_bInFrustum(false)
{
	m_iLightNum = g_iLightNum++;
}

CBossSpawnPoint::~CBossSpawnPoint()
{
}

HRESULT CBossSpawnPoint::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_vSpawnPos = vPos;

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CBossSpawnPoint::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_bInFrustum = m_pCalculCom->In_Frustum(m_pTransformCom);

	SetUp_Light(); // 조명 설정

	if (!m_bInFrustum)
	{
		return 0;
	}

	Add_RenderGroup(RENDER_ALPHA, this);
	return iExit;
}

void CBossSpawnPoint::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBossSpawnPoint::Render_GameObject()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pColliderCom->Render_Collider();

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

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_SpawnPointCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CBossSpawnPoint::SetUp_Light()
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = { 3.f, 3.f, 3.f, 1.f }; // 확산 색상
	light.Specular = { 3.f, 3.f, 3.f, 1.f }; // 반사 색상
	light.Ambient = { 3.f, 3.f, 3.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = { vPos.x, vPos.y + 10.f, vPos.z };

	// 아래쪽을 향하는 스포트라이트
	_vec3 vDir = { 0.0f, -1.0f, 0.0f };
	light.Direction = vDir;

	light.Range = 50.f * 10.f; // 조명의 범위
	light.Falloff = 1.0f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	// 스포트라이트의 내부 및 외부 각도 설정
	light.Theta = D3DXToRadian(20.0f); // 내부 각도 (작은 값일수록 집중된 조명)
	light.Phi = D3DXToRadian(90.0f); // 외부 각도 (큰 값일수록 퍼지는 조명)

	m_pGraphicDev->SetLight(m_iLightNum, &light);
	m_pGraphicDev->LightEnable(m_iLightNum, m_bInFrustum);
}

CBossSpawnPoint* CBossSpawnPoint::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CBossSpawnPoint* pCore = new CBossSpawnPoint(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
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
