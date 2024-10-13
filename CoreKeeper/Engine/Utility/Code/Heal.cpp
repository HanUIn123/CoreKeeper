#include "..\..\Header\Heal.h"



CHeal::CHeal()
{
}

CHeal::CHeal(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev), m_bStatic(false)
{
}

CHeal::CHeal(const CHeal& rhs)
	: PSystem(rhs), m_bStatic(rhs.m_bStatic)
{
}

CHeal::~CHeal()
{
}

HRESULT CHeal::Ready_Particles(D3DXVECTOR3* origin, _int numParticles, _bool _bStatic)
{
	_origin = *origin;
	//_size = 0.9;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
	//파티클 기본 속성들

	for (int i = 0; i < numParticles; i++)
		addParticle();

	m_bStatic = _bStatic;

	return S_OK;
}

void CHeal::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;	
	attribute->_position = _origin; // 처음위치로

	D3DXVECTOR3 min = D3DXVECTOR3(-0.5f, -0.3f, -0.5f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(0.5f, 0.3f, 0.5f); // 최대

	//랜덤벡터 생성
	d3d::GetRandomVector(
		&attribute->_position,
		&min,
		&max);

	attribute->_velocity.x = 0.f;
	attribute->_velocity.y = 0.01f;
	attribute->_velocity.z = 0.f;

	// 구를 만들기 위한 초기화
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 1.f; // 속도 지정

	attribute->_color = D3DXCOLOR(1.f, 1.f, 1.f, 0.2f); 

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
	attribute->_lifeTime = 0.7f; // 수명 2초
}

void CHeal::update(float timeDelta, _vec3 vDir)
{
	std::list<Attribute>::iterator i;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			if (m_bStatic)
			{
				if (i->_age > i->_lifeTime) // 수명이 끝남
				{
					resetParticle(&(*i));
				}
			}
			else
			{
				if (i->_age > i->_lifeTime) // 수명이 끝남
				{
					i->_isAlive = false;
				}
			}
		}
	}
}

void CHeal::preRender()
{
	PSystem::preRender();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// z버퍼 읽기 끔

	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CHeal::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, true);

	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CHeal* CHeal::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles, _bool _bStatic)
{
	CHeal* pInstance = new CHeal(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles, _bStatic)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CHeal::Clone()
{
	return new CHeal(*this);
}

void CHeal::Free()
{
	CComponent::Free();
}
