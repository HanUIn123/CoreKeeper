#include "pch.h"
#include "../Header/BoxObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/PetItem.h"
#include "../Header/Assistance.h"
#include "../Header/Bar.h"

CBoxObject::CBoxObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CObject(pGraphicDev), m_iTextureNum(0), m_bSpecial(false)
{
}

CBoxObject::~CBoxObject()
{
}

HRESULT CBoxObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CBoxObject::Update_GameObject(const _float& fTimeDelta)
{
	int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (!m_pCalculCom->In_Frustum(m_pTransformCom))
		return 0;

	if (Check_Interaction())
	{
		Interaction();
	}
	else if (!Check_Interaction())
	{
		if (m_bCollision)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (pPlayer->Get_ChestInv() && pPlayer->Get_Inventory())
			{
				pPlayer->Set_ChestInventory();

				//pPlayer->Set_Inventory();
			}

			m_bCollision = false;
		}
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CBoxObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CBoxObject::Render_GameObject()
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

void CBoxObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		pPlayer->Set_ChestInventory(m_pInventoryCom);

		//pPlayer->Set_Inventory();

		m_bCollision = true;
	}
}

void CBoxObject::SetUp_Item(CScene* _pScene)
{
    CItem* pGameObject(nullptr);

    pGameObject = CPetItem::Create(m_pGraphicDev);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Special_Item_Pet");
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    pGameObject = CAssistance::Create(m_pGraphicDev, ASSISTANCE_FEATHER);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Special_Item_Feather");
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

	pGameObject = CBar::Create(m_pGraphicDev, MATERIAL_COPPER);
	pGameObject->Add_Count(20);
	m_pInventoryCom->Add_Item(pGameObject);
	m_vecItemName.push_back(L"Special_Item_Feather");
	FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

	pGameObject = CBar::Create(m_pGraphicDev, MATERIAL_IRON);
	pGameObject->Add_Count(40);
	m_pInventoryCom->Add_Item(pGameObject);
	m_vecItemName.push_back(L"Special_Item_Feather");
	FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
}

HRESULT CBoxObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_BoxTexture"));
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

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_ChestInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

CBoxObject* CBoxObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CBoxObject* pCore = new CBoxObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CBoxObject::Free()
{
	Engine::CGameObject::Free();
}
