#include "..\..\Header\Smoke.h"



CSmoke::CSmoke()
{
}

CSmoke::CSmoke(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev)
{
}

CSmoke::CSmoke(const CSmoke& rhs)
	: PSystem(rhs)
{
}

CSmoke::~CSmoke()
{
}

HRESULT CSmoke::Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _int _iMaxTexture)
{
	_origin = *origin;
	//_size = 0.2;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들
	m_iMaxTexture = _iMaxTexture;

	for (int i = 0; i < numParticles; i++)
		addParticle();

	return S_OK;
}

void CSmoke::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	//attribute->_position = _origin; // 처음위치로

	attribute->_position.x = d3d::GetRandomFloat(-0.3f, 0.3f);
	attribute->_position.y = -0.8f;
	attribute->_position.z = d3d::GetRandomFloat(-0.3f, 0.3f);
	//D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f); // 최소
	//D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // 최대

	/*
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
	attribute->_velocity.x = 0.f;
	attribute->_velocity.y = 0.f;
	attribute->_velocity.z = 0.f;

	attribute->_color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f); 
	attribute->_colorFade = D3DXCOLOR(1.f, 1.f, 1.f, d3d::GetRandomFloat(0.f, 1.f));

	attribute->_iTextureNum = (_int)d3d::GetRandomFloat(0.f, (_float)m_iMaxTexture);

	attribute->_age = 0.0f;
	attribute->_lifeTime = 2.0f; // 수명 2초
}

void CSmoke::update(float timeDelta, _vec3 vDir)
{
	std::list<Attribute>::iterator i;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			i->_color -= i->_colorFade * timeDelta;

			i->_position += vDir;

			if (i->_color.a <= 0.f) // 수명이 끝남
			{
				i->_isAlive = false;
				resetParticle(&(*i));
			}
		}
	}
}

void CSmoke::preRender()
{
	PSystem::preRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CSmoke::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CSmoke* CSmoke::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _int _iMaxTexture)
{
	CSmoke* pInstance = new CSmoke(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles, _iMaxTexture)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CSmoke::Clone()
{
	return new CSmoke(*this);
}

void CSmoke::Free()
{
	CComponent::Free();
}
