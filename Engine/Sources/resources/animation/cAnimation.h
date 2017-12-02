#ifndef C_ANIMATION_H
#define C_ANIMATION_H

#include <Windows.h>
#include <vector>
#include <string>

#include "../cResources.h"

class cAnimation : cVirtualResource
{
private:
	const ResMemory type = RT_STATIC;
	cResources *pResources;
public:
	cAnimation(cResources *pRes);
	~cAnimation();

	HRESULT release();
	HRESULT recreate();
	ResMemory getMemoryType() { return type; };
};

#endif // !C_ANIMATION_H