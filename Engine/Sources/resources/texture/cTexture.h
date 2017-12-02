#ifndef C_TEXTURE_H
#define C_TEXTURE_H

#include <Windows.h>
#include <string>

#include "../cResources.h"

class cTexture : cVirtualResource
{
private:
	cResources *pResources;
	std::string name;
	std::string path;
	LPDIRECT3DTEXTURE9 direct3DTexture;
	ResMemory type;

public:
	cTexture(cResources *pRes, ResMemory flag);
	~cTexture();
	HRESULT load(std::string filePath);
	HRESULT release();
	HRESULT recreate();
	ResMemory getMemoryType() {	return type; };
};

#endif // !C_TEXTURE_H
