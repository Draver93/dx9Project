#include "cTexture.h"

cTexture::cTexture(cResources * pRes, ResMemory flag)
{
	pResources = pRes;
	pResources->addResource(this);
	type = flag;
}

cTexture::~cTexture()
{
	pResources->removeResource(this);
	release();
}

HRESULT cTexture::load(std::string filePath)
{
	if (filePath.empty() || 
		pResources == NULL || 
		direct3DTexture != NULL) return E_FAIL;

	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();
	if(pDevice == NULL) return E_FAIL;

	path = filePath;

	D3DPOOL pool = D3DPOOL_DEFAULT;
	if (type == RT_STATIC) pool = D3DPOOL_MANAGED;
	else pool = D3DPOOL_DEFAULT;

	HRESULT hr = D3DXCreateTextureFromFileEx(pDevice, path.c_str(), 0, 0, 0, 0, D3DFMT_UNKNOWN, pool,
		D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, NULL, NULL, &direct3DTexture);

	return hr;
}

HRESULT cTexture::release()
{
	if (direct3DTexture != NULL)
	{
		direct3DTexture->Release();
		direct3DTexture = NULL;
	}
	else return E_FAIL;

	return S_OK;
}

HRESULT cTexture::recreate()
{
	if (direct3DTexture == NULL && !path.empty()) 
		load(path);
	else 
		return E_FAIL;

	return S_OK;
}

