#include "pch.h"
#include "..\Header\Torch.h"
#include "Export_System.h"
#include "Export_Utility.h"

CTorch::CTorch(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
	ZeroMemory(&m_tStat, sizeof(STAT));

	m_eItemNum = ITEM_TORCH;
}

CTorch::~CTorch()
{
}

HRESULT CTorch::Ready_GameObject()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Scale(1.0f, 1.0f, 1.0f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(0.f, 0.7f, 0.f);

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);
	// 원래의 Y 위치 저장
	m_fFirstY = 0.7f;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 5, 3);

	return S_OK;
}

_int CTorch::Update_GameObject(const _float& fTimeDelta)
{
	SetUp_Light(); // 조명 설정

	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	}

	if (m_bDrop)
	{
		// 아이템 움직임
		CItem::Wave(fTimeDelta);

		Engine::CCollider* pPlayerCollider = dynamic_cast<Engine::CCollider*>
			(Engine::Get_Component(ID_DYNAMIC, L"Layer_GameLogic", L"Player", L"Com_Collider"));

		// 플레이어와 충돌
		if (m_pColliderCom->Check_Collision(pPlayerCollider))
		{
			Engine::CInventory* pPlayerInventory = dynamic_cast<Engine::CInventory*>
				(Engine::Get_Component(ID_STATIC, L"Layer_GameLogic", L"Player", L"Com_Inventory"));
		
			// 인벤토리에 들어갔다
			if (pPlayerInventory->Add_Item(this))
			{
				m_bActive = false;
				m_bDrop = false;
			}
		}
	}
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CTorch::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CTorch::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 카메라를 바라보게 하면서 스케일 유지
	//CItem::Apply_Billboard();  

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pShadowTransformCom->Get_WorldMatrix());

	m_pShadowTextureCom->Set_Texture(1);

	if (m_bActive && !m_bUse)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTorch::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_TorchAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_TorchTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CCollider*>(Engine::Clone_Proto(L"Proto_ItemCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

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

void CTorch::SetUp_Light()
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));

	light.Type = D3DLIGHT_POINT; // 포인트 조명
	light.Diffuse = { 1.f, 1.f, 1.f, 1.f }; // 확산 색상
	light.Specular = { 1.f, 1.f, 1.f, 1.f }; // 반사 색상
	light.Ambient = { 1.f, 1.f, 1.f, 1.f }; // 주변광

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);

	light.Position = vPos; // 횃불의 위치
	light.Range = 4.0f; // 조명의 범위
	light.Falloff = 1.f; // 감쇠
	light.Attenuation0 = 1.0f; // 감쇠 계수
	light.Attenuation1 = 0.01f;
	light.Attenuation2 = 0.0f;

	m_pGraphicDev->SetLight(0, &light); // 조명 설정

	if (m_bUse)
	{
		m_pGraphicDev->LightEnable(0, TRUE); // 조명 활성화
	}
	else
	{
		m_pGraphicDev->LightEnable(0, FALSE); // 조명 비활성화
	}
}

CTorch* CTorch::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTorch* pSeed = new CTorch(pGraphicDev);

	if (FAILED(pSeed->Ready_GameObject()))
	{
		Safe_Release(pSeed);
		MSG_BOX("pSeed Create Failed");
		return nullptr;
	}

	return pSeed;
}

void CTorch::Free()
{
	Engine::CGameObject::Free();
}
