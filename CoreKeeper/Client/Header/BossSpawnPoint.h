#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;

END

class CBossSpawnPoint : public CObject
{
private:
	explicit						CBossSpawnPoint(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CBossSpawnPoint();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos, _int _iTypeNum);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	_int							Get_TileTypeIndex() { return m_iSpawnTextureNumber; }
	void							Set_TileTypeIndex(_int _iSpawnTextureNumber) { m_iSpawnTextureNumber = _iSpawnTextureNumber; }

private:
	HRESULT							Add_Component();

protected:
	Engine::CObjectTex* m_pEmissiveBufferCom;
	Engine::CTexture* m_pEmissiveTextureCom;

public:
	static CBossSpawnPoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum);

private:
	virtual void					Free();

private:
	int								m_iTextureNum;
	bool							m_bActive;
	_int							m_iSpawnTextureNumber;

};

