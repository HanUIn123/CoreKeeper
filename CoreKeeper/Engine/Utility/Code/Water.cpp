#include "..\..\Header\Water.h"

CWater::CWater()
{
}

CWater::CWater(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev), fTime(0.f), m_iParticleNum(0), m_iAliveNum(0), m_bFirst(true)
{
}

CWater::CWater(const CWater& rhs)
	: PSystem(rhs), fTime(rhs.fTime), m_iParticleNum(rhs.m_iParticleNum), m_iAliveNum(rhs.m_iAliveNum), m_bFirst(rhs.m_bFirst)
{
}

CWater::~CWater()
{
}

HRESULT CWater::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
{
	_origin = *origin;
	//_size = 0.9;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들

	//for (int i = 0; i < numParticles; i++)
		//addParticle();
	m_iParticleNum = numParticles;

	return S_OK;
}

void CWater::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로

	attribute->_position.y = 0.2f;

	attribute->_velocity.x = 0.f;
	attribute->_velocity.y = 0.2f;
	attribute->_velocity.z = 0.3f;

	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 3.0f;

	attribute->_color = D3DCOLOR_ARGB(255, 255 ,255 , 255); 

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
	attribute->_lifeTime = 90.f; // 수명 2초
}

void CWater::update(float timeDelta, _vec3 vDir)
{
	if (m_bFirst)
	{
		std::list<Attribute>::iterator i;

		fTime += timeDelta;

		if (fTime >= 0.1f)
		{
			AddParticles();

			m_iAliveNum++;

			fTime = 0.f;
		}

		//if (fTime >= 180.f)
		//	fTime = 0.f;

		for (i = _particles.begin(); i != _particles.end(); i++)
		{
			//살아있는 파티클만 업데이트
			if (i->_isAlive)
			{

				i->_position.x += (i->_velocity.x * timeDelta);
				i->_position.y += (i->_velocity.y * i->_age - (0.5f * 9.8f * i->_age * i->_age));
				i->_position.z += (i->_velocity.z * timeDelta);

				i->_age += timeDelta;

				if (i->_position.y <= 0) // 수명이 끝남
				{
					resetParticles(&(*i));

					i->_age = 0.f;
				}
			}
		}

		if (m_iAliveNum == m_iParticleNum)
			m_bFirst = false;
	}
	else
	{
		std::list<Attribute>::iterator i;

		/*
		fTime += timeDelta;

		if (fTime >= 180.f)
			fTime = 0.f;
			*/

		for (i = _particles.begin(); i != _particles.end(); i++)
		{
			//살아있는 파티클만 업데이트
			if (i->_isAlive)
			{

				i->_position.x += (i->_velocity.x * timeDelta);
				i->_position.y += (i->_velocity.y * i->_age - (0.5f * 9.8f * i->_age * i->_age));
				i->_position.z += (i->_velocity.z * timeDelta);

				i->_age += timeDelta;

				if (i->_position.y <= 0) // 수명이 끝남
					resetParticles(&(*i));
			}
		}
	}
	/*
	i = _particles.begin();
	

	for (_int j = 0; j <= (_int)(_particles.size() / 2.f) - 1 ; j++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			//i->_position += i->_velocity * timeDelta;	
			i->_position.x += i->_velocity.x * timeDelta;
			i->_position.y += (i->_velocity.y * i->_age - (0.5f * 9.8f * i->_age * i->_age));
			i->_position.z += i->_velocity.z * timeDelta;

			i->_age += timeDelta;

			if (i->_position.y < 0)
			{
				resetParticle(&*(i));
			}
		}

		i++;
	}
	for (_int j = _particles.size() / 2.f; j < (_int)(_particles.size()) - 1; j++)
	{
		if (i->_isAlive)
		{
			//i->_position += i->_velocity * timeDelta;	c
			i->_position.x += i->_velocity.x * timeDelta;
			i->_position.y += (i->_velocity.y * i->_age - (0.5f * 9.8f * i->_age * fTime));
			i->_position.x += i->_velocity.z * timeDelta;

			i->_age += timeDelta;

			if (i->_position.y < 0)
				resetParticle(&*(i));
		}

		i++;
	}
	*/
}

void CWater::preRender()
{
	PSystem::preRender();
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	
	// z버퍼 읽기 끔
	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CWater::postRender()
{
	PSystem::postRender();

	//m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

void CWater::reset()
{
	PSystem::reset();
}

void CWater::resetParticles(Attribute* attribute)
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로

	attribute->_position.y = 0.2f;

	attribute->_lifeTime -= 25.f;

	attribute->_velocity.x = 2.5f * cosf(D3DXToRadian(attribute->_lifeTime));
	attribute->_velocity.z = 2.5f * sinf(D3DXToRadian(attribute->_lifeTime));

	attribute->_color = D3DCOLOR_ARGB(255, 255, 255, 255);

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
}

void CWater::AddParticles()
{
	Attribute attribute; // Attribute 구조체 불러오기

	resetParticle(&attribute);

	_particles.push_back(attribute);
}

CWater* CWater::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CWater* pInstance = new CWater(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CWater::Clone()
{
	return new CWater(*this);
}

void CWater::Free()
{
	CComponent::Free();
}
