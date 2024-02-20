#pragma once
#include "openxr_includes.h"

namespace XRGameBridge {
    class GB_Compositor {
        ComPtr<ID3D12RootSignature> root_signature;
        ComPtr<ID3D12PipelineState> pipeline_state;

        void Initialize(const ComPtr<ID3D12Device>& device);
        void InitShaders(const ComPtr<ID3D12Device>& device);
        void ComposeImage(const XrFrameEndInfo* frameEndInfo);
        void PresentToResource(ID3D12Resource* render_target); // Probably need rtv
    };
}
