#include "..\..\Header\Followers.h"



CFollow::CFollow() : _fTime(0.f)
{
}

CFollow::CFollow(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev), _fTime(0.f)
{
}

CFollow::CFollow(const CFollow& rhs)
	: PSystem(rhs), _fTime(rhs._fTime)
{
}

CFollow::~CFollow()
{
}

HRESULT CFollow::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
{
	_origin = *origin;
	//_size = 0.2;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들

	for (int i = 0; i < numParticles; i++)
		addParticle();

	return S_OK;
}

void CFollow::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로
	
	attribute->_position.x = d3d::GetRandomFloat(-0.4f, 0.4f);
	attribute->_position.y = d3d::GetRandomFloat(-0.2f, 0.3f);
	attribute->_position.z = d3d::GetRandomFloat(-0.5f, 0.2f);

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(1.0f, 1.0f, 1.0f); // 최대

	/*
	//랜덤벡터 생성
	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);*/

	attribute->_velocity.x = 0.f;
	attribute->_velocity.y = 0.f;
	attribute->_velocity.z = 0.f;

	attribute->_iTextureNum = (_int)d3d::GetRandomFloat(0.f, (_float)m_iMaxTexture);

	// 구를 만들기 위한 초기화
	
	/*
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 100.0f; // 속도 지정
	*/

	
	attribute->_color = D3DXCOLOR(1.f, 1.f, 1.f, 1.f); // 랜덤 색 생성 (빨간색 X)
		
	attribute->_age = 0.0f;
	attribute->_lifeTime = 0.08f; // 수명 2초
}

void CFollow::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	_fTime += timeDelta;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			//i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			if (i->_age > i->_lifeTime)
			{
				i->_iTextureNum++;
				i->_age = 0.f;
			}

			if (i->_iTextureNum > m_iMaxTexture - 1)
			{
				i->_iTextureNum = 0;
			}

		}
	}
}

void CFollow::preRender()
{
	PSystem::preRender();

	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFollow::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CFollow* CFollow::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CFollow* pInstance = new CFollow(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CFollow::Clone()
{
	return new CFollow(*this);
}

void CFollow::Free()
{
	CComponent::Free();
}
