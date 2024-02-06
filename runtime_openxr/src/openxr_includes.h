#pragma once

// OpenXR Windows and DirectX

//#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
//#endif

#include <windows.h>
#include <directx/d3d12.h>
#include <dxgi1_6.h>
#include <directx/d3dx12.h>

#include <d3d11.h>

#include <wrl/client.h>
#include <winerror.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "dxgi.lib")

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// OpenXR headers
#include <openxr/openxr.h>
#include <openxr/openxr_loader_negotiation.h>
#include <openxr/openxr_platform.h>
