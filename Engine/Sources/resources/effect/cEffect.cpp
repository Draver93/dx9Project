#include "cEffect.h"


cEffectData::cEffectData(std::string filePath, cResources * pRes)
{
	path = filePath;
	if (pRes == NULL) return;
	pResources = pRes;
	pResources->addInterface(this);
	HRESULT hr = load(filePath);
}
cEffectData::~cEffectData()
{
	if (data != NULL)
	{
		data->Release();
		data = NULL;
		pResources->removeInterface(this);
	}
	else return;
}

HRESULT cEffectData::load(std::string filePath)
{
	if (filePath.empty() ||
		pResources == NULL ||
		data != NULL) return E_FAIL;
	
	path = filePath;
	DWORD dwShaderFlags = D3DXSHADER_NO_PRESHADER;
	ID3DXBuffer* errorBuffer = 0;
	LPDIRECT3DDEVICE9 pDevice = pResources->getDevice();
	if (pDevice == NULL) return E_FAIL;

	HRESULT hr = D3DXCreateEffectFromFile(pDevice, path.c_str(), NULL, NULL, dwShaderFlags, NULL, &data, &errorBuffer);
	if (hr != S_OK) return E_FAIL;
	if (errorBuffer)
	{
		MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		return E_FAIL;
	}

	UINT i = 0;
	D3DXHANDLE hl = NULL;
	sGlobal gl;
	hl = data->GetParameter(NULL, i);
	while (hl != NULL)
	{
		gl.hl = hl;
		data->GetParameterDesc(hl, &gl.desc);

		UINT tmpOffset = 0;
		if (!globals.empty())
			gl.offset = globals.back().offset + globals.back().desc.Bytes;
		globals.push_back(gl);

		i++;
		hl = data->GetParameter(NULL, i);
	}

	i = 0;
	hl = 0;
	sTechnique tq;
	hl = data->GetTechnique(i);
	while (hl != NULL)
	{
		tq.hl = hl;
		data->GetTechniqueDesc(hl, &tq.desc);

		techniques.push_back(tq);

		i++;
		hl = data->GetTechnique(i);
	}

	return S_OK;
}
HRESULT cEffectData::onLostDevice()
{
	if (data == NULL)return E_FAIL;
	data->OnLostDevice();
	return S_OK;
}
HRESULT cEffectData::onResetDevice()
{
	if (data == NULL)return E_FAIL;
	data->OnResetDevice();
	return S_OK;
}
sGlobal * cEffectData::getGlobal(std::string name)
{
	for (UINT i = 0; i < globals.size(); i++)
	{
		if (globals.at(i).desc.Name == name) return &globals.at(i);
	}
	return NULL;
}
sTechnique * cEffectData::getTech(std::string name)
{
	for (UINT i = 0; i < techniques.size(); i++)
	{
		if (techniques.at(i).desc.Name == name) return &techniques.at(i);
	}
	return NULL;
}



cEffect::cEffect(std::string filePath, cResources * pRes)
{
	name = filePath;
	direct3DEffect = new cEffectData(filePath, pRes);

	std::vector<sGlobal> *vecGlobals = direct3DEffect->getGlobals();
	vecAttributs.clear();
	sAttr attr;
	for (UINT i = 0; i < vecGlobals->size(); i++)
	{
		attr = {};
		attr.name = vecGlobals->at(i).desc.Name;
		vecAttributs.push_back(attr);
	}
}
cEffect::~cEffect()
{
}

sAttr* cEffect::getAttr(std::string name)
{
	for (UINT i = 0; i < vecAttributs.size(); i++)
		if (vecAttributs.at(i).name == name) return &vecAttributs.at(i);
	return NULL;
}

HRESULT cEffect::setAllUniform()
{
	std::vector<sGlobal> *globals = direct3DEffect->getGlobals();
	LPD3DXEFFECT effect = direct3DEffect->getData();
	for (UINT i = 0; i < globals->size(); i++)
	{
		sGlobal *pGl = &globals->at(i);
		sAttr *pAttr = getAttr(pGl->desc.Name);
		if (pAttr != NULL)
		{
			switch (pAttr->type)
			{
				case TYPE_VALUE:
				{
					effect->SetValue(pGl->hl, pAttr->data_var.data(), pGl->desc.Bytes);
					break;
				}
				case TYPE_POINTER:
				{
					effect->SetValue(pGl->hl, pAttr->data_ptr, pGl->desc.Bytes);
					break;
				}
			}
		}
	}
	return S_OK;
}

