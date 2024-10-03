#include "..\..\Header\Smog.h"



CSmog::CSmog()
{
}

CSmog::CSmog(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev)
{
}

CSmog::CSmog(const CSmog& rhs)
	: PSystem(rhs)
{
}

CSmog::~CSmog()
{
}

HRESULT CSmog::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
{
	_origin = *origin;
	//_size = 0.9;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들

	for (int i = 0; i < numParticles; i++)
		addParticle();

	return S_OK;
}

void CSmog::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	//attribute->_position = _origin; // 처음위치로

	D3DXVECTOR3 min = D3DXVECTOR3(-3.0f, -2.0f, -3.0f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(3.0f, 2.0f, 3.0f); // 최대

	d3d::GetRandomVector(
		&attribute->_position,
		&min, &max);

	attribute->_velocity.x = 0.f;
	attribute->_velocity.y = 0.3f;
	attribute->_velocity.z = 0.f;

	// 구를 만들기 위한 초기화
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 5.0f; // 속도 지정

	attribute->_color = D3DCOLOR_ARGB(255, 255, 255, 255);

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
	attribute->_lifeTime = 0.07f; // 수명 2초
}

void CSmog::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			i->_position.y += i->_velocity.y * timeDelta;

			i->_age += timeDelta;
			
			if (i->_age > i->_lifeTime) // 수명이 끝남
			{
				i->_iTextureNum++;

				i->_age = 0.f;
			}

			if (i->_iTextureNum >= m_iMaxTexture)
				i->_isAlive = false;
		}
	}
}

void CSmog::preRender()
{
	PSystem::preRender();

	//m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	//m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CSmog::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CSmog* CSmog::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CSmog* pInstance = new CSmog(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CSmog::Clone()
{
	return new CSmog(*this);
}

void CSmog::Free()
{
	CComponent::Free();
}
