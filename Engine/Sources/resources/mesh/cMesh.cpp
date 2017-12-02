#include "cMesh.h"

cMesh::cMesh(cResources *pRes, ResMemory flag)
{
	if (pRes == NULL) return;
	type = flag;
	pResources = pRes;
	pResources->addResource(this);
}
cMesh::~cMesh()
{
	pResources->removeResource(this);
	release();
}

HRESULT cMesh::setBuffer(std::vector<VERTEX> *rawVBuffer)
{
	if (rawVBuffer == NULL ||
		rawVBuffer->empty()||
		pResources == NULL ||
		direct3DVertexBuffer != NULL) return E_FAIL;

	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();
	if (pDevice == NULL) return E_FAIL;

	HRESULT hr = pDevice->CreateVertexBuffer(rawVBuffer->size() * sizeof(VERTEX), 0, NULL, D3DPOOL_MANAGED, &direct3DVertexBuffer, NULL);
	if (hr != S_OK) 
		return hr;

	VOID* pBV;
	hr = direct3DVertexBuffer->Lock(0, 0, (void**)&pBV, 0);
	memcpy(pBV, rawVBuffer->data(), rawVBuffer->size() * sizeof(VERTEX));
	hr = direct3DVertexBuffer->Unlock();
	if (hr != S_OK)
		return hr;
	rawVertexBuffer = *rawVBuffer;
	D3DVERTEXBUFFER_DESC vb_d;
	direct3DVertexBuffer->GetDesc(&vb_d);
	indexed = false;

	return S_OK;
}
HRESULT cMesh::setBuffer(std::vector<VERTEX>* rawVBuffer, std::vector<WORD>* rawIBuffer)
{
	if (rawVBuffer == NULL ||
		rawVBuffer->empty()||
		rawIBuffer == NULL ||
		rawIBuffer->empty()||
		pResources == NULL ||
		direct3DVertexBuffer != NULL ||
		direct3DIndexBuffer != NULL) return E_FAIL;

	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();
	if (pDevice == NULL) return E_FAIL;

	HRESULT hr = pDevice->CreateVertexBuffer(rawVBuffer->size() * sizeof(VERTEX), 0, NULL, D3DPOOL_MANAGED, &direct3DVertexBuffer, NULL);
	if (hr != S_OK) return hr;

	VOID* pBV;
	direct3DVertexBuffer->Lock(0, 0, (void**)&pBV, 0);
	memcpy(pBV, rawVBuffer->data(), rawVBuffer->size() * sizeof(VERTEX));
	direct3DVertexBuffer->Unlock();

	pDevice->CreateIndexBuffer(rawIBuffer->size() * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &direct3DIndexBuffer, NULL);

	VOID* pBI;
	direct3DIndexBuffer->Lock(0, 0, (void**)&pBI, 0);
	memcpy(pBI, rawIBuffer->data(), rawIBuffer->size() * sizeof(WORD));
	direct3DIndexBuffer->Unlock();

	indexed = true;
	rawVertexBuffer = *rawVBuffer;
	rawIndexBuffer = *rawIBuffer;
	return S_OK;
}

HRESULT cMesh::getBuffer(LPDIRECT3DVERTEXBUFFER9 &vb, DWORD *vb_size)
{
	if (indexed || direct3DVertexBuffer == NULL) return E_FAIL;

	*vb_size = rawVertexBuffer.size();
	vb = direct3DVertexBuffer;
	return S_OK;
}
HRESULT cMesh::getBuffer(LPDIRECT3DVERTEXBUFFER9 &vb, DWORD *vb_size, LPDIRECT3DINDEXBUFFER9 &ib, DWORD *ib_size)
{
	if (!indexed ||
		direct3DVertexBuffer == NULL ||
		direct3DIndexBuffer == NULL) return E_FAIL;

	*vb_size = rawVertexBuffer.size();
	*ib_size = rawIndexBuffer.size();

	vb = direct3DVertexBuffer;
	ib = direct3DIndexBuffer;
	return S_OK;
}

HRESULT cMesh::setStream()
{
	HRESULT hr = S_OK;
	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();

	if (indexed && direct3DVertexBuffer != NULL && direct3DIndexBuffer != NULL)
	{
		hr = pDevice->SetStreamSource(0, direct3DVertexBuffer, 0, sizeof(VERTEX));
		hr = pDevice->SetIndices(direct3DIndexBuffer);
	}
	else if (direct3DVertexBuffer != NULL)
	{
		hr = pDevice->SetStreamSource(0, direct3DVertexBuffer, 0, sizeof(VERTEX));
	}
	return hr;
}

HRESULT cMesh::draw()
{
	HRESULT hr = S_OK;
	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();

	if (indexed && direct3DVertexBuffer != NULL && direct3DIndexBuffer != NULL)
	{
		hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, rawVertexBuffer.size(), 0, rawIndexBuffer.size() / 3);
	}
	else if (direct3DVertexBuffer != NULL)
	{
		hr = pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, rawVertexBuffer.size() / 3);
	}

	return hr;
}

HRESULT cMesh::release()
{
	return S_OK;
}
HRESULT cMesh::recreate()
{
	return S_OK;
}
