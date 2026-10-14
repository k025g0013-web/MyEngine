#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Object/Particle/Particle.h"
#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/StructuredBuffer.h"
#include "Graphics/Material/Material.h"

namespace Kizuna {

    class CameraManager;
    class DescriptorHeap;
    class TextureManager;

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
            float lifeTime
        );

        void Update(float deltaTime, CameraManager *camera);

        void Draw(ID3D12GraphicsCommandList *commandList);

        uint32_t GetAliveCount() const;

    private:
        ID3D12Device *device_ = nullptr;
        ID3D12GraphicsCommandList *commandList_ = nullptr;
        DescriptorHeap *srvHeap_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        std::vector<Particle> particles_;
        std::vector<std::string> particleTexturePaths_;

        Mesh mesh_;
        std::unique_ptr<Material> material_;
        std::unique_ptr<StructuredBuffer> transformationBuffer_;

        CameraManager *camera_ = nullptr;

        static constexpr uint32_t kMaxParticles = 1024;
        static constexpr float kParticleSpacing = 1.2f;
    };

}