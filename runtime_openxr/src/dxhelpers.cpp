#include "dxhelpers.h"

void CreateProxyResource(ID3D12Device* device, uint64_t width, uint64_t height, int64_t format, ID3D12Resource* resource) {
    DXGI_FORMAT dx_format = static_cast<DXGI_FORMAT>(format);

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = dx_format;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    //D3D12_DEPTH_STENCIL_VALUE depth_stencil_value;
    //depth_stencil_value.Depth = 100.f;
    //depth_stencil_value.Stencil = 0;

    CreateResource(device, textureDesc, resource);
}

void CreateResource(ID3D12Device* device, D3D12_RESOURCE_DESC desc, ID3D12Resource* resource) {
    D3D12_CLEAR_VALUE clear_value{};
    clear_value.Format = desc.Format;
    clear_value.Color[0] = clear_color[0];
    clear_value.Color[1] = clear_color[1];
    clear_value.Color[2] = clear_color[2];
    clear_value.Color[3] = clear_color[3];

    auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(&heap_properties, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clear_value, IID_PPV_ARGS(&resource))
    );
}