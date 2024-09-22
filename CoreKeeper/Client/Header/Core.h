#pragma once
#include "Building.h"

BEGIN(Engine)

class CObjectTex;
class CTransform;
class CCalculator;


END


class CCore : public CBuilding
{
private:
	explicit						CCore(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual							~CCore();
public:
	virtual			HRESULT			Ready_GameObject(_float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName);
	virtual			_int			Update_GameObject(const _float& fTimeDelta);
	virtual			void			LateUpdate_GameObject();
	virtual			void			Render_GameObject();

	virtual			_int			Get_BuildImgNum() { return m_iBuildingImgNum; }
	virtual			void			Set_BuildImgNum(_int _iBuildingNum) { m_iBuildingImgNum = _iBuildingNum; }
	wstring							Get_PickedBuildingName() { return m_strPickedBuildingName; }

private:
	HRESULT							Add_Component();

public:
	static CBuilding* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fX, _float fY, _bool bReposed, _int iBuildImgNum, const wstring _pickedBuildName);

private:
	virtual void					Free();
};

