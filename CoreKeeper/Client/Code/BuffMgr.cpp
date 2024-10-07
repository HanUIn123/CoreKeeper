#include "pch.h"
#include "../Header/BuffMgr.h"
#include "Export_Utility.h"
#include "../Header/Player.h"

IMPLEMENT_SINGLETON(CBuffMgr)

CBuffMgr::CBuffMgr() : m_pGraphicDev(nullptr), m_pPlayer(nullptr)
{
	for (_int i = 0; i < BUFFTYPE_END; i++)
	{
		m_arrBuffState[i].x = 0.f;
		m_arrBuffState[i].y = 0.f;
	}
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
	m_pPlayer->Set_BuffState(_eType, true);
	m_arrBuffState[_eType].x += fTime;
	m_arrBuffState[_eType].y += fTime;
}

// 버프 현재 시간이 0 이상이면 줄이기
void CBuffMgr::BuffTimer(const _float& fTimeDelta)
{
	for (_int i = 0; i < BUFFTYPE_END; i++)
	{
		if (m_pPlayer->Get_BuffState((BUFFTYPE)i))
		{
			if (m_arrBuffState[i].x > 0)
				m_arrBuffState[i].x -= fTimeDelta;
			else
			{
				m_arrBuffState[i].x = 0;
				m_arrBuffState[i].y = 0;
				m_pPlayer->Set_BuffState((BUFFTYPE)i, false);
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

void CBuffMgr::Free()
{
	Safe_Release(m_pGraphicDev);
}