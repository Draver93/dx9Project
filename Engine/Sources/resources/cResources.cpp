#include "cResources.h"

cResources::cResources(LPDIRECT3DDEVICE9 pDevice)
{
	if (pDevice == NULL) return;

	pResDevice = pDevice;
}

cResources::~cResources()
{
}

HRESULT cResources::addResource(cVirtualResource * pRes)
{
	if (pRes == NULL) return E_FAIL;
	for (UINT i = 0; i < pResources.size(); i++)
		if (pRes == pResources.at(i)) return E_FAIL;

	pResources.push_back(pRes);
	return S_OK;
}
HRESULT cResources::removeResource(cVirtualResource * pRes)
{
	if (pRes == NULL) return E_FAIL;

	for (UINT i = 0; i < pResources.size(); i++)
		if (pRes == pResources.at(i)) 
		{ 
			pResources.erase(pResources.begin() + i); 
			return S_OK; 
		}

	return E_FAIL;
}

HRESULT cResources::addInterface(cVirtualInterface * pInterface)
{
	if (pInterface == NULL) return E_FAIL;
	for (UINT i = 0; i < pInterfaces.size(); i++)
		if (pInterface == pInterfaces.at(i)) return E_FAIL;

	pInterfaces.push_back(pInterface);
	return S_OK;
}
HRESULT cResources::removeInterface(cVirtualInterface * pInterface)
{
	if (pInterface == NULL) return E_FAIL;

	for (UINT i = 0; i < pInterfaces.size(); i++)
		if (pInterface == pInterfaces.at(i))
		{
			pInterfaces.erase(pInterfaces.begin() + i);
			return S_OK;
		}

	return E_FAIL;
}


