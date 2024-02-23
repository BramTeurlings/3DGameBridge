#pragma once
#include "openxr_includes.h"

namespace XRGameBridge {
    class GB_Compositor {
        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;

        ComPtr<ID3D12Device> d3d12_device;
        ComPtr<ID3D12CommandQueue> command_queue;

        std::vector<ComPtr<ID3D12CommandAllocator>> command_allocators;
        std::vector<ComPtr<ID3D12GraphicsCommandList>> command_lists;

    public:
        void Initialize(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12CommandQueue>& queue, uint32_t back_buffer_count);
        //void InitShaders(const ComPtr<ID3D12Device>& device);
        void ComposeImage(const XrFrameEndInfo* frameEndInfo, ID3D12GraphicsCommandList* cmd_list);
        void ExecuteCommandLists(ID3D12GraphicsCommandList* cmd_list, const XrFrameEndInfo* frameEndInfo);

        void TransitionBackBufferImage(ID3D12GraphicsCommandList* cmd_list, ID3D12Resource* resource, D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after);

        ComPtr<ID3D12GraphicsCommandList>& GetCommandList(uint32_t index);
        ComPtr<ID3D12CommandAllocator>& GetCommandAllocator(uint32_t index);
    };
}
