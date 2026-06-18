#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>

class PipelineElements {
public:
    // 標準的なRasterizer
    static D3D12_RASTERIZER_DESC CreateDefaultRasterizer() {
        D3D12_RASTERIZER_DESC desc{};
        // 裏面(時計回り)を表示しない
        desc.CullMode = D3D12_CULL_MODE_BACK;
        // 三角形の中を塗りつぶす
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        return desc;
    }

    // 両面描画用のRasterizer
    static D3D12_RASTERIZER_DESC CreateNoCullRasterizer() {
        D3D12_RASTERIZER_DESC desc = CreateDefaultRasterizer();
        // 裏面非表示を無効化し、両面を描画
        desc.CullMode = D3D12_CULL_MODE_NONE;
        return desc;
    }

    // ワイヤーフレーム描画用のRasterizer
    static D3D12_RASTERIZER_DESC CreateWireframeRasterizer() {
        D3D12_RASTERIZER_DESC desc = CreateDefaultRasterizer();
        // 裏面非表示を無効化し、両面を描画
        desc.CullMode = D3D12_CULL_MODE_NONE;
        // 三角形の中を塗りつぶさない
        desc.FillMode = D3D12_FILL_MODE_WIREFRAME;

        return desc;
    }

    // 標準的なDepthStencil
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

    // 半透明用のDepthStencil
    static D3D12_DEPTH_STENCIL_DESC CreateAlphaDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc = CreateDefaultDepthStencil();
        // 書き込みしない
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        return desc;
    }

    // 2D不透明用のDepthStencil
    static D3D12_DEPTH_STENCIL_DESC Create2DDepthStencil() {
        D3D12_DEPTH_STENCIL_DESC desc{};
        // 2DなのでDepthの機能を無効化する
        desc.DepthEnable = false; 
        // 書き込みしない
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; 
        return desc;
    }

    // 標準的なBlendState
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

    // 半透明用のBlendState
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

    // 3Dモデル用の標準的なInputLayout
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


    // 3Dモデル用の標準的なInputLayout
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