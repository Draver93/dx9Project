#ifndef C_MOUSE_H
#define C_MOUSE_H

#include <Windows.h>
#include "../../headers/directx.h"
#include "../cWindow.h"

class cMouse
{
public:
	DIMOUSESTATE state;
private:
	cWindow *pWindow;
	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pMouse;
public:
	cMouse(cWindow *pWnd);
	~cMouse();
	HRESULT update();
};

#endif // !C_MOUSE_H
