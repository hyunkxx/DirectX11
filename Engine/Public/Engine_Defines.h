#pragma once

#define IN
#define OUT

#define MAX_TAG 40

#define DIRECTINPUT_VERSION 0x0800

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

// ImGui
#include "../../Client/ImGui/imgui.h"
#include "../../Client/ImGui/imgui_impl_win32.h"
#include "../../Client/ImGui/imgui_impl_dx11.h"

#include "cmath"

// DirectX
#include <d3d11.h>
#include <dinput.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <process.h>
#include <d3dcompiler.h>
#include <typeinfo.h>
#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "DirectXTK/ScreenGrab.h"

// Debug render
#include "DirectXTK/PrimitiveBatch.h"
#include "DirectXTK/VertexTypes.h"
#include "DirectXTK/Effects.h"

// Font Render
#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"


// C++ & STL
#include "commdlg.h"
#include <cstdlib>
#include <stdlib.h>
#include <thread>
#include <assert.h>
#include <cassert>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <concurrent_priority_queue.h>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cstring>

#include <time.h>

#ifdef _DEBUG
#include <filesystem>
using std::experimental::filesystem::recursive_directory_iterator;
#endif // _DEBUG


using namespace std;
using namespace DirectX;

// Media
#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

// FMOD
#include "Fmod/fmod.h"
#include "Fmod/fmod.hpp"
#include "Fmod/fmod_codec.h"
#include "Fmod/fmod_common.h"
#include "Fmod/fmod_dsp.h"
#include "Fmod/fmod_dsp_effects.h"
#include "Fmod/fmod_errors.h"
#include "Fmod/fmod_output.h"
#include <io.h>

#include "CustomVector.h"
#include "Engine_Macro.h"
#include "Engine_Enum.h"
#include "Engine_Typedef.h"
#include "Engine_Struct.h"
#include "Engine_Template.h"


#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW
#endif

#endif

using namespace Engine;