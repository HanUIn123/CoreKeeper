#include "..\..\Header\Hit.h"



CHit::CHit()
{
}

CHit::CHit(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev), m_fTime(0.f)
{
}

CHit::CHit(const CHit& rhs)
	: PSystem(rhs), m_fTime(rhs.m_fTime)
{
}

CHit::~CHit()
{
}

HRESULT CHit::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
{
	_origin = *origin;
	_size = 0.9;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들

	for (int i = 0; i < numParticles; i++)
		addParticle();

	return S_OK;
}

void CHit::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로

	attribute->_position.z = 0.4f;

	/*
	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // 최대

	//랜덤벡터 생성
	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);

	// 구를 만들기 위한 초기화
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 100.0f; // 속도 지정
	*/

	attribute->_velocity = { 0.f, 0.f, 0.f };

	attribute->_color = D3DXCOLOR(1.f, 1.f, 1.f, 1.0f); 

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
	attribute->_lifeTime = 0.07f; // 수명 2초
}

void CHit::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			i->_age += timeDelta;

			if (i->_age > i->_lifeTime)
			{
				i->_iTextureNum++;
				i->_age = 0.f;
			}

			if (i->_iTextureNum >= m_iMaxTexture) // 수명이 끝남
				i->_isAlive = false;
		}
	}
}

void CHit::preRender()
{
	PSystem::preRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CHit::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CHit* CHit::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CHit* pInstance = new CHit(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CHit::Clone()
{
	return new CHit(*this);
}

void CHit::Free()
{
	CComponent::Free();
}
