#include "pch.h"
#include "../Header/GravestoneObject.h"
#include "Export_System.h"
#include "Export_Utility.h"
#include "..\Header\Player.h"

#include "..\Header\Include.h"
#include "..\Header\Stage.h"

int CGravestoneObject::m_bDropItemNumber = 0;

CGravestoneObject::CGravestoneObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CObject(pGraphicDev), m_iItemNameNum(0), m_bSelf(false), m_bActive(true), m_bCheck(false)
{
}

CGravestoneObject::~CGravestoneObject()
{
}

HRESULT CGravestoneObject::Ready_GameObject(_vec3 vPos)
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransformCom->Set_Pos(vPos.x, vPos.y, vPos.z);

	return S_OK;
}

_int CGravestoneObject::Update_GameObject(const _float& fTimeDelta)
{
    int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    if (!m_pCalculCom->In_Frustum(m_pTransformCom))
        return 0;

    if (!m_bActive)
        return 0;

    if (m_bCheck && m_bActive && m_pInventoryCom->Empty())
    {
        m_bActive = false;

        _vec3 vPos;
        m_pTransformCom->Get_Info(INFO_POS, &vPos);

        CScene* pScene = Engine::Get_Scene();
        CStage* pStage = dynamic_cast<CStage*>(pScene);
        CItem* pGameObject = nullptr;

        vPos.y = 0.6f;

        pGameObject = CGravestone::Create(m_pGraphicDev, vPos);
        m_DropItemName = L"m_DropItemName" + std::to_wstring(m_bDropItemNumber++);
        
        if (pGameObject)
        {
            pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_DropItemName.c_str());
            pGameObject->Set_Active(true);
            pGameObject->Set_Drop(true);
        }  
    }

	// 플레이어와 충돌했으면 상호작용해라.
	if (!m_bSelf && Check_Interaction())
	{
		Interaction();
	}
	else if (!Check_Interaction())
	{
		if (m_bCollision)
		{
			CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

			if (pPlayer->Get_GraveUI() && pPlayer->Get_Inventory())
			{
				pPlayer->Set_GraveInventory();
			}

            m_bCollision = false;
		}
	}

	Add_RenderGroup(RENDER_ALPHA, this);

	return iExit;
}

void CGravestoneObject::LateUpdate_GameObject()
{
	Engine::CGameObject::LateUpdate_GameObject();
}

void CGravestoneObject::Render_GameObject()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_WorldMatrix());

	m_pColliderCom->Update_Collider(m_pTransformCom->Get_WorldMatrix());

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	FAILED_CHECK_RETURN(Setup_Material(), );

	m_pTextureCom->Set_Texture();

	m_pBufferCom->Render_Buffer();

	m_pColliderCom->Render_Collider();

	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CGravestoneObject::Interaction()
{
	if (Engine::Key_Down(DIK_E))
	{
        m_bCheck = true;

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

		pPlayer->Set_GraveInventory(m_pInventoryCom);

		m_bCollision = true;
	}
}

HRESULT CGravestoneObject::Add_Component()
{
	CComponent* pComponent = NULL;

	pComponent = m_pBufferCom = dynamic_cast<CObjectTex*>(Engine::Clone_Proto(L"Proto_ObjectTex"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

	pComponent = m_pTextureCom = dynamic_cast<CTexture*>(Engine::Clone_Proto(L"Proto_GravestoneTexture"));
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

	pComponent = m_pInventoryCom = dynamic_cast<CInventory*>(Engine::Clone_Proto(L"Proto_GravestoneInventory"));
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent[ID_STATIC].insert({ L"Com_Inventory", pComponent });

	return S_OK;
}

void CGravestoneObject::SetUp_Item(CScene* _pScene)
{
	CItem* pGameObject(nullptr);

    // 일기장
    pGameObject = CDiary::Create(m_pGraphicDev);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 나무
    pGameObject = CWood::Create(m_pGraphicDev);
    pGameObject->Add_Count(150);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 슬라임 코어
    pGameObject = CStatueCore::Create(m_pGraphicDev, ITEM_SLIME_CORE);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 애벌레 코어
    pGameObject = CStatueCore::Create(m_pGraphicDev, ITEM_LARVA_CORE);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 원기베리
    pGameObject = CSeed::Create(m_pGraphicDev, ITEM_BERRY_SEED);
    pGameObject->Add_Count(14);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 폭탄 후추
    pGameObject = CSeed::Create(m_pGraphicDev, ITEM_PEPPER_SEED);
    pGameObject->Add_Count(14);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 돌당근
    pGameObject = CSeed::Create(m_pGraphicDev, ITEM_CARROT_SEED);
    pGameObject->Add_Count(14);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 섬유질
    pGameObject = CSeed::Create(m_pGraphicDev, ITEM_FIBER_SEED);
    pGameObject->Add_Count(14);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 버섯
    pGameObject = CIngredient::Create(m_pGraphicDev, ITEM_END);
    pGameObject->Add_Count(14);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 해골조각
    pGameObject = CPiece::Create(m_pGraphicDev, ITEM_SKULL_PIECE);
    pGameObject->Add_Count(1);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 슬라임 점액
    pGameObject = CMucus::Create(m_pGraphicDev);
    pGameObject->Add_Count(56);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 구리 광석
    pGameObject = COre::Create(m_pGraphicDev, MATERIAL_COPPER);
    pGameObject->Add_Count(23);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 철광석
    pGameObject = COre::Create(m_pGraphicDev, MATERIAL_IRON);
    pGameObject->Add_Count(6);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 흙벽
    pGameObject = CWallPiece::Create(m_pGraphicDev, ITEM_DIRTWALL);
    pGameObject->Add_Count(16);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );

    // 돌벽
    pGameObject = CWallPiece::Create(m_pGraphicDev, ITEM_STONEWALL);
    pGameObject->Add_Count(7);
    m_pInventoryCom->Add_Item(pGameObject);
    m_vecItemName.push_back(L"Woodcutter's_Item" + std::to_wstring(m_iItemNameNum++));
    FAILED_CHECK_RETURN(_pScene->Create_GameObject(L"Layer_GameLogic", pGameObject, m_vecItemName.back().c_str()), );
}

CGravestoneObject* CGravestoneObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos)
{
	CGravestoneObject* pCore = new CGravestoneObject(pGraphicDev);

	if (FAILED(pCore->Ready_GameObject(vPos)))
	{
		Safe_Release(pCore);
		MSG_BOX("pCore Create Failed");
		return nullptr;
	}

	return pCore;
}

void CGravestoneObject::Free()
{
	Engine::CGameObject::Free();
}
