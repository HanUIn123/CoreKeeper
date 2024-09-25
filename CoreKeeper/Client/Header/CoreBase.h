#pragma once
#include "Building.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;


END


class CCoreBase : public CBuilding
{
private:
	explicit						CCoreBase(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CCoreBase();
public:
	virtual			HRESULT			Ready_GameObject(_float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }
	wstring							Get_PickedBuildingName() { return m_strPickedBuildingName; }

	virtual			_vec3			Get_ObjectPos() { return m_vBuildPosition; }
	virtual			void			Set_ObjectPos(_vec3 _iBuildingPos) { m_vBuildPosition = _iBuildingPos; }

private:
	HRESULT							Add_Component();

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName);

private:
	virtual void					Free();
};

