#include "RootSignature.h"

#include "Core/Logger.h"

#include <cassert>

namespace Kizuna {
    void RootSignature::Create(ID3D12Device *device, Logger *logger, Type type) {
        assert(device != nullptr);
        assert(logger != nullptr);

        // RootSignatureの設定
        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        // テクスチャ用DescriptorRange
        D3D12_DESCRIPTOR_RANGE descriptorRange{};
        descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange.NumDescriptors = 1;
        descriptorRange.BaseShaderRegister = 0;
        descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        // RootParameter
        D3D12_ROOT_PARAMETER rootParameters[5]{};

        UINT rootParameterCount = 0;

        // CBVを設定する共通処理
        auto SetCBV = [&](UINT index,
            UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility) {
                rootParameters[index].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
                rootParameters[index].ShaderVisibility = visibility;
                rootParameters[index].Descriptor.ShaderRegister = shaderRegister;
                rootParameters[index].Descriptor.RegisterSpace = 0;
            };

        // テクスチャ用DescriptorTableを設定する共通処理
        auto SetTexture = [&](UINT index) {
            rootParameters[index].ParameterType =
                D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            rootParameters[index].ShaderVisibility =
                D3D12_SHADER_VISIBILITY_PIXEL;
            rootParameters[index].DescriptorTable.pDescriptorRanges = &descriptorRange;
            rootParameters[index].DescriptorTable.NumDescriptorRanges = 1;
        };

        // 種類ごとのRootParameter設定
        switch (type) {
        case Type::Skinny3D:
            SetCBV(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            SetCBV(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            SetTexture(2);

            SetCBV(3, 1, D3D12_SHADER_VISIBILITY_PIXEL);

            rootParameterCount = 4;
            break;

        case Type::Skinny2D:
            SetCBV(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            SetCBV(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            SetTexture(2);

            rootParameterCount = 3;
            break;

        case Type::ThroughWall3D:
            SetCBV(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);
            SetCBV(1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            SetTexture(2);

            SetCBV(3, 1, D3D12_SHADER_VISIBILITY_PIXEL);
            SetCBV(4, 2, D3D12_SHADER_VISIBILITY_PIXEL);

            rootParameterCount = 5;
            break;

        case Type::Particles:
            SetCBV(0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

            rootParameters[1].ParameterType =
                D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
            rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange;
            rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;

            SetTexture(2);

            SetCBV(3, 1, D3D12_SHADER_VISIBILITY_PIXEL);

            rootParameterCount = 4;
            break;

        default:
            assert(false && "未対応のRootSignatureタイプです");
            return;
        }

        descriptionRootSignature.NumParameters =
            rootParameterCount;

        descriptionRootSignature.pParameters =
            rootParameters;

        // StaticSampler
        D3D12_STATIC_SAMPLER_DESC staticSampler{};
        staticSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler.MipLODBias = 0.0f;
        staticSampler.MaxAnisotropy = 1;
        staticSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        staticSampler.MinLOD = 0.0f;
        staticSampler.MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler.ShaderRegister = 0;
        staticSampler.RegisterSpace = 0;
        staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        descriptionRootSignature.pStaticSamplers = &staticSampler;
        descriptionRootSignature.NumStaticSamplers = 1;

        // Serialize
        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3D12SerializeRootSignature(
            &descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

        if (FAILED(hr)) {
            if (errorBlob) {
                logger->Log(
                    reinterpret_cast<char *>(
                        errorBlob->GetBufferPointer()));
            }

            assert(false && "RootSignatureのシリアライズに失敗しました");
            return;
        }

        // Create
        hr = device->CreateRootSignature(0, 
            signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

        if (FAILED(hr)) {
            assert(false && "RootSignatureの生成に失敗しました");
        }
    }
}