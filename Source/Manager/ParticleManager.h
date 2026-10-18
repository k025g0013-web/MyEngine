#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Object/Particle/Particle.h"
#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/StructuredBuffer.h"
#include "Graphics/Material/Material.h"
#include "Graphics/Resource/Texture.h"
#include "Graphics/Resource/TextureLoader.h"

namespace Kizuna {

    class CameraManager;
    class DescriptorHeap;
    class TextureManager;

    struct ParticleTexture {
        std::unique_ptr<Texture> texture;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
    };

    class ParticleManager {
    public:
        void Initialize(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            DescriptorHeap *srvHeap,
            TextureManager *textureManager
        );

        void Emit(
            const std::string &texturePath,
            uint32_t count,
            const Transform &transform,
            const Vector3 &velocity,
            float lifeTime,
            const Vector4 &color
        );

        void Update(float deltaTime, CameraManager *camera);

        void Draw(ID3D12GraphicsCommandList *commandList);

        uint32_t GetAliveCount() const;

    private:
        // パーティクル用テクスチャを読み込む
        D3D12_GPU_DESCRIPTOR_HANDLE LoadParticleTexture(
            const std::string &texturePath
        );

        ID3D12Device *device_ = nullptr;
        ID3D12GraphicsCommandList *commandList_ = nullptr;
        DescriptorHeap *srvHeap_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        TextureLoader textureLoader_;

        std::vector<Particle> particles_;
        std::vector<std::string> particleTexturePaths_;

        std::unordered_map<std::string, ParticleTexture>
            particleTextures_;

        Mesh mesh_;
        std::unique_ptr<Material> material_;
        std::unique_ptr<StructuredBuffer> transformationBuffer_;

        CameraManager *camera_ = nullptr;

        static constexpr uint32_t kMaxParticles = 1024;
        static constexpr float kParticleSpacing = 1.2f;
    };

}