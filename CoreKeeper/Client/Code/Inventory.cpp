#include "pch.h"
#include "..\Header\Inventory.h"
#include "Export_System.h"
#include "Export_Utility.h"

CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
    : Engine::CGameObject(pGraphicDev), m_iSlotCount(30)
{
    m_vecItems.reserve(m_iSlotCount);
}

CInventory::~CInventory()
{

}

HRESULT CInventory::Ready_GameObject()
{
    FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

    return S_OK;
}

void CInventory::Add_Item(CItem* pItem)
{
    // 아이템 타입을 기준으로 맵을 검색하거나 새로 생성
    ITEMNUM eItemNum = pItem->Get_ItemNum();

    auto iter = find_if(m_vecItems.begin(), m_vecItems.end(),
        [eItemNum](const map<ITEMNUM, int>& itemMap)
        {
            return itemMap.find((eItemNum)) != itemMap.end();
        });

    if (iter != m_vecItems.end()) 
    {
        // 아이템이 이미 존재하면 개수 추가
        (*iter)[eItemNum]++;
    }
    else 
    {
        // 새 아이템 추가
        map<ITEMNUM, int> newItemMap;
        newItemMap[eItemNum] = 1;
        m_vecItems.push_back(newItemMap);
    }
}

void CInventory::Remove_Item(CItem* pItem)
{
    ITEMNUM eItemNum = pItem->Get_ItemNum();

    auto iter = std::find_if(m_vecItems.begin(), m_vecItems.end(),
        [eItemNum](const std::map<ITEMNUM, int>& itemMap) 
        {
            return itemMap.find(eItemNum) != itemMap.end();
        });

    if (iter != m_vecItems.end()) 
    {
        (*iter)[eItemNum]--;

        if ((*iter)[eItemNum] <= 0)
        {
            iter->erase(eItemNum);
        }

        // 맵이 비어있으면 벡터에서 제거
        if (iter->empty()) 
        {
            m_vecItems.erase(iter);
        }
    }
}

void CInventory::Show_Inventory()
{

}

_int CInventory::Update_GameObject(const _float& fTimeDelta)
{
    Add_RenderGroup(RENDER_ALPHA, this);

    return Engine::CGameObject::Update_GameObject(fTimeDelta);
}

void CInventory::LateUpdate_GameObject()
{
    Engine::CGameObject::LateUpdate_GameObject();
}

void CInventory::Render_GameObject()
{
}

HRESULT CInventory::Add_Component()
{
    return S_OK;
}

CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CInventory* pInventory = new CInventory(pGraphicDev);

    if (FAILED(pInventory->Ready_GameObject()))
    {
        Safe_Release(pInventory);
        MSG_BOX("Inventory Create Failed");
        return nullptr;
    }

    return pInventory;
}

void CInventory::Free()
{
    Engine::CGameObject::Free();

    /*for (auto& itemMap : m_vecItems) 
    {
        for (auto& item : itemMap) 
        {
            
        }
    }*/

    m_vecItems.clear();
}
