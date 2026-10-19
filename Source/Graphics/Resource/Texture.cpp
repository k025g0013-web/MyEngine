#include "Texture.h"

#include <cassert>
#include <utility>

#include "Graphics/Core/DescriptorHeap.h"

namespace Kizuna {

    Texture::~Texture() {
        if (descriptorHeap_ != nullptr &&
            descriptorIndex_ != UINT32_MAX) {
            descriptorHeap_->FreeDescriptor(descriptorIndex_);
        }
    }

    void Texture::SetDescriptor(
        DescriptorHeap *descriptorHeap,
        uint32_t descriptorIndex) {

        assert(descriptorHeap != nullptr);
        assert(descriptorIndex != UINT32_MAX);
        assert(descriptorHeap_ == nullptr);

        descriptorHeap_ = descriptorHeap;
        descriptorIndex_ = descriptorIndex;
    }

    void Texture::SetData(const TextureData &data) {
        data_ = data;
        data_.resource = resource_.Get();
    }

    void Texture::SetResource(
        Microsoft::WRL::ComPtr<ID3D12Resource> resource) {

        resource_ = std::move(resource);
        data_.resource = resource_.Get();
    }

    void Texture::SetIntermediate(
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediate) {

        intermediate_ = std::move(intermediate);
    }
}