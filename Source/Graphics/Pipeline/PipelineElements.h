#pragma once
#include "PipelineConfig.h"

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>

namespace Kizuna {
    /// <summary>
    /// GraphicsPipelineで使用する各種設定要素を生成するクラス
    /// </summary>
    /// <remarks>
    /// Rasterizer、DepthStencil、BlendState、InputLayoutなどの
    /// DirectX12パイプライン設定を生成するためのユーティリティクラス。
    /// </remarks>
    class PipelineElements {
    public:
       
        static D3D12_RASTERIZER_DESC CreateRasterizer(RasterizerMode mode) {
            D3D12_RASTERIZER_DESC desc{};

            switch (mode) {
            case RasterizerMode::Default:
                desc.CullMode = D3D12_CULL_MODE_BACK;
                desc.FillMode = D3D12_FILL_MODE_SOLID;
                break;

            case RasterizerMode::NoCull:
                desc.CullMode = D3D12_CULL_MODE_NONE;
                desc.FillMode = D3D12_FILL_MODE_SOLID;
                break;

            case RasterizerMode::Wireframe:
                desc.CullMode = D3D12_CULL_MODE_NONE;
                desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
                break;
            }

            return desc;
        }

        static D3D12_DEPTH_STENCIL_DESC CreateDepthStencil(
            DepthMode mode
        ) {
            D3D12_DEPTH_STENCIL_DESC desc{};

            switch (mode) {
            case DepthMode::Default:
                desc.DepthEnable = true;
                desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
                desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
                break;

            case DepthMode::Alpha:
                desc.DepthEnable = true;
                desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
                desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
                break;

            case DepthMode::ThroughWall:
                desc.DepthEnable = true;
                desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
                desc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
                break;

            case DepthMode::Disable:
                desc.DepthEnable = false;
                desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
                break;
            }

            return desc;
        }

        static D3D12_BLEND_DESC CreateBlendState(
            BlendMode mode
        ) {
            D3D12_BLEND_DESC desc{};

            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;

            auto &target = desc.RenderTarget[0];

            target.RenderTargetWriteMask =
                D3D12_COLOR_WRITE_ENABLE_ALL;

            switch (mode) {

                //==================================================
                // 通常
                //==================================================
            case BlendMode::Default:
                target.BlendEnable = FALSE;
                break;


                //==================================================
                // Alpha
                //==================================================
            case BlendMode::Alpha:
                target.BlendEnable = TRUE;

                target.SrcBlend =
                    D3D12_BLEND_SRC_ALPHA;

                target.DestBlend =
                    D3D12_BLEND_INV_SRC_ALPHA;

                target.BlendOp =
                    D3D12_BLEND_OP_ADD;

                target.SrcBlendAlpha =
                    D3D12_BLEND_ONE;

                target.DestBlendAlpha =
                    D3D12_BLEND_ZERO;

                target.BlendOpAlpha =
                    D3D12_BLEND_OP_ADD;

                break;


                //==================================================
                // Add
                //==================================================
            case BlendMode::Add:
                target.BlendEnable = TRUE;

                target.SrcBlend =
                    D3D12_BLEND_SRC_ALPHA;

                target.DestBlend =
                    D3D12_BLEND_ONE;

                target.BlendOp =
                    D3D12_BLEND_OP_ADD;

                target.SrcBlendAlpha =
                    D3D12_BLEND_ONE;

                target.DestBlendAlpha =
                    D3D12_BLEND_ZERO;

                target.BlendOpAlpha =
                    D3D12_BLEND_OP_ADD;

                break;


                //==================================================
                // Subtract
                //==================================================
            case BlendMode::Subtract:
                target.BlendEnable = TRUE;

                target.SrcBlend =
                    D3D12_BLEND_SRC_ALPHA;

                target.DestBlend =
                    D3D12_BLEND_ONE;

                target.BlendOp =
                    D3D12_BLEND_OP_REV_SUBTRACT;

                target.SrcBlendAlpha =
                    D3D12_BLEND_ONE;

                target.DestBlendAlpha =
                    D3D12_BLEND_ZERO;

                target.BlendOpAlpha =
                    D3D12_BLEND_OP_ADD;

                break;


                //==================================================
                // Multiply
                //==================================================
            case BlendMode::Multiply:
                target.BlendEnable = TRUE;

                target.SrcBlend =
                    D3D12_BLEND_DEST_COLOR;

                target.DestBlend =
                    D3D12_BLEND_ZERO;

                target.BlendOp =
                    D3D12_BLEND_OP_ADD;

                target.SrcBlendAlpha =
                    D3D12_BLEND_ONE;

                target.DestBlendAlpha =
                    D3D12_BLEND_ZERO;

                target.BlendOpAlpha =
                    D3D12_BLEND_OP_ADD;

                break;


                //==================================================
                // Screen
                //==================================================
            case BlendMode::Screen:
                target.BlendEnable = TRUE;

                target.SrcBlend =
                    D3D12_BLEND_INV_DEST_COLOR;

                target.DestBlend =
                    D3D12_BLEND_ONE;

                target.BlendOp =
                    D3D12_BLEND_OP_ADD;

                target.SrcBlendAlpha =
                    D3D12_BLEND_ONE;

                target.DestBlendAlpha =
                    D3D12_BLEND_ZERO;

                target.BlendOpAlpha =
                    D3D12_BLEND_OP_ADD;

                break;
            }

            return desc;
        }
        
        static std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout(
            InputLayoutMode mode
        ) {
            std::vector<D3D12_INPUT_ELEMENT_DESC> descs;

            switch (mode) {
            case InputLayoutMode::Default:
            case InputLayoutMode::Model3d:
                descs.resize(3);

                // POSITION
                descs[0].SemanticName = "POSITION";
                descs[0].SemanticIndex = 0;
                descs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                descs[0].InputSlot = 0;
                descs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
                descs[0].InputSlotClass =
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                descs[0].InstanceDataStepRate = 0;

                // TEXCOORD
                descs[1].SemanticName = "TEXCOORD";
                descs[1].SemanticIndex = 0;
                descs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
                descs[1].InputSlot = 0;
                descs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
                descs[1].InputSlotClass =
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                descs[1].InstanceDataStepRate = 0;

                // NORMAL
                descs[2].SemanticName = "NORMAL";
                descs[2].SemanticIndex = 0;
                descs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
                descs[2].InputSlot = 0;
                descs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
                descs[2].InputSlotClass =
                    D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                descs[2].InstanceDataStepRate = 0;

                break;
            }

            return descs;
        }
    };
}