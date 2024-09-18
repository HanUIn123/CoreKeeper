#pragma once
#include "GameObject.h"
#include "Define.h"
#include "Export_System.h"

BEGIN(Engine)

class CTransform;
class CTexture;
class CCalculator;
class CWallTex;

END

class CWall : public Engine::CGameObject
{
private:
	explicit												CWall(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual													~CWall();

public:
	virtual			HRESULT									Ready_GameObject(_float _fWallX, _float _fWallZ, _int iWallImageNum);
	virtual			_int									Update_GameObject(const _float& fTimeDelta);
	virtual			void									LateUpdate_GameObject();
	virtual			void									Render_GameObject();

	_vec3													Get_WallPos() { return m_vWallPosition; }
	void													Set_WallPos(_vec3 _iWallPos) { m_vWallPosition = _iWallPos; }

	_int													Get_WallNumber() { return m_iWallImageNum; }
	void													Set_WallNumber(_int _iWallNum) { m_iWallImageNum = _iWallNum; }

private:
	HRESULT													Add_Component();

private:
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;
	Engine::CCalculator* m_pCalculatorCom;
	Engine::CWallTex* m_pBufferCom;

public:
	static CWall* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float _fWallX, _float _fWallZ, _int iWallImageNum);

private:
	virtual void											Free();

	_vec3													m_vWallPosition;
	_int													m_iWallImageNum;
};

