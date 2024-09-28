#include "pch.h"
#include "..\Header\Bag.h"
#include "Export_System.h"
#include "Export_Utility.h"

CBag::CBag(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev), m_iAddSlotCount(0)
{
	 

	m_eItemNum = ITEM_BAG;
}

CBag::~CBag()
{
}

HRESULT CBag::Ready_GameObject(MATERIAL _eMaterial, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eMaterial = _eMaterial;
	m_iTextureNumber = m_eMaterial;

	m_iAddSlotCount = 5 * (m_eMaterial + 1);

	m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	m_pAnimatorCom->Set_CurState(IDLE, 0, 0, 3);

	return S_OK;
}

_int CBag::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		Swing(0, 5, 2);

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
			In_Inventory();
		}
	}
	
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CBag::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBag::Render_GameObject()
{
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

	if (m_bDrop)
	{
		m_pShadowBufferCom->Render_Buffer();
	}

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CBag::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_BagAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BagTex"));
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

CBag* CBag::Create(LPDIRECT3DDEVICE9 pGraphicDev, MATERIAL _eMaterial, _vec3 vPos)
{
	CBag* pBag = new CBag(pGraphicDev);

	if (FAILED(pBag->Ready_GameObject(_eMaterial, vPos)))
	{
		Safe_Release(pBag);
		MSG_BOX("pBag Create Failed");
		return nullptr;
	}

	return pBag;
}


void CBag::Free()
{
	Engine::CGameObject::Free();
}
