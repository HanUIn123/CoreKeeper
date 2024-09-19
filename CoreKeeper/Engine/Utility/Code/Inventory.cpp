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

void CInventory::Add_Item(CItem* _pItem)
{
	if (m_vecItems.size() >= m_iSlotCount)
	{
		// ½½·ÔÀÌ ²Ë Â÷ÀÖÀ¸¸é ¸®ÅÏ
		return;
	}

	bool bItemFound = false;

	if (_pItem->Get_ItemNum() > 30)
	{
		for (auto pItem : m_vecItems)
		{
			if (pItem->Get_ItemNum() == _pItem->Get_ItemNum())
			{
				pItem->Add_Count(_pItem->Get_Count());
				bItemFound = true;
				break;
			}
		}
	}


	if (!bItemFound)
	{
		m_vecItems.push_back(_pItem);
	}
}

void CInventory::Swap_Item(CItem** _ppItem1, CItem** _ppItem2)
{
	CItem* pTemp = *_ppItem1;
	*_ppItem1 = *_ppItem2;
	*_ppItem2 = pTemp;
}

CItem* CInventory::Remove_Item(int _iIndex)
{
	CItem* pTemp = m_vecItems[_iIndex];

	m_vecItems[_iIndex] = nullptr;

	return pTemp;
}

CItem* CInventory::Get_HandedItem(_int iHandNum)
{
	if (Check_Empty(iHandNum))
		return nullptr;
	return m_vecItems[iHandNum - 1];
}

bool CInventory::Check_Empty(_int iIndex)
{
	if (m_vecItems.empty())
	{
		return true;
	}

	int i = 1;

	// iï¿½ï¿½ï¿½ï¿½ indexï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ forï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	for (auto iter : m_vecItems)
	{
		if (i == iIndex)
		{
			if (iter->Get_Count() == 0)
			{
				return true;
			}
			else
				return false;
		}

		i++;
	}

	return true;
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
