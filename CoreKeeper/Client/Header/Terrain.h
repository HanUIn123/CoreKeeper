#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTerrainTex;
class CTransform;
class CTexture;
class CCalculator;

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
	vector<int>											Get_TextureNumber() { return m_vecTextureNumber; }
	void												Set_TextureNumber(int _iIndex, int _iNum) { m_vecTextureNumber[_iIndex] = _iNum; }
	void                                                Set_TextureNumber(vector<int> _vecTextureNumber) { copy(_vecTextureNumber.begin(), _vecTextureNumber.end(), m_vecTextureNumber.begin());}

	vector<bool>										Get_Unreachable() { return m_vecUnreachable; }
	bool												Get_UnreachableByIndex(int _iIndex) { return m_vecUnreachable[_iIndex]; }
	void												Set_Unreachable(int _iIndex, bool _bUnreachable) { m_vecUnreachable[_iIndex] = _bUnreachable; }
	void                                                Set_Unreachable(vector<bool> _vecUnreachable) { copy(_vecUnreachable.begin(), _vecUnreachable.end(), m_vecUnreachable.begin()); }
	_vec3*												Get_PickPos();
private:
	Engine::CTerrainTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;

public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();

private:
	vector<int>											m_vecTextureNumber;
	vector<bool>										m_vecUnreachable;
	_vec3												m_vPickPos;
};
