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
	m_vecItems.resize(m_iSlotCount);

	return S_OK;
}

void CInventory::Sort_Item()
{
	// 10번째 인덱스부터 끝까지 정렬
	std::sort(m_vecItems.begin() + 10, m_vecItems.end(),
		[](CItem* a, CItem* b) {
			// 둘 다 nullptr인 경우
			if (a == nullptr && b == nullptr) return false;

			// a가 nullptr인 경우, b가 nullptr이 아닌 경우 b가 더 작다고 간주
			if (a == nullptr) return false; // nullptr은 뒤로
			if (b == nullptr) return true;  // nullptr이 아닌 것은 앞으로

			return a->Get_ItemNum() < b->Get_ItemNum();
		});
}

void CInventory::Sort_ChestItem()
{
	std::sort(m_vecItems.begin(), m_vecItems.end(),
		[](CItem* a, CItem* b) {
			// 둘 다 nullptr인 경우
			if (a == nullptr && b == nullptr) return false;

			// a가 nullptr인 경우, b가 nullptr이 아닌 경우 b가 더 작다고 간주
			if (a == nullptr) return false; // nullptr은 뒤로
			if (b == nullptr) return true;  // nullptr이 아닌 것은 앞으로

			return a->Get_ItemNum() < b->Get_ItemNum();
		});
}

bool CInventory::Add_Item(CItem* _pItem)
{
	int iMinSlot(m_iSlotCount);
	
	for (int i = 0; i < m_vecItems.size(); i++)
	{
		// 빈 슬롯 중 가장 앞 슬롯 탐색
		if (!m_vecItems[i])
		{
			iMinSlot = min(iMinSlot, i);

			if (_pItem->Get_ItemNum() < ITEM_ETC)
			{
				m_vecItems[iMinSlot] = _pItem;
				return true;
			}
		}
		else if(_pItem->Get_ItemNum() > ITEM_ETC && m_vecItems[i]->Get_ItemNum() == _pItem->Get_ItemNum())
		{
			m_vecItems[i]->Add_Count(_pItem->Get_Count());
			return true;
		}
	}

	if (iMinSlot < m_iSlotCount)
	{
		m_vecItems[iMinSlot] = _pItem;
		return true;
	}

	return false;
}

bool CInventory::Minus_Item(ITEMNUM _eItemNum, int _iCount)
{
	for (int i = 0; i < m_vecItems.size(); i++)
	{
		if (!m_vecItems[i])
			continue;

		if (m_vecItems[i]->Get_ItemNum() == _eItemNum)
		{
			if (m_vecItems[i]->Get_Count() > _iCount)
			{
				m_vecItems[i]->Minus_Count(_iCount);
				return true;
			}
			else if(m_vecItems[i]->Get_Count() == _iCount)
			{
				m_vecItems[i] = nullptr;
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

void CInventory::Swap_Item(CItem** _ppItem1, CItem** _ppItem2)
{
	CItem* pTemp = *_ppItem1;
	*_ppItem1 = *_ppItem2;
	*_ppItem2 = pTemp;
}

void CInventory::Remove_Item(int _iIndex)
{
	if (_iIndex >= m_vecItems.size())
	{
		return;
	}

	m_vecItems[_iIndex] = nullptr;
}

CItem* CInventory::Get_HandedItem(_int iHandNum)
{
	if (Check_Empty(iHandNum))
		return nullptr;

	return m_vecItems[iHandNum];
}

bool CInventory::Enough_Item(ITEMNUM _eItemNum, int _iCount)
{
	for (auto pItem : m_vecItems)
	{
		if (!pItem)
			continue;

		if (pItem->Get_ItemNum() == _eItemNum && pItem->Get_Count() >= _iCount)
		{
			return true;
		}
	}
	return false;
}

bool CInventory::Check_Empty(_int iIndex)
{
	if (m_vecItems[iIndex] == nullptr || m_vecItems[iIndex]->Get_Count() == 0)
	{
		return true;
	}

	return false;
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

CComponent* CInventory::Clone()
{
	return new CInventory(*this);
}

void CInventory::Free()
{
	CComponent::Free();
}
