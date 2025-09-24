#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <D3D11.h>

#include "DirectXMath.h"
#include "DirectXColors.h"
using namespace DirectX;

static constexpr float PI = 3.141592f;

#include <memory>

#if _DEBUG
#define RESOURCE_DIR L"../res/resources/"
#else
#define RESOURCE_DIR L"../res/resources/"
#endif

#include "assert.h"
#include "types.h"
#include "debug.h"
#include "intpoint.h"

