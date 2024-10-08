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
	virtual			HRESULT			Ready_GameObject(int _iStatueNum, int _iTextureNum, _vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

private:
	HRESULT							Add_Component();

protected:
	Engine::CObjectTex* m_pEmissiveBufferCom;
	Engine::CTexture* m_pEmissiveTextureCom;

public:
	static CBossSpawnPoint* Create(LPDIRECT3DDEVICE9 pGraphicDev, int _iStatueNum, int _iTextureNum, _vec3 vPos);

private:
	virtual void					Free();

private:
	int								m_iTextureNum;
	int								m_iStatueNum;
	bool							m_bActive;
};

