#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "Object/Sprite/Sprite.h"
#include "Graphics/Resource/Texture.h"

namespace Kizuna {

    class SpriteManager {
    public:
        void Initialize(ID3D12Device* device);

        void CreateSprite(
            float left,
            float top,
            float right,
            float bottom,
            const TextureData& texture,
            int textureIndex = 0,
            uint32_t color = 0xFFFFFFFF);

        void Update(uint32_t windowWidth, uint32_t windowHeight);

        void Draw(ID3D12GraphicsCommandList* commandList);

        void DeleteSprite(size_t index);

        Sprite* GetSprite(size_t index);

        void SetTexture(
            size_t index,
            const TextureData& texture,
            int textureIndex);

        int GetTextureIndex(size_t index) const;

        size_t GetSpriteCount() const {
            return sprites_.size();
        }

    private:
        struct SpriteEntry {
            std::unique_ptr<Sprite> sprite;
            TextureData texture;
            int textureIndex = 0;
        };

        ID3D12Device* device_ = nullptr;
        std::vector<SpriteEntry> sprites_;
    };
}