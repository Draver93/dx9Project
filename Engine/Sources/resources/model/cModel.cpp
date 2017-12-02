#include "cModel.h"


cModel::cModel(cResources * pRes, ResMemory flag)
{
	if (pRes == NULL) return;
	type = flag;
	pResources = pRes;

	effect = new cEffect("Effects/defaultFX.fx", pRes);
}

HRESULT cModel::setMesh(cMesh * pMesh)
{
	if (pMesh == NULL) return E_FAIL;
	mesh = pMesh;
	return S_OK;
}

HRESULT cModel::setEffect(cEffect * pEffect)
{
	if (pEffect == NULL) return E_FAIL;
	effect = pEffect;
	return S_OK;
}


cModel::~cModel()
{
	if (effect != NULL)
	{
		delete effect;
		effect = NULL;
	}
}
