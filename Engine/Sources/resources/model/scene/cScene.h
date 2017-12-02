#ifndef C_SCENE_H
#define C_SCENE_H

#include <Windows.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "../cModel.h"

class cScene
{
private:
	cResources *pResources;

	std::vector<cModel*> models;
	std::vector<cAnimation*> animations;

	std::vector<cModel*> getMeshes(const aiScene* scene, bool indexed);
	std::vector<cAnimation*> getAnimations(const aiScene* scene);
public:
	cScene(cResources *pRes);
	~cScene();
	HRESULT load(std::string filePath, bool indexed);

	std::vector<cModel*>* getModels() { return &models; }
	std::vector<cAnimation*>* getAnim() { return &animations; }

};

#endif // !C_SCENE_H
