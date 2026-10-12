#include "SpriteManager.h"

#include <cassert>
#include <utility>

namespace Kizuna {

    void SpriteManager::Initialize(
        ID3D12Device *device) {

        assert(device);
        device_ = device;
    }

    void SpriteManager::CreateSprite(
        float left, float top, float right, float bottom,
        const TextureData &texture, int textureIndex, uint32_t color) {
        assert(device_);

        auto sprite = std::make_unique<Sprite>();

        sprite->Initialize(device_,
            left, top, right, bottom, color);

        SpriteEntry entry;
        entry.sprite = std::move(sprite);
        entry.texture = texture;
        entry.textureIndex = textureIndex;

        sprites_.push_back(std::move(entry));
    }

    void SpriteManager::Update(
        uint32_t windowWidth,
        uint32_t windowHeight) {

        for (auto &entry : sprites_) {
            entry.sprite->Update(
                windowWidth,
                windowHeight);
        }
    }

    void SpriteManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        assert(commandList);

        for (auto &entry : sprites_) {
            entry.sprite->Draw(
                commandList,
                entry.texture);
        }
    }

    void SpriteManager::DeleteSprite(size_t index) {
        assert(index < sprites_.size());

        sprites_.erase(sprites_.begin() + index);
    }

    Sprite *SpriteManager::GetSprite(size_t index) {
        assert(index < sprites_.size());

        return sprites_[index].sprite.get();
    }

    void SpriteManager::SetTexture(
        size_t index,
        const TextureData &texture,
        int textureIndex) {

        assert(index < sprites_.size());

        sprites_[index].texture = texture;
        sprites_[index].textureIndex = textureIndex;
    }

    int SpriteManager::GetTextureIndex(size_t index) const {
        assert(index < sprites_.size());

        return sprites_[index].textureIndex;
    }
}