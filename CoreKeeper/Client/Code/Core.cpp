#include "pch.h"
#include "../Header/Core.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

CCore::CCore(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iRange(0)
{
	m_iLightNum = g_iLightNum++;
	ZeroMemory(&m_bActiveCore, sizeof(bool) * 3);

	//m_bActiveCore[0] = true;
	//m_bActiveCore[1] = true;
	//m_bActiveCore[2] = true;
}

CCore::~CCore()
{
}

HRESULT CCore::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CCore::Update_GameObject(const _float& fTimeDelta)
{
	if (Check_Interaction())
	{
		Interaction();
	}

	m_iRange = (m_bActiveCore[0] + m_bActiveCore[1] + m_bActiveCore[2]) * 10.f;

	SetUp_Light(); // 조명 설정

	Add_RenderGroup(RENDER_ALPHA, this);

	Add_RenderGroup(RENDER_MAP, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CCore::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CCore::Render_GameObject()
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pColliderCom->Render_Collider();

	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CCore::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		if (m_bActiveCore[0] && m_bActiveCore[1] && m_bActiveCore[2])
		{
			//_vec2 LTpos(0, 570);
			//_vec2 RBpos(WINCX, 570);

			//Engine::Render_Font_Custom(L"Font_Default", L"에너지가 필요한 것 같아", &LTpos, &RBpos, DT_CENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
		else
		{
			//_vec3 vPos;
			//m_pTransformCom->Get_Info(INFO_POS, &vPos);

			//_vec2 LTpos(0, 300);
			//_vec2 RBpos(WINCX, 400);

			//Engine::Render_Font_Custom(L"Font_Default", L"에너지가 필요한 것 같아", &LTpos, &RBpos, DT_CENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
			//Engine::Render_Font_Custom(L"Font_Default", L"전원이 차단된 것 같아", &LTpos, &RBpos, DT_CENTER, D3DXCOLOR(1.f, 1.f, 1.f, 1.f));
		}
	}
}

HRESULT CCore::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_CoreTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_CoreTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_CoreCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });

	return S_OK;
}

void CCore::SetUp_Light()
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_SPOT;
	light.Diffuse =		{ 0.4f + m_iRange * 0.1f, 0.4f + m_iRange * 0.1f, 0.6f + m_iRange * 0.1f, 1.f }; // 확산 색상
	light.Specular =	{ 0.4f + m_iRange * 0.1f, 0.4f + m_iRange * 0.1f, 0.6f + m_iRange * 0.1f, 1.f }; // 반사 색상
	light.Ambient =		{ 0.4f + m_iRange * 0.1f, 0.4f + m_iRange * 0.1f, 0.6f + m_iRange * 0.1f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	light.Position = { vPos.x, vPos.y + 10.f, vPos.z };
	 
	// 아래쪽을 향하는 스포트라이트
	_vec3 vDir = { 0.0f, -1.0f, 0.0f };
	light.Direction = vDir;

	light.Range = (5.f + m_iRange) * 20.f; // 조명의 범위
	light.Falloff = 1.0f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	// 스포트라이트의 내부 및 외부 각도 설정
	light.Theta = D3DXToRadian(20.0f); // 내부 각도 (작은 값일수록 집중된 조명)
	light.Phi = D3DXToRadian(60.0f + m_iRange); // 외부 각도 (큰 값일수록 퍼지는 조명)

	m_pGraphicDev->SetLight(m_iLightNum, &light);
	m_pGraphicDev->LightEnable(m_iLightNum, TRUE);
}

CCore* CCore::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CCore* pCore = new CCore(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
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
