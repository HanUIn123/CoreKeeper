#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;
class CTexture;

END

class CTerrain : public Engine::CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTerrain();

public:
	virtual			HRESULT								Ready_GameObject();
	virtual			_int								Update_GameObject(const _float& fTimeDelta);
	virtual			void								LateUpdate_GameObject();
	virtual			void								Render_GameObject();

private:
	HRESULT												Add_Component();
	HRESULT												Setup_Material();

public:
	void												Set_TextureNumber(int _iIndex, int _iNum) { m_vecTextureNumber[_iIndex] = _iNum; }
	vector<int>											Get_TextureNumber() { return m_vecTextureNumber; }


	void                                                Set_TextureNumber(vector<int> _vecTextureNumber) { copy(_vecTextureNumber.begin(), _vecTextureNumber.end(), m_vecTextureNumber.begin());}
private:
	Engine::CTerrainTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	vector<int>											m_vecTextureNumber;
};
