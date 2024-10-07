#include "pch.h"
#include "../Header/AnvilObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

CAnvilObject::CAnvilObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_eMaterial(MATERIAL_END), m_iTextureNum(0)
{
}

CAnvilObject::~CAnvilObject()
{
}

HRESULT CAnvilObject::Ready_GameObject(_vec3 vPos, MATERIAL _eMaterial)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	m_eMaterial = _eMaterial;
	m_iTextureNum = m_eMaterial;

	return S_OK;
}

_int CAnvilObject::Update_GameObject(const _float& fTimeDelta)
{
	if (Check_Interaction())
	{
		Interaction();
	}
	else if (!Check_Interaction())
	{
		if (m_bCollision)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (pPlayer->Get_CraftUI())
			{
				pPlayer->Set_Craft();

				pPlayer->Set_Inventory();

				m_bCollision = false;
			}
		}
	}
	Add_RenderGroup(RENDER_ALPHA, this);

	return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CAnvilObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CAnvilObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture(m_iTextureNum);

	m_pBufferCom->Render_Buffer();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CAnvilObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		pPlayer->Set_Craft(TABLE_ANVIL, m_eMaterial);

		pPlayer->Set_Inventory();

		m_bCollision = true;
	}
}

HRESULT CAnvilObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_AnvilTexture"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

	pComponent = m_pTransformCom = dynamic_cast<CTransform*>(Engine::Clone_Proto(L"Proto_Transform"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

	pComponent = m_pCalculCom = dynamic_cast<CCalculator*>(Engine::Clone_Proto(L"Proto_Calculator"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Calculator", pComponent });

	pComponent = m_pColliderCom = dynamic_cast<CColliderCube*>(Engine::Clone_Proto(L"Proto_NormalCubeCollider"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Collider", pComponent });


	return S_OK;
}

CAnvilObject* CAnvilObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, MATERIAL _eMaterial)
{
	CAnvilObject* pCore = new CAnvilObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos, _eMaterial)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CAnvilObject::Free()
{
	Engine::CGameObject::Free();
}
