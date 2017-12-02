#include "cScene.h"


cScene::cScene(cResources *pRes)
{
	pResources = pRes;
}
cScene::~cScene()
{
}

HRESULT cScene::load(std::string filePath, bool indexed)
{
	Assimp::Importer importer;
	const aiScene* scene;
	if(indexed)
		scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices); //индексированный
	else 
		scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals); //не индексированный 

	models = getMeshes(scene, indexed);
	animations = getAnimations(scene);

	return S_OK;
}
std::vector<cModel*> cScene::getMeshes(const aiScene * scene, bool indexed)
{
	if (!scene->HasMeshes()) return std::vector<cModel*>();
	std::vector<cModel*> resultVec;
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *pMesh = scene->mMeshes[i];
		std::vector<VERTEX> tmpVecVertices;
		std::vector<WORD> tmpVecIndices;

		for (UINT j = 0; j < pMesh->mNumVertices; j++)
		{
			VERTEX tmpVertex;
			if (pMesh->HasPositions())
			{
				tmpVertex.x = pMesh->mVertices[j].x;
				tmpVertex.y = pMesh->mVertices[j].y;
				tmpVertex.z = pMesh->mVertices[j].z;
			}
			if (pMesh->HasNormals())
			{
				tmpVertex.nx = pMesh->mNormals[j].x;
				tmpVertex.ny = pMesh->mNormals[j].y;
				tmpVertex.nz = pMesh->mNormals[j].z;
			}
			if (pMesh->HasTextureCoords(0)) // 0 значит загрузка только 1 UV координат
			{
				tmpVertex.tu = pMesh->mTextureCoords[0][j].x;
				tmpVertex.tv = pMesh->mTextureCoords[0][j].y;
			}
			tmpVecVertices.push_back(tmpVertex);
		}

		if (pMesh->HasFaces() && indexed)
		{
			for (UINT j = 0; j < pMesh->mNumFaces; j++)
			{
				aiFace *pFace = &pMesh->mFaces[j];
				for (UINT k = 0; k < pFace->mNumIndices; k++)
				{
					tmpVecIndices.push_back(pFace->mIndices[k]);
				}
			}
		}
		cMesh *mesh = new cMesh(pResources, RT_STATIC);

		if(indexed) mesh->setBuffer(&tmpVecVertices, &tmpVecIndices);
		else mesh->setBuffer(&tmpVecVertices);

		mesh->setBuffer(&tmpVecVertices);
		mesh->setName(scene->mMeshes[i]->mName.C_Str());

		cModel *model = new cModel(pResources, RT_STATIC);
		model->setMesh(mesh);

		resultVec.push_back(model);
	}
	return resultVec;
}
std::vector<cAnimation*> cScene::getAnimations(const aiScene* scene)
{
	return std::vector<cAnimation*>();
}
