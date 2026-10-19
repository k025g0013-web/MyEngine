#pragma once

#include <cstdint>
#include <d3d12.h>
#include <DirectXTex/DirectXTex.h>
#include <wrl.h>

namespace Kizuna {

    class DescriptorHeap;

    struct TextureData {
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
        DirectX::TexMetadata metadata{};
        ID3D12Resource *resource = nullptr;
    };

    class Texture {
    public:
        ~Texture();

        Texture() = default;

        // コピーによるディスクリプタの二重解放を防ぐ
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        // 必要ならムーブも禁止して、所有権を単純化する
        Texture(Texture &&) = delete;
        Texture &operator=(Texture &&) = delete;

        void SetDescriptor(
            DescriptorHeap *descriptorHeap,
            uint32_t descriptorIndex);

        void SetData(const TextureData &data);

        const TextureData &GetData() const {
            return data_;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const {
            return data_.gpuHandle;
        }

        ID3D12Resource *GetResource() const {
            return resource_.Get();
        }

        void SetResource(
            Microsoft::WRL::ComPtr<ID3D12Resource> resource);

        void SetIntermediate(
            Microsoft::WRL::ComPtr<ID3D12Resource> intermediate);

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediate_;

        TextureData data_{};

        DescriptorHeap *descriptorHeap_ = nullptr;
        uint32_t descriptorIndex_ = UINT32_MAX;
    };
}