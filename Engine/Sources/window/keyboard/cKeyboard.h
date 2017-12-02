#ifndef C_KEYBOARD_H
#define C_KEYBOARD_H

#include <Windows.h>
#include "../../headers/directx.h"
#include "../cWindow.h"

class cKeyboard
{
public:
	char state[256];
private:
	cWindow *pWindow;
	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pKeyboard;
public:
	cKeyboard(cWindow *pWnd);
	~cKeyboard();
	HRESULT update();
};

#endif // !C_KEYBOARD_H
