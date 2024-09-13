#include "Export_Utility.h"

CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
}


CInventory::~CInventory()
{
	m_vecItems.clear();
}

HRESULT CInventory::Ready_Inventory(int _iSlotCount)
{
	m_iSlotCount = _iSlotCount;
	m_vecItems.reserve(m_iSlotCount);

	return S_OK;
}


void CInventory::Add_Item(ITEMNUM _eItemNum)
{
	if (m_vecItems.size() >= m_iSlotCount)
	{
		// 슬롯이 꽉 차있으면 리턴
		return;
	}

	bool bItemFound = false;

	for (auto map : m_vecItems)
	{
		auto iter = map.find(_eItemNum);

		if (iter != map.end())
		{
			iter->second++;
			bItemFound = true;
			break;
		}
	}

	if (!bItemFound)
	{
		map<ITEMNUM, int> newItemMap;
		newItemMap[_eItemNum] = 1;
		m_vecItems.push_back(newItemMap);
	}
}

void CInventory::Remove_Item(ITEMNUM _eItemNum)
{
	if (m_vecItems.empty())
	{
		// 슬롯이 비어 있으면 리턴
		return;
	}

	auto iter = find_if(m_vecItems.begin(), m_vecItems.end(),
		[_eItemNum](map<ITEMNUM, int>& itemMap)
		{
			return itemMap.find(_eItemNum) != itemMap.end();
		});

	if (iter != m_vecItems.end())
	{
		auto& itemMap = *iter;
		auto itemIter = itemMap.find(_eItemNum);

		if (itemIter != itemMap.end())
		{
			itemIter->second--;

			// 개수가 0 이하가 되면 아이템을 맵에서 제거
			if (itemIter->second <= 0)
			{
				itemMap.erase(itemIter);

				// 맵이 비어 있으면 벡터에서 제거
				if (itemMap.empty())
				{
					m_vecItems.erase(iter);
				}
			}
		}
	}
}



CInventory* CInventory::Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iSlotCount)
{
	CInventory* pInventory = new CInventory(pGraphicDev);

	if (FAILED(pInventory->Ready_Inventory(_iSlotCount)))
	{
		Safe_Release(pInventory);
		return nullptr;
	}

	return pInventory;
}

CInventory* CInventory::Clone()
{
	return new CInventory(*this);
}

void CInventory::Free()
{
	CComponent::Free();
}
