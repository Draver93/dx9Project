#include "cMouse.h"

cMouse::cMouse(cWindow * pWnd)
{
	if (pWnd == NULL)return;
	pWindow = pWnd;
	HRESULT hr = S_OK;
	hr = DirectInput8Create(pWnd->getHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL);
	hr = pInput->CreateDevice(GUID_SysMouse, &pMouse, NULL);
	hr = pMouse->SetDataFormat(&c_dfDIMouse);
	hr = pMouse->SetCooperativeLevel(pWindow->getHWND(), DISCL_FOREGROUND);
	hr = pMouse->Acquire();
	if (hr != S_OK)
		return;
}

cMouse::~cMouse()
{
	if (pInput != NULL)
	{
		pInput->Release();
		pInput = NULL;
	}
	if (pMouse != NULL)
	{
		pMouse->Release();
		pMouse = NULL;
	}
}

HRESULT cMouse::update()
{
	if (pMouse == NULL) return E_FAIL;
	return pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&state);
}
