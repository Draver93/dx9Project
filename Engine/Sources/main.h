/*
Данный заголовочный файл определяет ряд стандартых 
функций для работы программы.
Разработчик Финошкин Алексей.
*/

#ifndef MAIN_H
#define MAIN_H

#include <Windows.h>
#include <vector>
#include "resources/mesh/cMesh.h"
#include "resources/model/cModel.h"
#include "resources/model/scene/cScene.h"
#include "window/cWindow.h"
#include "resources/animation/cAnimation.h"
#include "window/graphics/renderer/cRenderer.h"
#include "window/keyboard/cKeyboard.h"
#include "window/mouse/cMouse.h"

#include "resource.h"

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

int f();

#endif
