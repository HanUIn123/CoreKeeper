#include "pch.h"
#include "..\Header\Spawner.h"
#include "Export_System.h"
#include "Export_Utility.h"

CSpawner::CSpawner(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{
}

CSpawner::~CSpawner()
{
}

HRESULT CSpawner::Ready_GameObject(ITEMNUM _eItemNum, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_eItemNum = _eItemNum;

	switch (m_eItemNum)
	{
	case ITEM_PLAYER_SPAWNER:
		m_iTextureNumber = 0;
		m_wItemName = L"소환 우상";
		m_wItemExplain[0] = L"코어로 다시 순간이동시켜주며, 그 과정에서 소모됩니다.";
		break;
	case ITEM_MAL_SPAWNER:
		m_iTextureNumber = 1;
		m_wItemName = L"타락한 주술사의 해골";
		m_wItemExplain[0] = L"고대 기도문을 읊조리는 다시 이어붙인 크리스털 해골입니다. 이 물건은 왕좌의 방 인장 위로 두어야 한다는 욕구가 듭니다.";
		break;
	case ITEM_AZEOS_SPAWNER:
		m_iTextureNumber = 2;
		m_wItemName = L"반짝이고 깜빡이는 거대한 물건";
		m_wItemExplain[0] = L"크리스털 같은 계란 모양의 물건입니다. 먼 거리에서도 눈길을 사로잡습니다.";
		break;
	}
	
	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	return S_OK;
}

_int CSpawner::Update_GameObject(const _float& fTimeDelta)
{
	m_pAnimatorCom->Update_Animation();

	_vec3 vPos;
	m_pTransformCom->Get_Info(INFO_POS, &vPos);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	if (m_bUse)
	{
		m_bActive = true;
		m_bDrop = false;
		m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	}

	if (m_bDrop)
	{
		// 아이템 움직임
		Wave(fTimeDelta);

		// 플레이어와 충돌
		Check_Collision();
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CSpawner::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CSpawner::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pTextureCom->Set_Texture(m_iTextureNumber);

	m_pBufferCom->Set_Index(0);

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

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CSpawner::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_NormalAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_SpawnerTexture"));
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

CSpawner* CSpawner::Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eItemNum, _vec3 vPos)
{
	CSpawner* pSpawner = new CSpawner(pGraphicDev);

	if (FAILED(pSpawner->Ready_GameObject(_eItemNum, vPos)))
	{
		Safe_Release(pSpawner);
		MSG_BOX("pSpawner Create Failed");
		return nullptr;
	}

	return pSpawner;
}

void CSpawner::Free()
{
	Engine::CGameObject::Free();
}
