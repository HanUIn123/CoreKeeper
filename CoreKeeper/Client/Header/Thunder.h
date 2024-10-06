#pragma once
#include "Object.h"
#include "..\Header\Player.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;
class CTexture;
class CInventory;
class CColliderCube;
class CAnimTex;
class CAnimator;

END

class CThunder : public CObject
{
public:
	enum THUNDER_TYPE { THUNDER_LINE, THUNDER_CIRCLE, THUNDER_RANDOM};
private:
	explicit						CThunder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CThunder();
public:
	virtual			HRESULT			Ready_GameObject(_vec3 vPos, THUNDER_TYPE eType);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			void			Interaction();


	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

	void							Set_Burn();

	void                            Set_Move();
private:
	HRESULT							Add_Component();

private:
	Engine::CAnimTex* m_pAnimBufferCom;
	Engine::CAnimTex* m_pElectAnimBufferCom;
	Engine::CAnimator* m_pAnimatorCom;
	Engine::CTexture* m_pElectricTextureCom;

public:
	static CThunder* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, THUNDER_TYPE eType);

private:
	virtual void					Free();

private:
	_bool m_bCollision;
	STATE m_eState;
	float	  m_fBurnTime;
	bool  m_bActive;
	bool  m_bBurn;

	CPlayer* m_pPlayer;

	_vec3 m_vPos;
	_vec3 m_vDir;

	THUNDER_TYPE m_eType;

	_bool m_bLR;

	STATE m_ePreState;
};

