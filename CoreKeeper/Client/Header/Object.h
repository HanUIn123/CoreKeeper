#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CColliderCube;
class CCollider;
class CInventory;

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

public:
	virtual			void			Interaction();

protected:
	HRESULT                         Setup_Material();

private:
	HRESULT							Add_Component();

public:
	bool							Check_Interaction();


public:
	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }
public:
	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	// Delete_GameObject 함수를 사용하려면, 키값이 필요한데, 그 키값(이름)을 받아올 함수
	virtual wstring					Get_PickedObjectName() { return m_strPickedObjectName; }
	virtual void					Set_PickedObjectName(wstring _pickObjectName) { m_strPickedObjectName = _pickObjectName; }

protected:
	void			Set_SoundVolumeByDistance();


protected:
	Engine::CObjectTex*				m_pBufferCom;
	Engine::CTransform*				m_pTransformCom;
	Engine::CCalculator*			m_pCalculCom;
	Engine::CTexture*				m_pTextureCom;
	//Engine::CCollider*				m_pColliderCom;
	Engine::CColliderCube*				m_pColliderCom;
	Engine::CInventory*				m_pInventoryCom;

	Engine::PLACEOBJECT				m_eObjType;


public:
	static CObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void					Free();

protected:
	_int							m_iBuildingImgNum;
	_vec3							m_vBuildPosition;
	CTransform*						m_pPlayerTransform;

	_float							m_fSoundVolume;
	_bool                           m_bCollision; // 플레이어와 충돌 체크

	wstring							m_strPickedObjectName;

};

