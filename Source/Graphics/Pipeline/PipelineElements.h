#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>

/// <summary>
/// GraphicsPipelineで使用する各種設定要素を生成するクラス
/// </summary>
/// <remarks>
/// Rasterizer、DepthStencil、BlendState、InputLayoutなどの
/// DirectX12パイプライン設定を生成するためのユーティリティクラス。
/// </remarks>
class PipelineElements {
public:
    /// <summary>
    /// 標準的なRasterizerStateを生成する
    /// </summary>
    /// <remarks>
    /// 裏面カリングを有効にした通常の3D描画向け設定。
    /// </remarks>
    /// <returns>Rasterizer設定</returns>
    static D3D12_RASTERIZER_DESC CreateDefaultRasterizer() {
        D3D12_RASTERIZER_DESC desc{};
        // 裏面(時計回り)を表示しない
        desc.CullMode = D3D12_CULL_MODE_BACK;
        // 三角形の中を塗りつぶす
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        return desc;
    }

    /// <summary>
    /// 両面描画用RasterizerStateを生成する
    /// </summary>
    /// <remarks>
    /// カリングを無効化し、表裏両方のポリゴンを描画する。
    /// </remarks>
    /// <returns>Rasterizer設定</returns>
    static D3D12_RASTERIZER_DESC CreateNoCullRasterizer() {
        D3D12_RASTERIZER_DESC desc = CreateDefaultRasterizer();
        // 裏面非表示を無効化し、両面を描画
        desc.CullMode = D3D12_CULL_MODE_NONE;
        return desc;
    }

    /// <summary>
    /// ワイヤーフレーム描画用RasterizerStateを生成する
    /// </summary>
    /// <remarks>
    /// ポリゴンの面を塗りつぶさず、頂点間の線のみを描画する。
    /// </remarks>
    /// <returns>Rasterizer設定</returns>
    static D3D12_RASTERIZER_DESC CreateWireframeRasterizer() {
        D3D12_RASTERIZER_DESC desc = CreateDefaultRasterizer();
        // 裏面非表示を無効化し、両面を描画
        desc.CullMode = D3D12_CULL_MODE_NONE;
        // 三角形の中を塗りつぶさない
        desc.FillMode = D3D12_FILL_MODE_WIREFRAME;

        return desc;
    }

    /// <summary>
    /// 標準的なDepthStencilStateを生成する
    /// </summary>
    /// <remarks>
    /// 深度書き込みを有効化し、手前のオブジェクトを優先して描画する。
    /// </remarks>
    /// <returns>DepthStencil設定</returns>
    static D3D12_DEPTH_STENCIL_DESC CreateDefaultDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc{};
        // Depthの機能を有効化する
        desc.DepthEnable = true;
        // 書き込みします
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        // 比較関数はLessEqual。つまり、近ければ描画される
        desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        return desc;
    }

    /// <summary>
    /// 半透明描画用DepthStencilStateを生成する
    /// </summary>
    /// <remarks>
    /// 深度テストは行うが、深度値を書き込まない設定。
    /// </remarks>
    /// <returns>DepthStencil設定</returns>
    static D3D12_DEPTH_STENCIL_DESC CreateAlphaDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc = CreateDefaultDepthStencil();
        // 書き込みしない
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        return desc;
    }

    /// <summary>
    /// 壁越し描画用DepthStencilStateを生成する
    /// </summary>
    /// <remarks>
    /// 深度値を書き込まず、壁より奥に存在するオブジェクトのみを描画する。
    /// </remarks>
    /// <returns>DepthStencil設定</returns>
    static D3D12_DEPTH_STENCIL_DESC CreateThroughWallDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc = CreateAlphaDepthStencil();
        // 書き込みしない
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        // 壁より奥だけ描画
        desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
        return desc;
    }

    /// <summary>
    /// 2D描画用DepthStencilStateを生成する
    /// </summary>
    /// <remarks>
    /// 深度処理を無効化し、2D描画向けの設定を生成する。
    /// </remarks>
    /// <returns>DepthStencil設定</returns>
    static D3D12_DEPTH_STENCIL_DESC Create2DDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc{};
        // 2DなのでDepthの機能を無効化する
        desc.DepthEnable = false; 
        // 書き込みしない
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; 
        return desc;
    }

    /// <summary>
    /// 標準的なBlendStateを生成する
    /// </summary>
    /// <remarks>
    /// ブレンドを使用しない不透明描画向け設定。
    /// </remarks>
    /// <returns>Blend設定</returns>
    static D3D12_BLEND_DESC CreateDefaultBlendState() {
        D3D12_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = FALSE;
        
        // すべての色要素を書き込む
        desc.RenderTarget[0].RenderTargetWriteMask = 
            D3D12_COLOR_WRITE_ENABLE_ALL;  
        return desc;
    }

    /// <summary>
    /// 半透明描画用BlendStateを生成する
    /// </summary>
    /// <remarks>
    /// アルファ値を利用した透過合成を有効化する。
    /// </remarks>
    /// <returns>Blend設定</returns>
    static D3D12_BLEND_DESC CreateAlphaBlendState() {
        D3D12_BLEND_DESC desc = CreateDefaultBlendState(); 
        // ブレンドを有効化
        desc.RenderTarget[0].BlendEnable = TRUE;           

        desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        return desc;
    }

    /// <summary>
    /// 基本的な3Dモデル用InputLayoutを生成する
    /// </summary>
    /// <remarks>
    /// Position、Texcoord、Normalを持つ頂点形式を定義する。
    /// </remarks>
    /// <returns>InputLayout設定</returns>
    static std::vector<D3D12_INPUT_ELEMENT_DESC> CreateDefaultInputLayout() {
        std::vector<D3D12_INPUT_ELEMENT_DESC> descs(3);

        // position
        descs[0].SemanticName = "POSITION";
        descs[0].SemanticIndex = 0;
        descs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        descs[0].InputSlot = 0;
        descs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[0].InstanceDataStepRate = 0;

        // texcoord
        descs[1].SemanticName = "TEXCOORD";
        descs[1].SemanticIndex = 0;
        descs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        descs[1].InputSlot = 0;
        descs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[1].InstanceDataStepRate = 0;

        // normal
        descs[2].SemanticName = "NORMAL";
        descs[2].SemanticIndex = 0;
        descs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        descs[2].InputSlot = 0;
        descs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[2].InstanceDataStepRate = 0;

        return descs; // 配列の実体を安全に返す
    }


    /// <summary>
    /// モデル描画用InputLayoutを生成する
    /// </summary>
    /// <remarks>
    /// 3Dモデルデータ向けのPosition、Texcoord、Normalを定義する。
    /// </remarks>
    /// <returns>InputLayout設定</returns>
    static std::vector<D3D12_INPUT_ELEMENT_DESC> CreateModel3dInputLayout() {
        std::vector<D3D12_INPUT_ELEMENT_DESC> descs(3);

        // position
        descs[0].SemanticName = "POSITION";
        descs[0].SemanticIndex = 0;
        descs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        descs[0].InputSlot = 0;
        descs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[0].InstanceDataStepRate = 0;

        // texcoord
        descs[1].SemanticName = "TEXCOORD";
        descs[1].SemanticIndex = 0;
        descs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        descs[1].InputSlot = 0;
        descs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[1].InstanceDataStepRate = 0;

        // normal
        descs[2].SemanticName = "NORMAL";
        descs[2].SemanticIndex = 0;
        descs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        descs[2].InputSlot = 0;
        descs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        descs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        descs[2].InstanceDataStepRate = 0;

        return descs; // 配列の実体を安全に返す
    }
};