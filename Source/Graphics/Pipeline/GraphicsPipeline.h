#pragma once

#pragma comment(lib, "d3d12.lib")

#include <wrl.h>
#include <d3d12.h>
#include <vector>

namespace Kizuna {
    class RootSignature;
    class Shader;

    /// <summary>
    /// GraphicsPipelineState(PSO)を管理するクラス
    /// </summary>
    /// <remarks>
    /// RootSignatureやShader、各種描画ステートを組み合わせ、
    /// DirectX12で利用するGraphicsPipelineStateを生成・管理する。
    /// </remarks>
    class GraphicsPipeline {
    public:
        /// <summary>
        /// GraphicsPipelineStateを生成する
        /// </summary>
        /// <param name="device">DirectXデバイス</param>
        /// <param name="rootSignature">RootSignature</param>
        /// <param name="vertexShader">頂点シェーダー</param>
        /// <param name="pixelShader">ピクセルシェーダー</param>
        /// <param name="inputLayout">入力レイアウト</param>
        /// <param name="rasterizerDesc">ラスタライザ設定</param>
        /// <param name="depthStencilDesc">DepthStencil設定</param>
        /// <param name="blendDesc">Blend設定</param>
        void Initialize(
            ID3D12Device *device,
            ID3D12RootSignature *rootSignature,
            const Shader &vertexShader,
            const Shader &pixelShader,
            const std::vector<D3D12_INPUT_ELEMENT_DESC> &inputLayout,
            const D3D12_RASTERIZER_DESC &rasterizerDesc,
            const D3D12_DEPTH_STENCIL_DESC &depthStencilDesc,
            const D3D12_BLEND_DESC &blendDesc
        );

        /// <summary>
        /// GraphicsPipelineStateを取得する
        /// </summary>
        /// <returns>GraphicsPipelineState</returns>
        ID3D12PipelineState *GetPipelineState() const { return pipelineState_.Get(); }

    private:
        /// GraphicsPipelineState
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    };
}