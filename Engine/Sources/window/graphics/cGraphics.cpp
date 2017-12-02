#include "cGraphics.h"


cGraphics::cGraphics(cWindow *pWnd, D3DPRESENT_PARAMETERS *gp)
{	
	if (pWnd == NULL || pWnd->getHWND() == NULL) return;

	pDevice = NULL;
	pWindow = pWnd;
	if (gp != NULL) gParameters = *gp;
	D3DCAPS9 pCaps;
	if (!(context = Direct3DCreate9(D3D_SDK_VERSION)))  return;
	context->GetDeviceCaps(0, D3DDEVTYPE_HAL, &pCaps);
	if (context->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)) return;


	//CONTEXT
	if (context->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pWindow->getHWND(),
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&gParameters, &pDevice)) return;

	//POINT TO BB
	pBackBuffer = NULL;
	pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);


	//DECL
	D3DVERTEXELEMENT9 pDecl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },

		D3DDECL_END()
	};
	pDevice->CreateVertexDeclaration(pDecl, &declaration);
	pDevice->SetVertexDeclaration(declaration);
	resources = new cResources(pDevice);


	//SCREEN
	D3DXVECTOR2 pt = { (float)gParameters.BackBufferHeight, (float)gParameters.BackBufferWidth };
	pDisplay = createRndPanel(pt);
	cEffect *effect = new cEffect("Effects/display.fx",resources);
	pDisplay->setEffect(effect);
	pDisplay->transform.angle.x = 90;
	pDisplay->transform.angle.z = 90;
	pDisplay->transform.pos.z = 0.1f;
	pDisplay->transform.size = 1.0f;

	pDisplay->getEffect()->getAttr("MatrixWorld")->setPtr(&camera.wMatrix);
	pDisplay->getEffect()->getAttr("MatrixProjection")->setPtr(&camera.pMatrix);
	pDisplay->getEffect()->getAttr("MatrixView")->setPtr(&camera.vMatrix);

}
cGraphics::~cGraphics()
{
	if (context != NULL)
	{
		context->Release();
		context = NULL;
	}
	if (pDevice != NULL)
	{
		pDevice->Release();
		pDevice = NULL;
	}
	if (resources != NULL)
	{
		delete resources;
		resources = NULL;
	}
}
HRESULT cGraphics::reСreate(cWindow * pWnd, D3DPRESENT_PARAMETERS * gp)
{
	if (pWindow == NULL ||
		pDevice == NULL)  return E_FAIL;

	if (pWnd != NULL) 
		pWindow = pWnd;
	if (gp != NULL) 
		gParameters = *gp;

	if (pDevice != NULL)
	{
		pDevice->Release(); 
		pDevice = NULL;
	}

	if (context->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pWindow->getHWND(),
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&gParameters, &pDevice)) 
	{ 
		return E_FAIL; 
	}

	return S_OK;
}
HRESULT cGraphics::edit(D3DPRESENT_PARAMETERS gp)
{
	if (pWindow == NULL || 
		pDevice == NULL)  return E_FAIL;

	HRESULT hr = pDevice->Reset(&gp);
	if (hr != S_OK)
	{
		hr = pDevice->TestCooperativeLevel();
		if (D3DERR_DEVICELOST == hr) return E_FAIL;
		if (D3DERR_DEVICENOTRESET == hr) return E_FAIL;
		if (D3DERR_DRIVERINTERNALERROR == hr) return E_FAIL;
	}
	gParameters = gp;

	return S_OK;
}
HRESULT cGraphics::display(LPDIRECT3DTEXTURE9 pTexture) //Производительность падает из за установки текстуры в шейдер
{
	camera.update(gParameters.BackBufferWidth, gParameters.BackBufferHeight, VIEW_2D);

	cMesh *mesh = pDisplay->getMesh();
	cEffect *effect = pDisplay->getEffect();
	HRESULT hr = S_OK;
	hr = mesh->setStream();

	std::vector<sTechnique> *pTechniques = effect->getTechniques();
	LPD3DXEFFECT data = effect->getData();
	for (UINT i = 0; i < pTechniques->size(); i++)
	{
		camera.wMatrix = pDisplay->transform.getMatrix();
		effect->getAttr("txrData")->setPtr(&pTexture);

		data->SetTechnique(pTechniques->at(i).hl);
		effect->setAllUniform();

		data->Begin(NULL, 0);
		for (UINT j = 0; j < pTechniques->at(i).desc.Passes; j++)
		{
			data->BeginPass(j);
				hr = mesh->draw();
			data->EndPass();
		}
		data->End();
	}
	return S_OK;
}

cModel* cGraphics::createRndPanel(D3DXVECTOR2 pt)
{
	POINT tmpPt;
	tmpPt.x = 0;
	tmpPt.y = 0;

	std::vector<VERTEX> tmpBuffer;
	VERTEX tmp;

	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);

	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);

	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);

	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);

	tmp.x = (-pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (-pt.y / 2) + tmpPt.y;
	tmp.tv = 0;
	tmp.tu = 0;
	tmpBuffer.push_back(tmp);

	tmp.x = (pt.x / 2) + tmpPt.x;
	tmp.y = 0.0f;
	tmp.z = (pt.y / 2) + tmpPt.y;
	tmp.tv = 1;
	tmp.tu = 1;
	tmpBuffer.push_back(tmp);

	cMesh *mesh = new cMesh(resources, RT_STATIC);
	mesh->setBuffer(&tmpBuffer);

	cModel *tmpModel = new cModel(resources, RT_STATIC);
	tmpModel->setMesh(mesh);
	return tmpModel;
}

