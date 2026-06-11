#pragma once

#include <wrl.h>
#include <d3d12.h>

class RootSignature;
class InputLayout;
class BlendState;
class RasterizerState;
class Shader;
class DepthStencilState;

class GraphicsPipeline {
public:
    void Initialize(ID3D12Device *device,
        RootSignature &rootSignature, InputLayout &inputLayout,
        BlendState &blendState, RasterizerState &rasterizerState,
        DepthStencilState &depthStencilState,
        Shader &vertexShader, Shader &pixelShader
    );

    ID3D12PipelineState *GetPipelineState() const {return pipelineState_.Get();}

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};