#pragma once

#pragma warning (disable : 4251)
#pragma warning (disable : 26812)

#include <d3d11.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXTK\SimpleMath.h>
#include <d3dcompiler.h>
#include <Effect11\d3dx11effect.h>
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include <DirectXTK\ScreenGrab.h>
#include <DirectXCollision.h>
#include <DirectXCollision.inl>

#include <DirectXTK\PrimitiveBatch.h>
#include <DirectXTK\VertexTypes.h>
#include <DirectXTK\Effects.h>

#include <DirectXTK\SpriteBatch.h>
#include <DirectXTK\SpriteFont.h>

using namespace DirectX;

/* assimp */
#include <assimp\scene.h>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>

/* fmod channel */
#include "fmod.h"
#include "fmod.hpp"
#include <io.h>


enum class OBJECT_TYPE {
	TERRAIN, PLAYER, PORP, MONSTER, BOSS, PART, SKY, _END
};

enum class CHANNELID {
	CHANNEL_BGM,
	CHANNEL_0, CHANNEL_1, CHANNEL_2, CHANNEL_3, CHANNEL_4,
	CHANNEL_5, CHANNEL_6, CHANNEL_7, CHANNEL_8, CHANNEL_9,
	CHANNEL_10, CHANNEL_11, CHANNEL_12, CHANNEL_13, CHANNEL_14,
	CHANNEL_15, CHANNEL_16, CHANNEL_17, CHANNEL_18, CHANNEL_19,
	CHANNEL_20, CHANNEL_21, CHANNEL_22, CHANNEL_23, CHANNEL_24,
	CHANNEL_25, CHANNEL_26, CHANNEL_27, CHANNEL_28, CHANNEL_29,
	CHANNEL_30,
	CHANNEL_END
};

enum class STATE {
	IDLE, WALK, RUN, AVOID, ATTACK, RAVENATTACK, HIT, WAKE, PARRY, SKILL, 
	LOCK_IDLE, LOCK_WALK, LOCK_AVOID, LOCK_ATTACK, LOCK_PARRY, _END
};

enum LAYER_TAG {
	LAYER_PLAYER, LAYER_MONSTER, LAYER_BOSS, LAYER_PROP, LAYER_MAP,
	LAYER_BACKGROUND, LAYER_CAMERA, LAYER_SKY, LAYER_EDITOBJECT, LAYER_EDITERRAIN,
	LAYER_END
};

#include <cstdio>
#include <typeinfo>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector> 
#include <list> 
#include <map> 
#include <unordered_map> 
#include <algorithm>
#include <set>
#include <filesystem>
#include <random>
using namespace std;


#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
using namespace Engine;

#define	VK_MAX				0xff

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG


