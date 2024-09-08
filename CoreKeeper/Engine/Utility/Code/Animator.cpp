#include "Export_Utility.h"

CAnimator::CAnimator(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev)
{
	m_tAnim.ePreState = STATE_END;
	m_tAnim.eCurState = STATE_END;
}


CAnimator::~CAnimator()
{
}

HRESULT CAnimator::Ready_Animator()
{
	return S_OK;
}

void CAnimator::Update_Animation()
{
	m_tAnim.iCurCount++;

	if (m_tAnim.iCurCount >= m_tAnim.iCount)
	{
		m_tAnim.iMotion++;
		m_tAnim.iCurCount = 0;
	}

	if (m_tAnim.iMotion > m_tAnim.iEnd)
	{
		m_tAnim.iMotion = m_tAnim.iStart;
	}
}

void CAnimator::Set_CurState(STATE _eCurState, int _iStart, int _iEnd, int _iCount)
{
	// 상태, 애니메이션, 시간 중 하나라도 다르면 바뀔 수 있도록.
	// 같은 IDLE이어도 start나 end나 count가 다르면 바뀌게 해둠
	if (m_tAnim.eCurState != _eCurState || m_tAnim.iStart != _iStart || m_tAnim.iEnd != _iEnd || m_tAnim.iCount != _iCount)
	{
		m_tAnim.iStart = _iStart;
		m_tAnim.iEnd = _iEnd;
		m_tAnim.iMotion = _iStart;
		m_tAnim.iCount = _iCount;
		m_tAnim.iCurCount = 0;

		m_tAnim.ePreState = m_tAnim.eCurState;
		m_tAnim.eCurState = _eCurState;
	}
}

CAnimator* CAnimator::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CAnimator* pAnimator = new CAnimator(pGraphicDev);

	if (FAILED(pAnimator->Ready_Animator()))
	{
		Safe_Release(pAnimator);
		return nullptr;
	}

	return pAnimator;
}

CAnimator* CAnimator::Clone()
{
	return new CAnimator(*this);
}

void CAnimator::Free()
{
	CComponent::Free();
}
