#ifndef C_SHADER_H
#define C_SHADER_H

#include <Windows.h>
#include <string>

#include "../cResources.h"


enum ShaderType {ST_VERTEX, ST_PIXEL};

struct shaderV
{
	std::string filePath;
	LPDIRECT3DVERTEXSHADER9 direct3DVertexShader;
	LPD3DXCONSTANTTABLE pConstTable;

	void release()
	{
		filePath.clear();
		std::string().swap(filePath);
		if (direct3DVertexShader != NULL)
			direct3DVertexShader->Release();
		if (pConstTable != NULL)
			pConstTable->Release();
	}
};
struct shaderP
{
	std::string filePath;
	LPDIRECT3DPIXELSHADER9 direct3DPixelShader;
	LPD3DXCONSTANTTABLE pConstTable;

	void release()
	{
		filePath.clear();
		std::string().swap(filePath);
		if (direct3DPixelShader != NULL)
			direct3DPixelShader->Release();
		if (pConstTable != NULL)
			pConstTable->Release();
	}
};


class cShader : cVirtualResource
{
private:
	cResources *pResources;
	const ResMemory type = RT_STATIC;

	std::string name;
	shaderV vertexShader;
	shaderP pixelShader;

public:
	cShader(cResources *pRes);
	cShader(cResources *pRes, std::string vertexShaderPath, std::string pixelShaderPath);
	cShader(cResources *pRes, std::string shaderPath, ShaderType type);
	~cShader();

	HRESULT loadShader(std::string shaderPath, ShaderType type);

	shaderV* getVertexShader() { return &vertexShader; }
	shaderP* getPixelShader() { return &pixelShader; }

	HRESULT release();
	HRESULT recreate();
	ResMemory getMemoryType() { return type; };
};

#endif // !C_SHADER_H
