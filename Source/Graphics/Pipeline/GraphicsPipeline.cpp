#include "GraphicsPipeline.h"
#include "Graphics/Resource/Shader.h"
#include <cassert>

void GraphicsPipeline::Initialize(
    ID3D12Device *device, ID3D12RootSignature *rootSignature,
    const Shader &vertexShader, const Shader &pixelShader,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout,
    const D3D12_RASTERIZER_DESC &rasterizerDesc,
    const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc,
    const D3D12_BLEND_DESC &blendDesc
) {
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};

    // 引数から直接セット
    psoDesc.pRootSignature = rootSignature;
    psoDesc.RasterizerState = rasterizerDesc;
    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.BlendState = blendDesc;

    // インプットレイアウトの紐付け（寿命が切れる前にここでセット）
    psoDesc.InputLayout.pInputElementDescs = inputLayout.data();
    psoDesc.InputLayout.NumElements = static_cast<UINT>(inputLayout.size());

    // シェーダーのセット
    psoDesc.VS = { vertexShader.GetBlob()->GetBufferPointer(), vertexShader.GetBlob()->GetBufferSize() };
    psoDesc.PS = { pixelShader.GetBlob()->GetBufferPointer(), pixelShader.GetBlob()->GetBufferSize() };

    // 固定値・環境依存の設定
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    // 実際に生成
    HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
    (void)hr;
}