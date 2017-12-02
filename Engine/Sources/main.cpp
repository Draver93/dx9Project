#include "main.h"

cGraphics *gWnd = NULL;
cResources *pRes = NULL;
cScene *scene = NULL;

cKeyboard *keyboard = NULL;
cMouse *mouse = NULL;

cRenderer *render = NULL;
float angle = 90;
bool init = false;
bool stop = false;
float ss_x = 0, ss_y = 0, ss_z = 0;
int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wClass = { 0 };
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.style = CS_HREDRAW | CS_VREDRAW;
	wClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hIcon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	wClass.hIconSm = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
	wClass.lpszClassName = "cNone";
	wClass.lpszMenuName = "Window_1";
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;


	D3DPRESENT_PARAMETERS gParameters = { 0 };
	gParameters.Windowed = TRUE;
	gParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	gParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	gParameters.AutoDepthStencilFormat = D3DFMT_D16;
	gParameters.EnableAutoDepthStencil = TRUE;
	gParameters.BackBufferCount = 0;
	gParameters.FullScreen_RefreshRateInHz = 0;
	gParameters.BackBufferHeight = 0;
	gParameters.BackBufferWidth = 0;

	cWindow wnd(wClass, WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	while (true)
	{
		Sleep(100);
		if (!init && wnd.getHWND() != NULL)
		{
			gWnd = new cGraphics(&wnd, &gParameters);
			gWnd->getDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		//Отсечение не попадающих в кадр полигонов 

			render = new cRenderer(gWnd, f, RT_DEFERRED);
			sLight lt;
			lt.fov = 90;
			lt.pos.y = 0.0f;
			lt.look.x = 0.258021176f;
			lt.look.y = 0.965925813f;
			lt.look.z = 0.020306704f;
			render->lights.push_back(lt);
			keyboard = new cKeyboard(&wnd);
			mouse = new cMouse(&wnd);
			render->shadowMapFx->getAttr("x")->setVar(ss_x);
			render->shadowMapFx->getAttr("y")->setVar(ss_y);
			render->shadowMapFx->getAttr("z")->setVar(ss_z);

			scene = new cScene(gWnd->getResources());
			scene->load("Models/conference.obj", true);

			std::vector<cModel*> *tmpVec = scene->getModels();
			sCamera *cam = gWnd->getCamera();
			for (UINT i = 0; i < tmpVec->size(); i++)
			{
				tmpVec->at(i)->transform.size = 0.01f;
				tmpVec->at(i)->getEffect()->getAttr("MatrixWorld")->setPtr(&cam->wMatrix);
				tmpVec->at(i)->getEffect()->getAttr("MatrixProjection")->setPtr(&cam->pMatrix);
				tmpVec->at(i)->getEffect()->getAttr("MatrixView")->setPtr(&cam->vMatrix);

			}

			init = true;
		}
		render->shadowMapFx->getAttr("x")->setVar(ss_x);
		render->shadowMapFx->getAttr("y")->setVar(ss_y);
		render->shadowMapFx->getAttr("z")->setVar(ss_z);
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_ESCAPE)) break;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_Q)) stop = true;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_E)) stop = false;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_UPARROW)) render->lights.at(0).zNear += 0.01f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_DOWNARROW)) render->lights.at(0).zNear -= 0.01f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_LEFTARROW)) render->lights.at(0).fov -= 1.0f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_RIGHTARROW)) render->lights.at(0).fov += 1.0f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_O))ss_x = -0.1f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_P))ss_x = 0.1f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_9))ss_y -= 1.0f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_0))ss_y += 1.0f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_K))ss_z -= 1.0f;
		if (keyboard != NULL && KEYDOWN(keyboard->state, DIK_L))ss_z += 1.0f;

	}
	return 0;
}
int f()
{	
	if (keyboard != NULL)keyboard->update();
	if (mouse != NULL)mouse->update();

	if (gWnd != NULL)
	{
		
	}
	if (init == TRUE)
	{
		sCamera *cam = gWnd->getCamera();
		if (!stop)
		{
			render->lights.at(0).pos = cam->pos;
			//render->lights.at(0).look = cam->look;
		}
		cam->update(gWnd->getGParameters().BackBufferWidth, gWnd->getGParameters().BackBufferHeight, VIEW_3D, 45);
		render->draw(scene);

		cam->manualControl(keyboard->state, &mouse->state);
	}
	return 0;
}