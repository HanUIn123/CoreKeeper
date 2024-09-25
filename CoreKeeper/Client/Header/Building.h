#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;

END

class CBuilding : public Engine::CGameObject
{
protected:
	explicit						CBuilding(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CBuilding();

public:
	virtual			HRESULT			Ready_GameObject();
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

private:
	HRESULT							Add_Component();
public:
	HRESULT							Setup_Material();
	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	// Delete_GameObject 함수를 사용하려면, 키값이 필요한데, 그 키값(이름)을 받아올 함수
	wstring							Get_PickedBuildingName() { return m_strPickedBuildingName; }
public:
	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

protected:
	Engine::CObjectTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CCalculator* m_pCalculCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void					Free();

protected:
	_int							m_iBuildingImgNum;
	wstring							m_strPickedBuildingName;
	_vec3							m_vBuildPosition;


};

