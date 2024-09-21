#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Define.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)		
	{ 
		return m_byKeyState[byKeyID]; 
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 
	{ 	
		return m_tMouseState.rgbButtons[eMouse];
	}

	_bool   Key_Down(_ubyte byKeyID)
	{
		if ((m_byOldKeyState[byKeyID] == false) && (m_byKeyState[byKeyID] & 0x80))
		{
			m_byOldKeyState[byKeyID] = true;
			return true;
		}
				
		return false;
	}

	_bool   Key_Up(_ubyte byKeyID)	
	{
		if ((m_byOldKeyState[byKeyID] == true) && (!(m_byKeyState[byKeyID] & 0x80)))
		{
			m_byOldKeyState[byKeyID] = false;
			return true;
		}

		return false;
	}

	_bool   Button_Down(MOUSEKEYSTATE eMouseState)
	{
		if (!(m_bOldMouseState[eMouseState]) && (m_tMouseState.rgbButtons[eMouseState] & 0x80))
		{
			m_bOldMouseState[eMouseState] = true;
			return true;
		}

		return false;
	}

	// 현재 마우스의 특정 축 좌표를 반환
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)		
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	
public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	_bool                   m_bOldMouseState[DIM_END];
	_bool                   m_byOldKeyState[256];
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	DIMOUSESTATE			m_tMouseState;	

public:
	virtual void	Free(void);

};
END
#endif // InputDev_h__
