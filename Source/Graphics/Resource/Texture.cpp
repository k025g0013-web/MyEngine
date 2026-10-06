#include "Texture.h"

namespace Kizuna {

    void Texture::SetData(
        const TextureData &data) {

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