#ifndef C_MESH_H
#define C_MESH_H

#include <Windows.h>
#include <vector>
#include <string>

#include "../cResources.h"

struct VERTEX
{
	FLOAT x, y, z;
	FLOAT nx, ny, nz;
	FLOAT tu, tv;
	FLOAT weights[4];
	FLOAT indeces[4];

	VERTEX()
	{
		x = 0.0f, y = 0.0f, z = 0.0f;
		nx = 0.0f, ny = 0.0f, nz = 0.0f;
		tu = 0.0f, tv = 0.0f;
		weights[0] = 0.0f;
		weights[1] = 0.0f;
		weights[2] = 0.0f;
		weights[3] = 0.0f;
		indeces[0] = 0.0f;
		indeces[1] = 0.0f;
		indeces[2] = 0.0f;
		indeces[3] = 0.0f;
	}
};

class cMesh : cVirtualResource
{
public:
	bool indexed;
private:
	cResources *pResources;
	ResMemory type;

	std::string name;
	std::vector<VERTEX> rawVertexBuffer;
	std::vector<WORD> rawIndexBuffer;

	LPDIRECT3DVERTEXBUFFER9 direct3DVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	direct3DIndexBuffer;
public:
	cMesh(cResources *pRes, ResMemory flag);
	~cMesh();
	HRESULT setName(std::string meshName) { if (!meshName.empty())name = meshName; else name = "NONE"; return S_OK; };

	HRESULT setBuffer(std::vector<VERTEX> *rawVBuffer);
	HRESULT setBuffer(std::vector<VERTEX> *rawVBuffer, std::vector<WORD> *rawIBuffer);

	HRESULT getBuffer(LPDIRECT3DVERTEXBUFFER9 &vb, DWORD *vb_size);
	HRESULT getBuffer(LPDIRECT3DVERTEXBUFFER9 &vb, DWORD *vb_size, LPDIRECT3DINDEXBUFFER9 &ib, DWORD *ib_size);

	HRESULT setStream();
	HRESULT draw();


	HRESULT release();
	HRESULT recreate();
	ResMemory getMemoryType() { return type; };
};

#endif // !C_MESH_H
