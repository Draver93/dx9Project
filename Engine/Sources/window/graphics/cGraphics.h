#ifndef C_GRAPHICS_H
#define C_GRAPHICS_H

#include <Windows.h>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <chrono>

#include "../../headers/directx.h"
#include "../cWindow.h"

#include "../../resources/cResources.h"
#include "../../resources/model/cModel.h"


enum View { VIEW_2D, VIEW_3D };

struct sCamera
{
	D3DXMATRIX pMatrix, vMatrix, wMatrix;
	View view;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 look;
	D3DXVECTOR3 up;
	D3DXVECTOR2 angle;
	float sensetive, speed;
	std::chrono::high_resolution_clock::time_point timeLast;
	sCamera()
	{
		timeLast = std::chrono::high_resolution_clock::now();
		D3DXMatrixIdentity(&pMatrix);
		D3DXMatrixIdentity(&vMatrix);
		D3DXMatrixIdentity(&wMatrix);
		pos.x = pos.y = pos.z = 0;
		look.x = look.y = look.z = 0;
		angle.x = angle.y = 0;
		up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		sensetive = 0.1f;
		speed = 0.01f;
	}
	~sCamera()
	{

	}
	void manualControl(char keyState[256], DIMOUSESTATE *pMouse)
	{
		std::chrono::high_resolution_clock::time_point timeNew = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::microseconds>(timeNew - timeLast).count() / 1000.0f; //Time ms

		D3DXVECTOR2 rad;

		angle.x += (float)pMouse->lX;
		angle.y += (float)pMouse->lY;

		rad.x = angle.x * (D3DX_PI / 360);

		if (angle.y > 359)angle.y = 359;
		if (angle.y < 1) angle.y = 1;

		rad.y = angle.y * (D3DX_PI / 360);

		if (KEYDOWN(keyState, DIK_W))
		{
			pos.x += (time * sin(rad.y)*sin(rad.x))* speed;
			pos.y += (time * cos(rad.y))* speed;
			pos.z += (time * sin(rad.y)*cos(rad.x))* speed;
		}
		if (KEYDOWN(keyState, DIK_S))
		{
			pos.x -= (time * sin(rad.y)*sin(rad.x))* speed;
			pos.y -= (time * cos(rad.y))* speed;
			pos.z -= (time * sin(rad.y)*cos(rad.x))* speed;
		}
		if (KEYDOWN(keyState, DIK_A))
		{
			pos.x += (time * sin(rad.x - (180 * (D3DX_PI / 360))))* speed;
			pos.z += (time * cos(rad.x - (180 * (D3DX_PI / 360))))* speed;
		}
		if (KEYDOWN(keyState, DIK_D))
		{
			pos.x -= (time * sin(rad.x - (180 * (D3DX_PI / 360))))* speed;
			pos.z -= (time * cos(rad.x - (180 * (D3DX_PI / 360))))* speed;
		}

		look.x = pos.x + 1.0f * sin(rad.y)*sin(rad.x);
		look.y = pos.y + 1.0f * cos(rad.y);
		look.z = pos.z + 1.0f * sin(rad.y)*cos(rad.x);

		timeLast = std::chrono::high_resolution_clock::now();
	}
	void update(float width, float height, View view, float fov = 90)
	{
		D3DXMatrixIdentity(&vMatrix);
		D3DXMatrixIdentity(&pMatrix);
		if (view == VIEW_3D)
		{
			D3DXMatrixPerspectiveFovLH(&pMatrix, fov * (D3DX_PI / 180), width / height, 1.0f, 1000.0f);
			D3DXMatrixLookAtLH(&vMatrix,	&D3DXVECTOR3(pos.x, pos.y, pos.z),
											&D3DXVECTOR3(look.x, look.y, look.z),
											&up);
		}
		else
		{											
			D3DXMatrixOrthoLH(&pMatrix, width, height, 0.0f, 1.0f); 
			D3DXMatrixLookAtLH(&vMatrix,	&D3DXVECTOR3(0.0f, 0.0f, 0.0f),//
											&D3DXVECTOR3(0.0f, 0.0f, 1.0f), // Определяет только вектор направления взгляда смотит по Z оси
											&up);//
		}
	}
};

class cGraphics
{
public:
	cModel *pDisplay;
private:

	sCamera camera;
	LPDIRECT3D9 context;
	D3DDISPLAYMODE displayMode;
	LPDIRECT3DDEVICE9 pDevice;
	D3DPRESENT_PARAMETERS gParameters;
	PDIRECT3DVERTEXDECLARATION9 declaration;

	cResources *resources;
	cWindow *pWindow;
	LPDIRECT3DSURFACE9 pBackBuffer;
	cModel* createRndPanel(D3DXVECTOR2 pt);
public:
	cGraphics(cWindow *pWnd, D3DPRESENT_PARAMETERS *gp = NULL);
	~cGraphics();
	HRESULT display(LPDIRECT3DTEXTURE9 pTexture);

	HRESULT reСreate(cWindow *pWnd = NULL, D3DPRESENT_PARAMETERS *gp = NULL);
	HRESULT edit(D3DPRESENT_PARAMETERS gp);

	cResources *getResources() { return resources; }
	LPDIRECT3DDEVICE9 getDevice() { return pDevice; }
	LPDIRECT3DSURFACE9 getBackBuffer() { return pBackBuffer; }

	D3DPRESENT_PARAMETERS getGParameters() { return gParameters; }
	cWindow* getWindow() { return pWindow; }
	sCamera* getCamera() { return &camera; }
};




#endif // !C_GRAPHICS_H
