#include "..\..\Header\Fume.h"



CFume::CFume()
{
}

CFume::CFume(LPDIRECT3DDEVICE9 pGraphicDev)
	: PSystem(pGraphicDev)
{
}

CFume::CFume(const CFume& rhs)
	: PSystem(rhs)
{
}

CFume::~CFume()
{
}

HRESULT CFume::Ready_Particles(D3DXVECTOR3* origin, _int numParticles)
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

void CFume::resetParticle(Attribute* attribute) // 파티클 리셋
{
	attribute->_isAlive = true;
	attribute->_position = _origin; // 처음위치로

	D3DXVECTOR3 min = D3DXVECTOR3(-0.1f, 0.0f, -0.1f); // 최소
	D3DXVECTOR3 max = D3DXVECTOR3(0.1f, 1.0f, 0.1f); // 최대

	//랜덤벡터 생성
	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);

	// 구를 만들기 위한 초기화
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 1.0f; // 속도 지정
	
	/*
	attribute->_color = D3DXCOLOR(
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		1.0f); // 랜덤 색 생성 (빨간색 X)
		*/

	//attribute->_color = D3DXCOLOR(d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f));

	//attribute->_colorFade = D3DXCOLOR(d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f), d3d::GetRandomFloat(0.0f, 0.5f));

	attribute->_color = D3DXCOLOR(1.f, 1.f, 1.f, 0.1f);

	attribute->_colorFade = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);

	attribute->_iTextureNum = 0;

	attribute->_age = 0.0f;
	attribute->_lifeTime = d3d::GetRandomFloat(0.0f, 3.0f); // 수명 2초
}

void CFume::update(float timeDelta, _vec3 vDir)
{
	std::list<Attribute>::iterator i;

	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		//살아있는 파티클만 업데이트
		if (i->_isAlive)
		{
			i->_position += i->_velocity * timeDelta;

			//i->_color -= i->_colorFade * timeDelta * 100.f;

			i->_age += timeDelta;
			
			if (i->_age >= i->_lifeTime) // 수명이 끝남
			{
					resetParticle(&(*i));
			}

			//if (i->_color.a <= 0)
			//	resetParticle(&(*i));
			/*
			if (i->_iTextureNum >= m_iMaxTexture)
				i->_isAlive = false;*/
		}
	}
}

void CFume::preRender()
{
	PSystem::preRender();
	/*
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	*/


	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pGraphicDev->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(100, 255, 255, 255));
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	// z버퍼 읽기 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CFume::postRender()
{
	PSystem::postRender();

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, true);


	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, false);
}

CFume* CFume::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXVECTOR3* origin, _int numParticles)
{
	CFume* pInstance = new CFume(pGraphicDev);

	if (FAILED(pInstance->Ready_Particles(origin, numParticles)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Firework Create Failed");
		return nullptr;
	}

	return pInstance;
}

CComponent* CFume::Clone()
{
	return new CFume(*this);
}

void CFume::Free()
{
	CComponent::Free();
}
