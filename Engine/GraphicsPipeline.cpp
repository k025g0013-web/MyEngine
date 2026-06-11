#include "GraphicsPipeline.h"

#include "RootSignature.h"      // RootSignature
#include "InputLayout.h"		// InputLayout
#include "BlendState.h"	// BlendState
#include "RasterizerState.h"	// RasterizerState
#include "Shader.h"	            // Shader
#include "DepthStencilState.h"  // DepthStencilState

#include <cassert>

void GraphicsPipeline::Initialize(ID3D12Device *device,
    RootSignature &rootSignature, InputLayout &inputLayout,
    BlendState &blendState, RasterizerState &rasterizerState,
    DepthStencilState& depthStencilState,
    Shader &vertexShader, Shader &pixelShader
) {
    // PSO生成
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature.GetRootSignature();
    graphicsPipelineStateDesc.InputLayout = inputLayout.GetDesc();

    graphicsPipelineStateDesc.VS = { vertexShader.GetBlob()->GetBufferPointer(),
    vertexShader.GetBlob()->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShader.GetBlob()->GetBufferPointer(),
    pixelShader.GetBlob()->GetBufferSize() };
    
    graphicsPipelineStateDesc.BlendState = blendState.GetDesc();
    
    graphicsPipelineStateDesc.RasterizerState = rasterizerState.GetDesc();
    
    // 書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    
    // 利用するトポロジ(形状)のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    
    // どのように画面に色を打ち込むかの設定(気にしなくて良い)
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

    // DepthStencilの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilState.GetDesc();
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 実際に生成
    HRESULT hr = device->CreateGraphicsPipelineState(
        &graphicsPipelineStateDesc,
        IID_PPV_ARGS(&pipelineState_)
    );
    assert(SUCCEEDED(hr));
}