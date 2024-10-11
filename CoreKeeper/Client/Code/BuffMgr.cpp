#include "pch.h"
#include "../Header/BuffMgr.h"
#include "Export_Utility.h"
#include "../Header/Player.h"
#include "../Header/UIBuff.h"

IMPLEMENT_SINGLETON(CBuffMgr)

CBuffMgr::CBuffMgr() : m_pGraphicDev(nullptr), m_pPlayer(nullptr)
{
	for (_int i = 0; i < BUFFTYPE_END; i++)
	{
		m_arrBuffState[i].x = 0.f;
		m_arrBuffState[i].y = 0.f;
	}
	m_vecUIBuff.reserve(BUFFTYPE_END);
	m_vecCurOrder.reserve(BUFFTYPE_END);
}

CBuffMgr::~CBuffMgr()
{
	Free();
}

HRESULT CBuffMgr::Ready_Buff(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;

	return S_OK;
}

void CBuffMgr::Update_Buff(const _float& fTimeDelta)
{
	if (!m_pPlayer)
		m_pPlayer = dynamic_cast<CPlayer*>(Engine::Get_GameObject(L"Layer_GameLogic", L"Player"));

	if(m_pPlayer)
		BuffTimer(fTimeDelta);
}

void CBuffMgr::Set_BuffStart(BUFFTYPE _eType, _float fTime)
{
	if (m_pPlayer)
	{
		_int iType = (_int)_eType;
		m_pPlayer->Set_BuffState(_eType, true);
		if (_eType == DEBUFF_SLOW)
		{
			m_arrBuffState[iType].x = fTime;
			m_arrBuffState[iType].y = fTime;
		}
		else
		{
			m_arrBuffState[iType].x += fTime;
			m_arrBuffState[iType].y += fTime;
		}
		m_vecUIBuff[iType]->Set_Window(_eType);
		m_vecUIBuff[iType]->Set_BuffTime(m_arrBuffState[iType].x, m_arrBuffState[iType].y);
		if (!m_vecUIBuff[iType]->Get_Allocate())
		{
			m_vecUIBuff[iType]->Set_Position(_vec2(60 + 50 * m_vecCurOrder.size(), 130.f));
			m_vecCurOrder.push_back(_eType);
			m_vecUIBuff[iType]->Set_Allocate(true);
		}
	}
}

// 버프 현재 시간이 0 이상이면 줄이기, 999는 상시 버프(ON/OFF)
void CBuffMgr::BuffTimer(const _float& fTimeDelta)
{
	for (_int i = 0; i < BUFFTYPE_END; i++)
	{
		if (m_pPlayer->Get_BuffState((BUFFTYPE)i))
		{
			if (m_arrBuffState[i].x > 0)
			{
				if (m_arrBuffState[i].x <= 999)
					m_arrBuffState[i].x -= fTimeDelta;
				m_vecUIBuff[i]->Set_BuffTime(m_arrBuffState[i].x, m_arrBuffState[i].y);
			}
			else
			{
				m_arrBuffState[i].x = 0;
				m_arrBuffState[i].y = 0;
				m_pPlayer->Set_BuffState((BUFFTYPE)i, false);
				if (m_vecUIBuff[i]->Get_Allocate())
				{
					m_vecUIBuff[i]->Set_Allocate(false);
					Arrange_UIBuff(i);
				}

			}
		}
		else
		{
			if (m_arrBuffState[i].x > 0)
			{
				m_arrBuffState[i].x = 0;
				m_arrBuffState[i].y = 0;
			}
		}
	}
}

void CBuffMgr::Arrange_UIBuff(_int iIndex)
{
	_int iCurIndex = 0, iCritIndex = 0;
	// 현재 등록되어 있는 모든 버프창에 대해서
	for (auto iter = m_vecCurOrder.begin(); iter != m_vecCurOrder.end();)
	{
		// 삭제한 버프창이 순서 목록에 있는지 확인하고 있으면 지우기
		if ((*iter) == (BUFFTYPE)iIndex)
		{
			iCritIndex = iCurIndex;
			iter = m_vecCurOrder.erase(iter);
		}
		else
		{
			iCurIndex++;
			iter++;
		}
		// 삭제한 버프창보다 순서 목록이 뒤에 있을 경우 위치 다시 잡아주기
		if (iCurIndex > iCritIndex)
			m_vecUIBuff[*(iter - 1)]->Set_Position(_vec2(60 + 50 * (iCurIndex - 1), 130.f));
	}
}

void CBuffMgr::Free()
{
	Safe_Release(m_pGraphicDev);
}