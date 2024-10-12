#include "pch.h"
#include "..\Header\Food.h"
#include "Export_System.h"
#include "Export_Utility.h"

CFood::CFood(LPDIRECT3DDEVICE9 pGraphicDev)
	: CItem(pGraphicDev)
{

}

CFood::~CFood()
{
}

HRESULT CFood::Ready_GameObject(ITEMNUM _eIngredient1, ITEMNUM _eIngredient2, _vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	switch (_eIngredient1)
	{
	case ITEM_BERRY:
		switch (_eIngredient2)
		{
		case ITEM_BERRY:
			m_eItemNum = ITEM_BERRY_BERRY_FOOD;
			m_wItemName = L"원기의 베리 푸딩";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_PEPPER:
			m_eItemNum = ITEM_BERRY_PEPPER_FOOD;
			m_wItemName = L"원기의 후추 랩";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_CARROT:
			m_eItemNum = ITEM_BERRY_CARROT_FOOD;
			m_wItemName = L"바삭한 베리 푸딩";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_MUSHROOM:
			m_eItemNum = ITEM_BERRY_MUSHROOM_FOOD;
			m_wItemName = L"버섯 향 베리 푸딩";
			m_wItemExplain[0] = L"";
			break;
		}
		break;
	case ITEM_PEPPER:
		switch (_eIngredient2)
		{
		case ITEM_PEPPER:
			m_eItemNum = ITEM_PEPPER_PEPPER_FOOD;
			m_wItemName = L"매콤한 후추 랩";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_CARROT:
			m_eItemNum = ITEM_PEPPER_CARROT_FOOD;
			m_wItemName = L"바삭한 후추 랩";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_MUSHROOM:
			m_eItemNum = ITEM_PEPPER_MUSHROOM_FOOD;
			m_wItemName = L"버섯 향 후추 랩";
			m_wItemExplain[0] = L"";
			break;
		}
		break;
	case ITEM_CARROT:
		switch (_eIngredient2)
		{
		case ITEM_CARROT:
			m_eItemNum = ITEM_CARROT_CARROT_FOOD;
			m_wItemName = L"바삭한 돌 디핑 스낵";
			m_wItemExplain[0] = L"";
			break;
		case ITEM_MUSHROOM:
			m_eItemNum = ITEM_CARROT_MUSHROOM_FOOD;
			m_wItemName = L"바삭한 버섯 수프";
			m_wItemExplain[0] = L"";
			break;
		}
		break;
	case ITEM_MUSHROOM:
		m_eItemNum = ITEM_MUSHROOM_MUSHROOM_FOOD;
		m_wItemName = L"버섯 향 버섯 수프";
		m_wItemExplain[0] = L"";
		break;
	default:
		return E_FAIL;
	}

	m_iTextureNumber = m_eItemNum - ITEM_BERRY_BERRY_FOOD;

	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pShadowTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);
	m_pShadowTransformCom->Set_Pos(vPos.x, 0.1f, vPos.z);

	// 원래의 Y 위치 저장
	m_fFirstY = vPos.y;

	return S_OK;
}

_int CFood::Update_GameObject(const _float& fTimeDelta)
{
	if (!m_bUse && !m_bDrop && !m_bActive)
		return 0;

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

void CFood::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CFood::Render_GameObject()
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

HRESULT CFood::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CAnimTex*>(Engine::Clone_Proto(L"Proto_NormalAnimTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_FoodTexture"));
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

CFood* CFood::Create(LPDIRECT3DDEVICE9 pGraphicDev, ITEMNUM _eIngredient1, ITEMNUM _eIngredient2, _vec3 vPos)
{
	CFood* pFood = new CFood(pGraphicDev);

	if (FAILED(pFood->Ready_GameObject(_eIngredient1, _eIngredient2, vPos)))
	{
		Safe_Release(pFood);
		MSG_BOX("pFood Create Failed");
		return nullptr;
	}

	return pFood;
}

void CFood::Free()
{
	Engine::CGameObject::Free();
}
