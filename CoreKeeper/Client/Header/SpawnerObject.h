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

class CSpawnerObject : public CObject
{
private:
	explicit						CSpawnerObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CSpawnerObject();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos, _int _iTypeNum);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	_int							Get_SpawnTextureIndex() { return m_iTextureNumber; }
	void							Set_SpawnTextureIndex(_int _iTextureNumber) { m_iTextureNumber = _iTextureNumber; }

	_vec3							Get_SpawnPos() { return m_vMaluSpawnPos; }

	virtual			void			Interaction();
	_bool							Check_Object_Interaction();

private:
	HRESULT							Add_Component();

public:
	static CSpawnerObject*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum);

private:
	virtual void					Free();

private:
	_int							m_iTextureNumber;
	_vec3							m_vMaluSpawnPos;
	_bool							m_bIsAlreadySpawn;
};

