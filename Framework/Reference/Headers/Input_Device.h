#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum class MOUSEKEY_STATE { LBUTTON, RBUTTON, WHEELBUTTON, ENUM_END };
	enum class MOUSEMOVE_STATE { X, Y, WHEEL, ENUM_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_bool	Key_Pressing(_int _iKey);
	_bool	Key_Down(_int _iKey);
	_bool	Key_Up(_int _iKey);

	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEY_STATE eState) { return m_tMouseState.rgbButtons[_uint(eState)]; }
	_long	Get_DIMouseMove(MOUSEMOVE_STATE eState) { return *(((_long*)&m_tMouseState) + _uint(eState)); }
	_bool	Is_MosueMove();

public:
	HRESULT	Reserve_InputDevice(HINSTANCE hInst, HWND hWnd);
	void	Tick();

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];
	DIMOUSESTATE			m_tMouseState;

	_bool					m_bKeyState[VK_MAX];

public:
	virtual void	Free();



};
END