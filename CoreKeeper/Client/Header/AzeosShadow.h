#pragma once
#include "Object.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CTexture;
class CColliderCube;

END

class CAzeosShadow : public CObject
{
private:
	explicit						CAzeosShadow(LPDIRECT3DDEVICE9 _pGraphicDev);
	virtual							~CAzeosShadow();

public:
	virtual			HRESULT			Ready_GameObject(_vec3 _vPos);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	//_int							Get_ObjectType() { return AZEOS_POOP; }

	_int							Get_ShadowTextureNumber() { return m_iShadowTextureNumber; }
	void							Set_ShadowTextureNumber(_int _iShadowTextureNumber) { m_iShadowTextureNumber = _iShadowTextureNumber; }

	//_vec3							Get_PoopPos() { return m_vPoopPos; }

	_float							Get_AppearTime() { return m_fAppearTime; }
	void							Set_ApeearTime(_float _fAppearTime) { m_fAppearTime = _fAppearTime; }
	void							StartCount_AppearTime(_float _fAppearTime) { m_fAppearTime += _fAppearTime; }

public:
	bool							Get_Show() { return m_bShow; }
	void							Set_Show(bool _bShow) { m_bShow = _bShow; }

	void							Set_Direction();
	
private:
	HRESULT							Add_Component();
public:
	static CAzeosShadow*			Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 _vPos);

private:
	virtual void					Free();

	Engine::AZEOSSHADOW_DIRECTION	m_eShadowDirection;

	_int							m_iShadowTextureNumber;
	_vec3							m_vShadowPos;
	bool							m_bInFrustum;
	bool							m_bShow;
	_vec3							m_vMoveDirection;

	_float							m_fSpeed;
	_float							m_fRotationAngle;
	_float							m_fCoolTime;
	_float							m_fAppearTime;


	_bool							m_bChangePattern;



};


