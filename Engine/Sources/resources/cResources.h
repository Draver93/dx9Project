#ifndef C_RESOURCES_H
#define C_RESOURCES_H

#include <string>
#include <vector>

#include "../headers/directx.h"

//Обьект данного класса хранит все ресурсы связанные с указанным графическим окном
enum ResMemory { RT_DINAMIC, RT_STATIC };

class cVirtualResource
{
public:
	virtual HRESULT release() = 0;
	virtual HRESULT recreate() = 0;
	virtual ResMemory getMemoryType() = 0;
};
class cVirtualInterface
{
public:
	virtual HRESULT onLostDevice() = 0;
	virtual HRESULT onResetDevice() = 0;
};

class cResources
{
private:
	//Буфферы и ресурсы девайса
	LPDIRECT3DDEVICE9 pResDevice;
	std::vector<cVirtualInterface*> pInterfaces;
	std::vector<cVirtualResource*> pResources;
public:
	cResources(LPDIRECT3DDEVICE9 pDevice);
	~cResources();
	HRESULT addResource(cVirtualResource* pRes);
	HRESULT removeResource(cVirtualResource* pRes);

	HRESULT addInterface(cVirtualInterface* pInterface);
	HRESULT removeInterface(cVirtualInterface* pInterface);
	LPDIRECT3DDEVICE9 getDevice() { return pResDevice; }
};



#endif // !C_RESOURCES_H
