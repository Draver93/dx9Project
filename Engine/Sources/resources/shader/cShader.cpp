#include "cShader.h"

cShader::cShader(cResources * pRes)
{
	if (pRes == NULL) return;

	pResources = pRes;
	pResources->addResource(this);
}
cShader::cShader(cResources * pRes, std::string vertexShaderPath, std::string pixelShaderPath)
{
	if (pRes == NULL) return;

	pResources = pRes;
	pResources->addResource(this);

	loadShader(vertexShaderPath, ST_VERTEX);
	loadShader(pixelShaderPath, ST_PIXEL);
}
cShader::cShader(cResources * pRes, std::string shaderPath, ShaderType type)
{
	if (pRes == NULL) return;

	pResources = pRes;
	pResources->addResource(this);

	loadShader(shaderPath, type);
}
cShader::~cShader()
{
	if (vertexShader.pConstTable != NULL) { vertexShader.pConstTable->Release(); vertexShader.pConstTable = NULL; }
	if (vertexShader.direct3DVertexShader != NULL) { vertexShader.direct3DVertexShader->Release(); vertexShader.direct3DVertexShader = NULL; }
	if (pixelShader.pConstTable != NULL) { pixelShader.pConstTable->Release(); pixelShader.pConstTable = NULL; }
	if (pixelShader.direct3DPixelShader != NULL) { pixelShader.direct3DPixelShader->Release(); pixelShader.direct3DPixelShader = NULL; }
}

HRESULT cShader::loadShader(std::string shaderPath, ShaderType type)
{
	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();
	LPD3DXBUFFER pBuffer = NULL;

	if (type == ST_VERTEX)
	{
		vertexShader.filePath = shaderPath;
		if (vertexShader.pConstTable != NULL) { vertexShader.pConstTable->Release(); vertexShader.pConstTable = NULL; }
		if (vertexShader.direct3DVertexShader != NULL) { vertexShader.direct3DVertexShader->Release(); vertexShader.direct3DVertexShader = NULL; }

		D3DXCompileShaderFromFile(shaderPath.c_str(), 0, 0, "vs_main", "vs_3_0", NULL, &pBuffer, NULL, &vertexShader.pConstTable);
		pDevice->CreateVertexShader((DWORD*)pBuffer->GetBufferPointer(), &vertexShader.direct3DVertexShader);
	}
	else if (type == ST_PIXEL)
	{
		pixelShader.filePath = shaderPath;
		if (pixelShader.pConstTable != NULL) { pixelShader.pConstTable->Release(); pixelShader.pConstTable = NULL; }
		if (pixelShader.direct3DPixelShader != NULL) { pixelShader.direct3DPixelShader->Release(); pixelShader.direct3DPixelShader = NULL; }

		D3DXCompileShaderFromFile(shaderPath.c_str(), 0, 0, "ps_main", "ps_3_0", NULL, &pBuffer, NULL, &pixelShader.pConstTable);
		pDevice->CreatePixelShader((DWORD*)pBuffer->GetBufferPointer(), &pixelShader.direct3DPixelShader);
	}
	pBuffer->Release();
	return S_OK;
}

HRESULT cShader::release()
{
	return E_NOTIMPL;
}
HRESULT cShader::recreate()
{
	return E_NOTIMPL;
}
