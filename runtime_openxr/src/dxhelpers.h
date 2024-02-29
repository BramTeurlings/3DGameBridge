#pragma once
#include "openxr_includes.h"

// Edited from the Microsoft documentation

inline float clear_color[4]{ 0.5f, 0.5f, 0.0f, 1.0f };

void CreateProxyResource(ID3D12Device* device, uint64_t width, uint64_t height, int64_t format, ID3D12Resource* resource);

void CreateResource(ID3D12Device* device, D3D12_RESOURCE_DESC desc, ID3D12Resource* resource);

class CDescriptorHeapWrapper {
public:
    CDescriptorHeapWrapper() { memset(this, 0, sizeof(*this)); }

    HRESULT Create(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT NumDescriptors, bool bShaderVisible = false) {
        D3D12_DESCRIPTOR_HEAP_DESC Desc;
        Desc.Type = Type;
        Desc.NumDescriptors = NumDescriptors;
        Desc.Flags = (bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

        HRESULT hr = pDevice->CreateDescriptorHeap(&Desc, __uuidof(ID3D12DescriptorHeap), static_cast<void**>(& pDH));
        if (FAILED(hr)) {
            return hr;
        }

        hCPUHeapStart = pDH->GetCPUDescriptorHandleForHeapStart();
        hGPUHeapStart = pDH->GetGPUDescriptorHandleForHeapStart();

        HandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize(Desc.Type);
        return hr;
    }
    operator ID3D12DescriptorHeap* () const { return pDH.Get(); }

    D3D12_CPU_DESCRIPTOR_HANDLE hCPU(int32_t index) const {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(hCPUHeapStart, index, HandleIncrementSize);
    }
    D3D12_GPU_DESCRIPTOR_HANDLE hGPU(int32_t index) const {
        ThrowIfFailed(Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        return CD3DX12_GPU_DESCRIPTOR_HANDLE( hGPUHeapStart, index, HandleIncrementSize);
    }

    D3D12_DESCRIPTOR_HEAP_DESC Desc;
    ComPtr<ID3D12DescriptorHeap> pDH;
    D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
    D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
    UINT HandleIncrementSize;
};
