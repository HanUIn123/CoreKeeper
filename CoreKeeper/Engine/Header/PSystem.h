#pragma once
#include "Component.h"
#include "d3dUtility.h"

BEGIN(Engine)

class ENGINE_DLL PSystem : public CComponent
{
public:
	explicit PSystem();
	explicit PSystem(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit PSystem(const PSystem& rhs);
	virtual ~PSystem();

	virtual bool init(const _tchar* texFileName, _int iTextNum = 1, _float fSize = 0.2f);
	virtual void reset();

	virtual void resetParticle(Attribute* attribute) = 0;
	virtual void addParticle();

	virtual void update(_float timeDelta) = 0;

	virtual void preRender();
	virtual void render();
	virtual void postRender();

	bool isEmpty();
	bool isDead();

protected:
	virtual void removeDeadParticles();

protected:
	D3DXVECTOR3 _origin;
	d3d::BoundingBox _boundingBox;
	_float      _emitRate;
	_float      _size;
	IDirect3DTexture9* _tex;
	IDirect3DVertexBuffer9* _vb;
	list<Attribute> _particles;
	int             _maxParticles;

	_int            m_iMaxTexture;

	DWORD _vbSize;
	DWORD _vbOffset;
	DWORD _vbBatchSize;

	vector<IDirect3DTexture9*> m_vecTexture;

public:
	virtual void		Free();
};

END