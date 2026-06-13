#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>
#include <vector>

class RootSignature;
class Shader;

class GraphicsPipeline {
public:
    void Initialize(
        ID3D12Device *device, ID3D12RootSignature *rootSignature,
        const Shader &vertexShader, const Shader &pixelShader,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout,
        const D3D12_RASTERIZER_DESC &rasterizerDesc,
        const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc,
        const D3D12_BLEND_DESC &blendDesc
    );

    ID3D12PipelineState *GetPipelineState() const {return pipelineState_.Get();}

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};