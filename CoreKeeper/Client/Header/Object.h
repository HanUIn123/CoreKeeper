#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;

END

class CObject : public Engine::CGameObject
{
protected:
	explicit						CObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CObject();

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
public:
	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

protected:
	Engine::CObjectTex*				m_pBufferCom;
	Engine::CTransform*				m_pTransformCom;
	Engine::CCalculator*			m_pCalculCom;
	Engine::CTexture*				m_pTextureCom;

public:
	static CObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void					Free();

protected:
	_int							m_iBuildingImgNum;
	_vec3							m_vBuildPosition;
};

