#include "cRenderer.h"


cRenderer::cRenderer(cGraphics *pGWnd, int(*rf)(), renderType flag)
{
	if (pGWnd == NULL) return;
	pGraphics = pGWnd;
	renderFunction = rf;
	closeLoop = FALSE;
	type = flag;

	normalSh = NULL;
	depthSh = NULL;
	posSh = NULL;

	LPDIRECT3DDEVICE9 pDevice = pGraphics->getDevice();
	

	//FINAL
	pDevice->CreateTexture(pGraphics->getGParameters().BackBufferWidth, pGraphics->getGParameters().BackBufferHeight, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &finalTarget, NULL);

	//DEFERRED
	renderTarget = NULL;
	if (flag == RT_DEFERRED)
	{
		//RENDER
		pDevice->CreateTexture(pGraphics->getGParameters().BackBufferWidth * 2, pGraphics->getGParameters().BackBufferHeight * 2, 1,
			D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &renderTarget, NULL);

		pDevice->CreateDepthStencilSurface(pGraphics->getGParameters().BackBufferWidth * 2, pGraphics->getGParameters().BackBufferHeight * 2, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &rtStancil, NULL);

		//SH
		int scale = 1;
		smSize = 500;
		pDevice->CreateTexture(smSize * 3, smSize * 2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &shadowMap, NULL);
		pDevice->CreateDepthStencilSurface(smSize * 3, smSize * 2, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &smStancil, NULL);

		pDevice->CreateTexture(pGraphics->getGParameters().BackBufferWidth / scale, pGraphics->getGParameters().BackBufferHeight / scale, 1,
			D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &finalShadowMap , NULL);

		normalSh = new cShader(pGWnd->getResources(), "Shaders/normal.ps", ST_PIXEL);
		depthSh = new cShader(pGWnd->getResources(), "Shaders/depth.ps", ST_PIXEL);
		posSh = new cShader(pGWnd->getResources(), "Shaders/position.ps", ST_PIXEL);
		shadowMapFx = new cEffect("Effects/shadowMap.fx", pGWnd->getResources());
	}

	//THREAD
	gThread = new std::thread(
	[](cRenderer *Rnd)
	{
		Rnd->graphicLoop();
	}
	, this);
	gThread->detach();

}
cRenderer::~cRenderer()
{
	closeLoop = TRUE;
}

HRESULT cRenderer::setRndState(renderState state)
{
	LPDIRECT3DDEVICE9 pDevice = pGraphics->getDevice();

	D3DVIEWPORT9 vp;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;

	switch (state)
	{
	case RS_COLOR:
	{
		vp.X = 0;
		vp.Y = 0;
		vp.Height = pGraphics->getGParameters().BackBufferHeight;
		vp.Width = pGraphics->getGParameters().BackBufferWidth;

		pDevice->SetViewport(&vp);
		break;
	}
	case RS_NORMAL:
	{
		vp.X = pGraphics->getGParameters().BackBufferWidth;
		vp.Y = 0;
		vp.Height = pGraphics->getGParameters().BackBufferHeight;
		vp.Width = pGraphics->getGParameters().BackBufferWidth;
		pDevice->SetViewport(&vp);

		pDevice->SetPixelShader(normalSh->getPixelShader()->direct3DPixelShader);
		break;
	}
	case RS_POSITION:
	{
		vp.X = 0;
		vp.Y = pGraphics->getGParameters().BackBufferHeight;
		vp.Height = pGraphics->getGParameters().BackBufferHeight;
		vp.Width = pGraphics->getGParameters().BackBufferWidth;
		pDevice->SetViewport(&vp);

		pDevice->SetPixelShader(posSh->getPixelShader()->direct3DPixelShader);
		break;
	}
	case RS_DEPTH:
	{
		vp.X = pGraphics->getGParameters().BackBufferWidth;
		vp.Y = pGraphics->getGParameters().BackBufferHeight;
		vp.Height = pGraphics->getGParameters().BackBufferHeight;
		vp.Width = pGraphics->getGParameters().BackBufferWidth;
		pDevice->SetViewport(&vp);

		pDevice->SetPixelShader(depthSh->getPixelShader()->direct3DPixelShader);
		break;
	}
	}
	return S_OK;
}
HRESULT cRenderer::createShadowMap(sLight * light)
{
	sCamera				*camera = pGraphics->getCamera();
	LPDIRECT3DDEVICE9	pDevice = pGraphics->getDevice();
	cEffect				*effect = shadowMapFx;
	D3DXVECTOR3 nPos = light->pos;

	LPDIRECT3DSURFACE9 pFace;
	shadowMap->GetSurfaceLevel(0, &pFace);
	pDevice->SetRenderTarget(0, pFace);
	pDevice->SetDepthStencilSurface(smStancil);
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	D3DVIEWPORT9 vp;

	for (UINT f = 0; f < 6; f++)
	{
		switch (f)
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
			light->look = D3DXVECTOR3(light->pos.x + 1.0f, light->pos.y + 0.0f, light->pos.z + 0.0f);
			light->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_X:
			light->look = D3DXVECTOR3(light->pos.x - 1.0f, light->pos.y + 0.0f, light->pos.z + 0.0f);
			light->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Y:
			light->look = D3DXVECTOR3(light->pos.x + 0.0f, light->pos.y + 1.0f, light->pos.z + 0.0f);
			light->up = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Y:
			light->look = D3DXVECTOR3(light->pos.x + 0.0f, light->pos.y - 1.0f, light->pos.z + 0.0f);
			light->up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			break;
		case D3DCUBEMAP_FACE_POSITIVE_Z:
			light->look = D3DXVECTOR3(light->pos.x + 0.0f, light->pos.y + 0.0f, light->pos.z + 1.0f);
			light->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		case D3DCUBEMAP_FACE_NEGATIVE_Z:
			light->look = D3DXVECTOR3(light->pos.x + 0.0f, light->pos.y + 0.0f, light->pos.z - 1.0f);
			light->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			break;
		}
		int str = f / 3; 
		int column = f % 3;

		vp.MinZ = 0.0f;
		vp.MaxZ = 1.0f;
		vp.Y = str * smSize;
		vp.X = column * smSize;
		vp.Height = smSize;
		vp.Width = smSize;
		pDevice->SetViewport(&vp);

		D3DXMATRIX mVP = light->getMatrix(1, 1);

		//DRAW OBJ
		for (UINT i = 0; i < queue.size(); i++)
		{
			cMesh	*mesh = queue.at(i).model->getMesh();
			mesh->setStream();

			std::vector<sTechnique> *pTechniques = effect->getTechniques();
			LPD3DXEFFECT data = effect->getData();

			effect->getAttr("MatrixVP")->setPtr(&mVP);
			effect->getAttr("MatrixWorld")->setPtr(&queue.at(i).mW);
			effect->getAttr("dWidth")->setVar(pGraphics->getGParameters().BackBufferWidth);
			effect->getAttr("dHeight")->setVar(pGraphics->getGParameters().BackBufferHeight);
			std::vector<float> vec = {light->pos.x, light->pos.y, light->pos.z};
			effect->getAttr("lpos")->setMas(&vec);
			
			data->SetTechnique(effect->getTech("map")->hl);
			effect->setAllUniform();
			UINT pass = 0;
			data->Begin(&pass, 0);
			for (UINT k = 0; k < pass; k++)
			{
				data->BeginPass(k);
				mesh->draw();
				data->EndPass();
			}
			data->End();
		}
	}


	vp.X = 0;
	vp.Y = 0;
	vp.Height = pGraphics->getGParameters().BackBufferHeight;
	vp.Width = pGraphics->getGParameters().BackBufferWidth;
	pDevice->SetViewport(&vp);

	//pDevice->SetDepthStencilSurface(NULL);

	//D3DXSaveSurfaceToFileA("f.bmp", D3DXIFF_BMP, pFace, NULL, NULL);
	pFace->Release();
	return S_OK;
}
HRESULT cRenderer::drawShadowMap(sLight *light)
{
	sCamera				*camera = pGraphics->getCamera();
	LPDIRECT3DDEVICE9	pDevice = pGraphics->getDevice();

	HRESULT hr = S_OK;
	for (UINT i = 0; i < queue.size(); i++)
	{
		cMesh	*mesh = queue.at(i).model->getMesh();
		cEffect *effect = shadowMapFx;
		hr = mesh->setStream();

		std::vector<sTechnique> *pTechniques = effect->getTechniques();
		LPD3DXEFFECT data = effect->getData();

		light->look = D3DXVECTOR3(light->pos.x + 0.0f, light->pos.y + 0.0f, light->pos.z + 1.0f);
		light->up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		D3DXMATRIX mVP = light->getMatrix(1, 1);

		D3DXMATRIX mVPC = camera->vMatrix * camera->pMatrix;
		effect->getAttr("MatrixVP")->setPtr(&mVP);
		effect->getAttr("MatrixVPC")->setPtr(&mVPC);
		effect->getAttr("MatrixWorld")->setPtr(&queue.at(i).mW);
		effect->getAttr("txrData")->setPtr(&shadowMap);
		effect->getAttr("dWidth")->setVar(pGraphics->getGParameters().BackBufferWidth );
		effect->getAttr("dHeight")->setVar(pGraphics->getGParameters().BackBufferHeight );
		std::vector<float> vec = { light->pos.x, light->pos.y, light->pos.z };
		effect->getAttr("lpos")->setMas(&vec);
		
		data->SetTechnique(effect->getTech("draw")->hl);
		effect->setAllUniform();
		UINT pass = 0;
		data->Begin(&pass, 0);
		for (UINT k = 0; k < pass; k++)
		{
			data->BeginPass(k);
			hr = mesh->draw();
			data->EndPass();
		}
		data->End();

	}
	return S_OK;
}

HRESULT cRenderer::renderObjects()
{
	
	sCamera				*camera = pGraphics->getCamera();
	LPDIRECT3DDEVICE9	pDevice = pGraphics->getDevice();


	HRESULT hr = S_OK;
	for (UINT i = 0; i < queue.size(); i++)
	{
		cMesh	*mesh = queue.at(i).model->getMesh();
		cEffect *effect = queue.at(i).model->getEffect();
		hr = mesh->setStream();

		/*if (type == RT_FORWARD)
		{
			std::vector<sTechnique> *pTechniques = effect->getTechniques();
			LPD3DXEFFECT data = effect->getData();
			for (UINT i = 0; i < pTechniques->size(); i++)
			{
				camera->wMatrix = queue.at(j).mW;
				camera->vMatrix = queue.at(j).mV;
				camera->pMatrix = queue.at(j).mP;

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
		}
		else if (type == RT_DEFERRED)
		{}*/
		
		std::vector<sTechnique> *pTechniques = effect->getTechniques();
		LPD3DXEFFECT data = effect->getData();
		for (UINT j = 0; j < pTechniques->size(); j++)
		{
			camera->wMatrix = queue.at(i).mW;
			camera->vMatrix = queue.at(i).mV;
			camera->pMatrix = queue.at(i).mP;

			data->SetTechnique(pTechniques->at(j).hl);
			effect->setAllUniform();

			data->Begin(NULL, 0);
			for (UINT k = 0; k < pTechniques->at(j).desc.Passes; k++)
			{
				data->BeginPass(k);

				setRndState(RS_COLOR); // Must go first
				hr = mesh->draw();

				setRndState(RS_NORMAL);
				hr = mesh->draw();

				setRndState(RS_DEPTH);
				hr = mesh->draw();

				setRndState(RS_POSITION);
				hr = mesh->draw();

				data->EndPass();
			}
			data->End();
		}		
	}
	return S_OK;
}

int cRenderer::graphicLoop() //Эта функция на данный момент заточена пот отложенный рендеринг
{
	LPDIRECT3DDEVICE9 pDevice = pGraphics->getDevice();
	cWindow *pWindow = pGraphics->getWindow();
	LPDIRECT3DSURFACE9 pRTSurf;
	renderTarget->GetSurfaceLevel(0, &pRTSurf);
	//LPDIRECT3DSURFACE9 pSMSurf;
	//shadowMap->GetSurfaceLevel(0, &pSMSurf);
	LPDIRECT3DSURFACE9 pSMFSurf;
	finalShadowMap->GetSurfaceLevel(0, &pSMFSurf);

	LPDIRECT3DSURFACE9 pRTST;
	pDevice->GetDepthStencilSurface(&pRTST);


	while (pWindow->getState() != W_CLOSE && !closeLoop)
	{
		//SET TARGET

		pDevice->SetRenderTarget(0, pRTSurf);
		//pDevice->SetDepthStencilSurface(rtStancil);

		pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

		pDevice->BeginScene();

		if (renderFunction != NULL)
			renderFunction();

		renderObjects();

		//VP
		{
			D3DVIEWPORT9 vp;
			vp.MinZ = 0.0f;
			vp.MaxZ = 1.0f;
			vp.X = 0;
			vp.Y = 0;
			vp.Height = pGraphics->getGParameters().BackBufferHeight / 4;
			vp.Width = pGraphics->getGParameters().BackBufferWidth / 4;
			pDevice->SetViewport(&vp);
		}

		for (UINT i = 0; i < lights.size(); i++)
		{
			createShadowMap(&lights.at(i));

			pDevice->SetRenderTarget(0, pSMFSurf);
			pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			drawShadowMap(&lights.at(i));
		}
		pDevice->SetRenderTarget(0, pGraphics->getBackBuffer());
		//pDevice->SetDepthStencilSurface(pRTST);

		pGraphics->display(finalShadowMap);
		pDevice->EndScene();
		pDevice->Present(NULL, NULL, NULL, NULL);
		queue.clear();
	}
	pRTSurf->Release();
	//pSMSurf->Release();
	pSMFSurf->Release();
	delete gThread;
	gThread = NULL;
	return 0;
}

HRESULT cRenderer::draw(cScene * scene)
{
	for (UINT i = 0; i < scene->getModels()->size(); i++)
		draw(scene->getModels()->at(i));
	return S_OK;
}
HRESULT cRenderer::draw(cModel * model)
{
	sCamera	*camera = pGraphics->getCamera();

	sObject tmpObj;
	tmpObj.model = model;

	//TRANSFORM
	tmpObj.mW = model->transform.getMatrix();
	tmpObj.mV = camera->vMatrix;
	tmpObj.mP = camera->pMatrix;

	queue.push_back(tmpObj);
	return S_OK;
}
