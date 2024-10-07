#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CTexture;
class CColliderCube;

END
class CMushroom : public CObject
{
private:
	explicit    CMushroom(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual     ~CMushroom();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos, _int _iTypeNum);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	_int							Get_ObjectType() { return MUSHROOM; }

	_int							Get_TileTypeIndex() { return m_iMushTypeIndex; }
	void							Set_TileTypeIndex(_int _iTileTypeIndex) { m_iMushTypeIndex = _iTileTypeIndex; }
	
	_vec3							Get_MushPos() { return m_vMushroomPos; }
private:
	HRESULT							Add_Component();
public:
	static CMushroom*				Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int _iTypeNum);

private:
	virtual void					Free();
	_int							m_iMushTypeIndex;
	_vec3							m_vMushroomPos;
};

