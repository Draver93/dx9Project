#ifndef C_MODEL_H
#define C_MODEL_H

#include <Windows.h>
#include <vector>
#include <string>
#include <fstream>

#include <iostream>
#include <sstream>

#include "../mesh/cMesh.h"
#include "../effect/cEffect.h"
#include "../animation/cAnimation.h"

#include "../cResources.h"

struct sTransform
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 angle;
	float size;
	sTransform()
	{
		pos.x = pos.y = pos.z = 0;
		angle.x = angle.y = angle.z = 0;
		size = 1.0f;
	}
	D3DXMATRIX getMatrix()
	{
		D3DXMATRIX	matrix,
			matrixT,
			matrixRX,
			matrixRY,
			matrixRZ,
			matrixS;

		float	angle_rad_x = angle.x*(D3DX_PI / 180),
			angle_rad_y = angle.y*(D3DX_PI / 180),
			angle_rad_z = angle.z*(D3DX_PI / 180);

		double num = 0.5;
		double crat = 10;
		if (size > 0)
			for (int i = 0; i < size; i++)
			{
				num += 1.0f / (float)crat;
			}
		else
			for (int i = 0; i > size; i--)
			{
				double tmp = (num - (1.0 / crat));
				if (tmp < (1.0 / crat))
					crat *= 10;
				num -= 1.0 / crat;
			}

		D3DXMatrixIdentity(&matrix);
		D3DXMatrixScaling(&matrixS, (float)size, (float)size, (float)size);
		D3DXMatrixMultiply(&matrix, &matrix, &matrixS);
		D3DXMatrixRotationX(&matrixRX, angle_rad_x);
		D3DXMatrixRotationY(&matrixRY, angle_rad_y);
		D3DXMatrixRotationZ(&matrixRZ, angle_rad_z);
		D3DXMatrixMultiply(&matrix, &matrix, &matrixRX);
		D3DXMatrixMultiply(&matrix, &matrix, &matrixRY);
		D3DXMatrixMultiply(&matrix, &matrix, &matrixRZ);
		D3DXMatrixTranslation(&matrixT, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&matrix, &matrix, &matrixT);

		return matrix;
	}
};

class cModel
{
public:
	sTransform transform;
private:
	ResMemory type;
	cResources *pResources;

	cMesh *mesh;
	cEffect *effect;
public:
	cModel(cResources *pRes, ResMemory flag);
	HRESULT setMesh(cMesh *pMesh);
	HRESULT setEffect(cEffect *pEffect);

	cMesh* getMesh() { return mesh; }
	cEffect* getEffect() { return effect; }
	~cModel();
};

#endif // !C_MODEL_H
