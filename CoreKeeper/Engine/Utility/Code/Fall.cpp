#include "..\..\Header\Fall.h"



CFall::CFall()
{
	fTime = 0;
}

CFall::CFall(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev)
{
	fTime = 0;
}

CFall::CFall(const CFall& rhs)
	: PSystem(rhs), fTime(0)
{
}

CFall::~CFall()
{
}

HRESULT CFall::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
{
	_origin = *origin;
	//_size = 0.2;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들
	m_iMaxTexture = 0;

	for (int i = 0; i < numParticles; i++)
		addParticle();

	return S_OK;
}

void CFall::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // 최대

	//랜덤벡터 생성
	/*
	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);
		*/


	attribute->_velocity.x = d3d::GetRandomFloat(-1.0f, 1.0f) * 0.01f;
	attribute->_velocity.y = d3d::GetRandomFloat(0.0f, 0.5f) * 0.05f;
	attribute->_velocity.z = d3d::GetRandomFloat(-1.0f, 1.0f) * 0.01f;

	// 구를 만들기 위한 초기화
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 10.0f; // 속도 지정

	attribute->_velocity.y *= 0.2f;

	//attribute->_acceleration = { 0.f, 0.f, 0.f };

	attribute->_color = D3DCOLOR_ARGB(255, 255, 0, 0); // 랜덤 색 생성 (빨간색 X)

	attribute->_age = 0.0f;
	attribute->_lifeTime = 1.0f; // 수명 1초
	
	attribute->_iTextureNum = 0;
}

void CFall::update(float timeDelta, _vec3 vDir)
{
	std::list<Attribute>::iterator i;

	fTime += timeDelta;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{

			i->_position.x += (i->_velocity.x * timeDelta);
			i->_position.y += (i->_velocity.y * fTime - (0.5f * 9.8f * fTime * fTime));
			i->_position.z += (i->_velocity.z * timeDelta);

			i->_age += timeDelta;

			if (i->_position.y <= 0) // 수명이 끝남
				i->_isAlive = false;

			//i->_acceleration = (fPrePos - i->_velocity) / timeDelta;
			//i->_velocity = i->_velocity + i->_acceleration * timeDelta;
			/*
			if (i->_position.y <= 0 || i->_age > i->_lifeTime) // 수명이 끝남
				i->_isAlive = false;
				*/
		}
	}
}

void CFall::preRender()
{
	PSystem::preRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFall::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

void CFall::reset()
{
	PSystem::reset();

	fTime = 0.f;
}

CFall* CFall::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CFall* pInstance = new CFall(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CFall::Clone()
{
	return new CFall(*this);
}

void CFall::Free()
{
	CComponent::Free();
}
