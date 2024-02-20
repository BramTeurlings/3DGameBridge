#include "compositor.h"

namespace XRGameBridge
{
    void GB_Compositor::Initialize(const ComPtr<ID3D12Device>& device) {
        // Create the root signature.
        {
            D3D12_FEATURE_DATA_ROOT_SIGNATURE feature_data = {};

            // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
            feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

            if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature_data, sizeof(feature_data)))) {
                feature_data.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
            }

            CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
            ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
            ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
            //ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

            CD3DX12_ROOT_PARAMETER1 root_parameters[4];
            root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
            root_parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
            //root_parameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_VERTEX);
            root_parameters[3].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_desc;
            root_signature_desc.Init_1_1(_countof(root_parameters), root_parameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

            ComPtr<ID3DBlob> signature;
            ComPtr<ID3DBlob> error;
            ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_desc, feature_data.HighestVersion, &signature, &error));
            ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));
            root_signature->SetName(L"Compositor Root Signature");
        }

        // Create the pipeline state, which includes loading shaders.
        {
            //UINT8* pVertexShaderData;
            //UINT vertexShaderDataLength;
            UINT8* pPixelShaderData;
            UINT pixelShaderDataLength;

            //ThrowIfFailed(ReadDataFromFile(GetAssetFullPath(L"shader_mesh_simple_vert.cso").c_str(), &pVertexShaderData, &vertexShaderDataLength));
            ThrowIfFailed(ReadDataFromFile(GetAssetFullPath(L"shader_mesh_dynamic_indexing_pixel.cso").c_str(), &pPixelShaderData, &pixelShaderDataLength));

            CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
            rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

            // Describe and create the graphics pipeline state object (PSO).
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            //psoDesc.InputLayout = { SampleAssets::StandardVertexDescription, SampleAssets::StandardVertexDescriptionNumElements };
            //psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShaderData, vertexShaderDataLength);
            psoDesc.pRootSignature = root_signature.Get();
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShaderData, pixelShaderDataLength);
            psoDesc.RasterizerState = rasterizerStateDesc;
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            psoDesc.SampleDesc.Count = 1;

            ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_state)));
            pipeline_state->SetName(L"Compositor Pipeline State");

            //delete pVertexShaderData;
            delete pPixelShaderData;
        }
    }

    void GB_Compositor::ComposeImage(const XrFrameEndInfo* frameEndInfo) {
        // TODO uses the command queue and the frame struct from endframe to compose the whole frame
        // TODO after that it executes the command list to render to the actual swapchain and set the fences on every proxy swapchain image
    }

    void GB_Compositor::PresentToResource(ID3D12Resource* render_target) {
        // TODO present the image on screen
        // Or to the UEVR window, or both...
    }
}
