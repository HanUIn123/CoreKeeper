#pragma once
#include "Object.h"
#include "Scene.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CInventory;

END

class CBoxObject : public CObject
{
private:
	explicit						CBoxObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CBoxObject();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	virtual         void            Interaction();

public:
	void							SetUp_Item(CScene* _pScene);
	void							Set_Special() { m_bSpecial = true; m_iTextureNum = 1; }
private:
	HRESULT							Add_Component();

public:
	static CBoxObject* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);

private:
	virtual void					Free();

	bool							m_bSpecial;
	int								m_iTextureNum;
	vector<wstring>					m_vecItemName;
};

