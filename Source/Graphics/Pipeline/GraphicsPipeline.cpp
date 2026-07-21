#include "GraphicsPipeline.h"
#include "Graphics/Resource/Shader.h"
#include <cassert>

namespace Kizuna {
    void GraphicsPipeline::Initialize(
        ID3D12Device *device,
        ID3D12RootSignature *rootSignature,
        const Shader &vertexShader,
        const Shader &pixelShader,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout,
        const D3D12_RASTERIZER_DESC &rasterizerDesc,
        const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc,
        const D3D12_BLEND_DESC &blendDesc
    ) {
        // GraphicsPipelineState設定を初期化する
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

        // RootSignatureと各種描画ステートを設定する
        psoDesc.pRootSignature = rootSignature;
        psoDesc.RasterizerState = rasterizerDesc;
        psoDesc.DepthStencilState = depthStencilDesc;
        psoDesc.BlendState = blendDesc;

        // 入力レイアウトを設定する
        psoDesc.InputLayout.pInputElementDescs = inputLayout.data();
        psoDesc.InputLayout.NumElements = static_cast<UINT>(inputLayout.size());

        // 頂点シェーダーを設定する
        psoDesc.VS = {
            vertexShader.GetBlob()->GetBufferPointer(),
            vertexShader.GetBlob()->GetBufferSize()
        };

        // ピクセルシェーダーを設定する
        psoDesc.PS = {
            pixelShader.GetBlob()->GetBufferPointer(),
            pixelShader.GetBlob()->GetBufferSize()
        };

        // RenderTargetやDepthStencilなどの固定設定を行う
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        // GraphicsPipelineStateを生成する
        HRESULT hr = device->CreateGraphicsPipelineState(
            &psoDesc,
            IID_PPV_ARGS(&pipelineState_)
        );
        assert(SUCCEEDED(hr));

        (void)hr;
    }
}