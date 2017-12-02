#include "cKeyboard.h"

cKeyboard::cKeyboard(cWindow *pWnd)
{
	if (pWnd == NULL)return;
	pWindow = pWnd;
	HRESULT hr = S_OK;
	hr = DirectInput8Create(pWnd->getHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL);
	hr = pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL);
	hr = pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = pKeyboard->SetCooperativeLevel(pWindow->getHWND(), DISCL_FOREGROUND);
	hr = pKeyboard->Acquire();
	if (hr != S_OK)
		return;
}

cKeyboard::~cKeyboard()
{
	if (pInput != NULL)
	{
		pInput->Release(); 
		pInput = NULL;
	}
	if (pKeyboard != NULL)
	{
		pKeyboard->Release();
		pKeyboard = NULL;
	}
}

HRESULT cKeyboard::update()
{
	if (pKeyboard == NULL) return E_FAIL;
	return pKeyboard->GetDeviceState(sizeof(state), (LPVOID)&state);
}
