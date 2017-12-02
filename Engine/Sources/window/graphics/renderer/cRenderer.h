#ifndef C_RENDERER_H
#define C_RENDERER_H

#include "../cGraphics.h"
#include "../../../resources/cResources.h"
#include "../../../resources/model/scene/cScene.h"
#include "../../../resources/shader/cShader.h"

enum renderType { RT_FORWARD, RT_DEFERRED };
enum renderState { RS_POSITION, RS_NORMAL, RS_COLOR, RS_DEPTH };
enum lightType {LT_};

struct sObject
{
	D3DXMATRIX mV, mW, mP;
	cModel *model;
};

struct sLight
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 look;
	D3DXVECTOR3 up;
	float fov; // Angle
	float color[4];
	float distance; 
	float zNear, zFar;
	sLight()
	{
		zNear = 1.0f;
		zFar = 1000.0f;
		pos.x = pos.y = pos.z = 0.0f;
		look.x = look.y = look.z = 0.0f;
		up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		fov = 90.0f;
		distance = 0.0f;
		color[0] = color[1] = color[2] = color[3] = 1.0f;
	}
	D3DXMATRIX getMatrix(float width, float height)
	{
		D3DXMATRIX mxV, mxP;
		D3DXMatrixLookAtLH(&mxV, &pos, &look, &up);

		D3DXMatrixPerspectiveFovLH(&mxP, fov * (D3DX_PI / 180), width / height, zNear, zFar);
		return mxV * mxP;
	}
};

class cRenderer
{
public:
	std::vector<sLight> lights;
	std::vector<sObject> queue;
	cEffect* shadowMapFx;
private:
	cGraphics *pGraphics;
	renderType type;
	LPDIRECT3DTEXTURE9 renderTarget;
	LPDIRECT3DSURFACE9 rtStancil;

	LPDIRECT3DTEXTURE9 finalTarget;

	float smSize;
	LPDIRECT3DTEXTURE9 shadowMap;
	//LPDIRECT3DCUBETEXTURE9 shadowMapCube;
	LPDIRECT3DTEXTURE9 finalShadowMap;
	LPDIRECT3DSURFACE9 smStancil;


	cShader* normalSh;
	cShader* depthSh;
	cShader* posSh;

	int(*renderFunction)();
	std::thread *gThread;
	HRESULT setRndState(renderState state);
	bool closeLoop;

	HRESULT createShadowMap(sLight *light);
	HRESULT drawShadowMap(sLight *light);
	HRESULT renderObjects();
public:
	cRenderer(cGraphics *pRes, int(*rf)(), renderType flag);
	~cRenderer();
	HRESULT setRenderFunc(int(*rf)()) { if (rf != NULL) { renderFunction = rf; } else { return E_FAIL; } return S_OK; }
	int graphicLoop();

	HRESULT draw(cScene *scene);
	HRESULT draw(cModel *model);

};

#endif // !C_RENDERER_H
