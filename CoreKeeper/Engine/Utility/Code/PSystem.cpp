#include <cstdlib>
#include "PSystem.h"

PSystem::PSystem()
	: _emitRate(0), _size(0), _tex(0), _vb(0), _maxParticles(0), _vbSize(0), _vbOffset(0), _vbBatchSize(0), m_iMaxTexture(0)
{	
	_origin = {};
	ZeroMemory(&_boundingBox, sizeof(_boundingBox));
}

PSystem::PSystem(LPDIRECT3DDEVICE9 pGraphicDev)
	:CComponent(pGraphicDev), _emitRate(0), _size(0), _tex(0), _vb(0), _maxParticles(0), _vbSize(0), _vbOffset(0), _vbBatchSize(0), m_iMaxTexture(0)
{
	_origin = {};
	ZeroMemory(&_boundingBox, sizeof(_boundingBox));
}

PSystem::PSystem(const PSystem& rhs)
	:CComponent(rhs), _origin(rhs._origin), _boundingBox(rhs._boundingBox), _emitRate(rhs._emitRate), _size(rhs._size), _tex(rhs._tex), _vb(rhs._vb), _particles(rhs._particles),
	_maxParticles(rhs._maxParticles), _vbSize(rhs._vbSize), _vbOffset(rhs._vbOffset), _vbBatchSize(rhs._vbBatchSize), m_iMaxTexture(rhs.m_iMaxTexture)
{
}

PSystem::~PSystem()
{

}

bool PSystem::init(const _tchar* texFileName, _int iTexNum, _float fSize)
{

	HRESULT hr = 0;

	hr = m_pGraphicDev->CreateVertexBuffer(
		_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, FVF_PARTICLE,
		D3DPOOL_DEFAULT,
		&_vb,
		0);

	if (FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexBuffer() - FAILED", L"PSystem", 0);
	}

	m_vecTexture.reserve(iTexNum);

	IDirect3DTexture9* pTexture = NULL;

	for (_int i = 0; i < iTexNum; ++i)
	{
		TCHAR		szFileName[128] = L"";

		wsprintf(szFileName, texFileName, i);

		FAILED_CHECK_RETURN(D3DXCreateTextureFromFile(m_pGraphicDev, szFileName, (LPDIRECT3DTEXTURE9*)&pTexture), E_FAIL);

		m_vecTexture.push_back(pTexture);
	}

	_size = fSize;

	return true;
}

void PSystem::reset() // 파티클 다시 리셋 시키기 (다시쓰기)
{
	std::list<Attribute>::iterator i;
	for (i = _particles.begin(); i != _particles.end(); i++)
	{
		resetParticle(&(*i));
	}
}

void PSystem::addParticle() // 파티클 추가
{
	Attribute attribute; // Attribute 구조체 불러오기

	resetParticle(&attribute);

	_particles.push_back(attribute);
}

void PSystem::preRender() // 파티클 출력 전에 정해줘야할 렌더 상태들
{
	//m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, true); //포인트 크기지정 가능
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(_size)); //포인트 사이즈
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f)); //포인트 최소 사이즈

	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f)); // FtoDw ->  float toDw float를 Dw로 숨겨서 가져가기
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f)); // C에 1을 넣어라(?)

	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE); // 알파 블렌딩
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void PSystem::postRender()
{
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, true);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void PSystem::render()
{
	if (!_particles.empty())
	{
		preRender();

		m_pGraphicDev->SetFVF(FVF_PARTICLE);
		m_pGraphicDev->SetStreamSource(0, _vb, 0, sizeof(Particle));

		if (_vbOffset >= _vbSize)
			_vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(
			_vbOffset * sizeof(Particle),
			_vbBatchSize * sizeof(Particle),
			(void**)&v,
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD); // 지정한 batch 범위 이상을 벗어나지 못하게 잠금

		DWORD numParticlesInBatch = 0;

		list<Attribute>::iterator i;
		for (i = _particles.begin(); i != _particles.end(); i++)
		{
			if (i->_isAlive) // 파티클(들)에 정보 넘겨주기
			{
				m_pGraphicDev->SetTexture(0, m_vecTexture[i->_iTextureNum]);

				v->_position = i->_position;
				v->_color = (D3DCOLOR)i->_color;
				v++;

				numParticlesInBatch++;


				//batch가 꽉찼을때 출력

				if (numParticlesInBatch == _vbBatchSize)
				{
					_vb->Unlock();

					m_pGraphicDev->DrawPrimitive(
						D3DPT_POINTLIST,
						_vbOffset,
						_vbBatchSize);

					_vbOffset += _vbBatchSize;

					if (_vbOffset >= _vbSize)
						_vbOffset = 0;

					_vb->Lock(
						_vbOffset * sizeof(Particle),
						_vbBatchSize * sizeof(Particle),
						(void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0;
				} // 다음 batch 사이즈가 꽉찰때까지 버퍼 잠그고 정보 넘겨주기
			}
		}
		//모든 파티클 출력할때까지 batch사이즈 만큼 정보 불러온 후 
		// batch 사이즈가 꽉 차면 출력 후 다시 정보 불러오기 반복

		_vb->Unlock();


		// 정해놓은 batchsize 보다 작아 남아잇는 파티클들 출력
		if (numParticlesInBatch)
		{
			m_pGraphicDev->DrawPrimitive(
				D3DPT_POINTLIST,
				_vbOffset,
				numParticlesInBatch);
		}

		// 버퍼 중 다음 블록(?)으로 넘어가기
		_vbOffset += _vbBatchSize;

		postRender();
	}
}

bool PSystem::isEmpty()
{
	return _particles.empty();
}

bool PSystem::isDead() // 파티클이 모두 죽었는지 확인
{
	list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		if (i->_isAlive) // 살아있으면 return
			return false;
	}
	
	return true;
}

void PSystem::removeDeadParticles() //죽은 파티클 삭제
{
	list<Attribute>::iterator i;

	i = _particles.begin();

	while (i != _particles.end())
	{
		if (i->_isAlive == false)// 죽은것 삭제
		{
			i = _particles.erase(i);
		}
		else
		{
			i++;
		}
	}
}

void PSystem::Free()
{
	CComponent::Free();
}
