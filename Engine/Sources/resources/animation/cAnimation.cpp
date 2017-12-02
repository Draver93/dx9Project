#include "cAnimation.h"

cAnimation::cAnimation(cResources *pRes)
{
	if (pRes == NULL) return;

	pResources = pRes;
	pResources->addResource(this);
}

cAnimation::~cAnimation()
{
}

HRESULT cAnimation::release()
{
	return E_NOTIMPL;
}

HRESULT cAnimation::recreate()
{
	return E_NOTIMPL;
}

