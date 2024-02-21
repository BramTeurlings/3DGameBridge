#include "compositor.h"

#include <array>
#include <fstream>
#include <filesystem>

#include "swapchain.h"

namespace XRGameBridge {
    const std::string LAYERING_VERTEX = "../../runtime_openxr/shaders/layering_vertex.cso";
    const std::string LAYERING_PIXEL = "../../runtime_openxr/shaders/layering_pixel.cso";

    std::vector<char> LoadBinaryFile(std::string path) {
        std::filesystem::path file_path(path);
        std::string abs_path = std::filesystem::absolute(file_path).string();

        std::ifstream file(abs_path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return std::vector<char>(0);
        }

        // Get size and reset cursor
        uint32_t size = file.tellg();
        file.seekg(0);

        // Load into buffer
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            return std::vector<char>(0);
        }

        return buffer;
    }

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
            ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

            CD3DX12_ROOT_PARAMETER1 root_parameters[4];
            root_parameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
            root_parameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
            root_parameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_VERTEX);
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
            auto vertex_shader = LoadBinaryFile(LAYERING_VERTEX);
            auto pixel_shader = LoadBinaryFile(LAYERING_PIXEL);

            //ThrowIfFailed(ReadDataFromFile(GetAssetFullPath(L"shader_mesh_simple_vert.cso").c_str(), &pVertexShaderData, &vertexShaderDataLength));


            CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
            rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

            // Define the vertex input layout.
            std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs = {
                //{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };

            // Describe and create the graphics pipeline state object (PSO).
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
            psoDesc.InputLayout = { inputElementDescs.data(),static_cast<uint32_t>(inputElementDescs.size()) };
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertex_shader.data(), vertex_shader.size());
            psoDesc.pRootSignature = root_signature.Get();
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixel_shader.data(), pixel_shader.size());
            psoDesc.RasterizerState = rasterizerStateDesc;
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; //TODO choose format from the client
            psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            psoDesc.SampleDesc.Count = 1;

            ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_state)));
            pipeline_state->SetName(L"Compositor Pipeline State");
        }
    }

    void GB_Compositor::ComposeImage(const XrFrameEndInfo* frameEndInfo, ID3D12GraphicsCommandList* cmd_list) {
        // TODO uses the command queue and the frame struct from endframe to compose the whole frame
        // TODO after that it executes the command list to render to the actual swapchain and set the fences on every proxy swapchain image

        cmd_list->SetPipelineState(pipeline_state.Get());
        cmd_list->DrawInstanced(4, 1, 0, 0);
    }
}
