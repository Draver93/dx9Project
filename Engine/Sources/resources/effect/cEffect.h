#ifndef C_EFFECT_H
#define C_EFFECT_H

#include <Windows.h>
#include <vector>
#include <string>
#include "../cResources.h"

enum AttrType { TYPE_VALUE, TYPE_POINTER, TYPE_NONE};

struct sTechnique
{
	D3DXHANDLE hl;
	D3DXTECHNIQUE_DESC desc;
	sTechnique()
	{
		hl = NULL;
	}
};
struct sGlobal
{
	D3DXHANDLE hl;
	D3DXPARAMETER_DESC desc;
	UINT offset;
	sGlobal()
	{
		hl = NULL;
		offset = 0;
	}
};
struct sAttr
{
	std::string name;
	AttrType type;
	std::vector<float> data_var;
	void* data_ptr;

	sAttr()
	{
		type = TYPE_NONE;
		data_var.clear();
		data_ptr = NULL;
	}

	void setMatrix(D3DXMATRIX *mt)
	{
		data_var.clear();
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				data_var.push_back(mt->m[i][j]);
		type = TYPE_VALUE;
	}
	void setVar(float var)
	{
		data_var.clear();
		data_var.push_back(var);
		type = TYPE_VALUE;
	}
	void setMas(std::vector<float> *vec)
	{
		data_var.clear();
		data_var = *vec;
		type = TYPE_VALUE;
	}
	void setPtr(void* ptr)
	{
		data_ptr = ptr;
		type = TYPE_POINTER;
	}
};

class cEffectData : cVirtualInterface
{
private:
	cResources *pResources;

	std::string path;
	LPD3DXEFFECT data;
	std::vector<sGlobal> globals;
	std::vector<sTechnique> techniques;

	HRESULT load(std::string filePath);
public:
	cEffectData(std::string filePath, cResources *pRes);
	~cEffectData();

	virtual HRESULT onLostDevice();
	virtual HRESULT onResetDevice();

	sGlobal *getGlobal(std::string name);
	std::vector<sGlobal> *getGlobals() { return &globals; }
	std::vector<sTechnique> *getTechniques() { return &techniques; }
	sTechnique *getTech(std::string name);
	LPD3DXEFFECT getData() { return data; }
};


class cEffect
{
private:
	std::string name;
	cEffectData *direct3DEffect;
	std::vector<sAttr> vecAttributs;
public:
	cEffect(std::string filePath, cResources *pRes);
	~cEffect();

	sAttr* getAttr(std::string name);
	HRESULT setAllUniform();

	sGlobal *getGlobal(std::string name) { return direct3DEffect->getGlobal(name); }
	sTechnique *getTech(std::string name) { return direct3DEffect->getTech(name); }
	std::vector<sGlobal> *getGlobals() { return direct3DEffect->getGlobals(); }
	std::vector<sTechnique> *getTechniques() { return direct3DEffect->getTechniques(); }
	LPD3DXEFFECT getData() { return direct3DEffect->getData(); }
};

#endif // !C_EFFECT_H
