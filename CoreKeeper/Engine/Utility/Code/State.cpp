#include "..\..\Header\State.h"

CState::CState() : m_bDead(false)
{
	ZeroMemory(&m_tStat, sizeof(STAT));
}

CState::CState(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_bDead(false)
{
	ZeroMemory(&m_tStat, sizeof(STAT));
}

CState::CState(const CState& rhs)
	: CComponent(rhs), m_bDead(rhs.m_bDead)
{
	memcpy(&m_tStat, &rhs.m_tStat, sizeof(STAT));
}

CState::~CState()
{
}

HRESULT CState::Ready_State()
{
	return S_OK;
}

CState* CState::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CState* pState = new CState(pGraphicDev);

	if (FAILED(pState->Ready_State()))
	{
		Safe_Release(pState);
		MSG_BOX("State Component Create Failed");
		return nullptr;
	}
	return pState;
}

CComponent* CState::Clone()
{
	return new CState(*this);
}

void CState::Free()
{
	CComponent::Free();
}
