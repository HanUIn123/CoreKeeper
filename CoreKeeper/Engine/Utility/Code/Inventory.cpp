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

void CInventory::Add_Item(CItem* _pItem)
{
	int iMinSlot(m_iSlotCount);
	bool bItemFound = false;

	//vector<CItem*>::iterator it;

	//it = m_vecItems.begin();

	if (_pItem->Get_ItemNum() > 30)
	{
		for (int i = 0; i < m_vecItems.size(); i++)
		{
			if (m_vecItems[i] == nullptr)
			{
				if (iMinSlot > i)
				{
					iMinSlot = i;
				}
				continue;
			}

			if (m_vecItems[i]->Get_ItemNum() == _pItem->Get_ItemNum())
			{
				m_vecItems[i]->Add_Count(_pItem->Get_Count());
				bItemFound = true;
				return;
			}
		}
	}
	else
	{
		for (int i = 0; i < m_vecItems.size(); i++)
		{
			if (m_vecItems[i] == nullptr)
			{
				if (iMinSlot > i)
				{
					iMinSlot = i;
				}
				continue;
			}
		}
	}
	// ²ËÂ÷ÀÖÀ½
	if (iMinSlot == m_iSlotCount)
	{
		return;
	}

	if (!bItemFound)
	{
		m_vecItems[iMinSlot] = _pItem;
	}

	/*if ( _iIndex == 0)
	{
		if (Check_Empty(0))
		{
			m_vecItems.insert(it, _pItem);

		}
		else
		{
			for (int i = 1; ; i++)
			{
				if (Check_Empty(i))
				{
					m_vecItems.erase(it + i);
					m_vecItems.insert(it + i, _pItem);
					return;
				}
			}
		}
	}
	else
	{
		m_vecItems.erase(it + _iIndex);
		m_vecItems.insert(it + _iIndex, _pItem);
	}*/

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

CInventory* CInventory::Clone()
{
	return new CInventory(*this);
}

void CInventory::Free()
{
	CComponent::Free();
}
