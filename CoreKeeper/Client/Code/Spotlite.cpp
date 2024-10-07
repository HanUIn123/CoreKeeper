#include "pch.h"
#include "../Header/Spotlite.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSpotlite::CSpotlite(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_fRange(0.f)
{
	m_iLightNum = g_iLightNum++;
}

CSpotlite::~CSpotlite()
{
}

HRESULT CSpotlite::Ready_GameObject(float _fRange, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_fRange = _fRange;

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CSpotlite::Update_GameObject(const _float& fTimeDelta)
{
	SetUp_Light();

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSpotlite::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CSpotlite::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	//m_pLightBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
}

HRESULT CSpotlite::Add_Component()
{
	CComponent* pComponent = NULL;

	//pComponent = m_pLightBufferCom = dynamic_cast<CLightBuffer*>(Engine::Clone_Proto(L"Proto_LightBuffer"));
	//NULL_CHECK_RETURN(pComponent, E_FAIL);
	//m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	return S_OK;
}

void CSpotlite::SetUp_Light()
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = vPos;

	// 아래쪽을 향하는 스포트라이트
	_vec3 vDir = { 0.0f, -1.0f, 0.0f };
	light.Direction = vDir;

	light.Range = m_fRange * 10.f; // 조명의 범위
	light.Falloff = 1.0f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	// 스포트라이트의 내부 및 외부 각도 설정
	light.Theta = D3DXToRadian(20.0f); // 내부 각도 (작은 값일수록 집중된 조명)
	light.Phi = D3DXToRadian(40.0f); // 외부 각도 (큰 값일수록 퍼지는 조명)

	m_pGraphicDev->SetLight(m_iLightNum, &light);
	m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
}

CSpotlite* CSpotlite::Create(LPDIRECT3DDEVICE9 pGraphicDev, float _fRange, _vec3 vPos)
{
	CSpotlite* pSpotlite = new CSpotlite(pGraphicDev);

	if (FAILED(pSpotlite->Ready_GameObject(_fRange, vPos)))
	{
		Safe_Release(pSpotlite);
		MSG_BOX("pSpotlite Create Failed");
		return nullptr;
	}

	return pSpotlite;
}

void CSpotlite::Free()
{
	Engine::CGameObject::Free();
}
