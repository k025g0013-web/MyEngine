#include "ParticleManager.h"

#include "Manager/CameraManager.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Resource/TransformationMatrix.h"
#include "Math/FunctionMatrix.h"

#include "Manager/TextureManager.h"

#include <algorithm>
#include <cassert>
#include <unordered_set>

namespace Kizuna {

    void ParticleManager::Initialize(
        ID3D12Device *device,
        ID3D12GraphicsCommandList *commandList,
        DescriptorHeap *srvHeap,
        TextureManager *textureManager) {

        assert(device);
        assert(commandList);
        assert(srvHeap);
        assert(textureManager);

        device_ = device;
        commandList_ = commandList;
        srvHeap_ = srvHeap;
        textureManager_ = textureManager;

        // パーティクル用テクスチャローダーを初期化
        textureLoader_.Initialize(device_, srvHeap_);

        // パーティクル用の四角形メッシュを作成する。
        std::vector<VertexData> vertices(4);

        vertices[0].position = { -0.5f,  0.5f, 0.0f, 1.0f };
        vertices[0].texcoord = { 0.0f, 0.0f };
        vertices[0].normal = { 0.0f, 0.0f, -1.0f };

        vertices[1].position = { 0.5f,  0.5f, 0.0f, 1.0f };
        vertices[1].texcoord = { 1.0f, 0.0f };
        vertices[1].normal = { 0.0f, 0.0f, -1.0f };

        vertices[2].position = { -0.5f, -0.5f, 0.0f, 1.0f };
        vertices[2].texcoord = { 0.0f, 1.0f };
        vertices[2].normal = { 0.0f, 0.0f, -1.0f };

        vertices[3].position = { 0.5f, -0.5f, 0.0f, 1.0f };
        vertices[3].texcoord = { 1.0f, 1.0f };
        vertices[3].normal = { 0.0f, 0.0f, -1.0f };

        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 1, 3
        };

        mesh_.Create(device_, vertices, indices);

        // マテリアルを作成する。
        material_ = std::make_unique<Material>();
        material_->Initialize(device_, 0xFFFFFFFF, false);

        // インスタンスごとの変換行列を格納する構造化バッファを作成する。
        transformationBuffer_ = std::make_unique<StructuredBuffer>();

        const uint32_t descriptorIndex = srvHeap_->AllocateDescriptor();

        transformationBuffer_->Initialize(
            device_,
            srvHeap_,
            descriptorIndex,
            sizeof(TransformationMatrix),
            kMaxParticles
        );
    }

    void ParticleManager::Emit(
        const std::string &texturePath,
        uint32_t count,
        const Transform &transform,
        const Vector3 &velocity,
        float lifeTime,
        const Vector4 &color) {

        assert(textureManager_);

        if (count == 0 || lifeTime <= 0.0f) {
            return;
        }

        // 最大数を超えないようにする。
        const uint32_t aliveCount =
            static_cast<uint32_t>(particles_.size());

        const uint32_t available =
            kMaxParticles - (std::min)(aliveCount, kMaxParticles);

        count = (std::min)(count, available);

        if (count == 0) {
            return;
        }

        const D3D12_GPU_DESCRIPTOR_HANDLE textureHandle =
            LoadParticleTexture(texturePath);

        for (uint32_t i = 0; i < count; ++i) {
            Particle particle;

            // 全パーティクルを指定された同じ座標に生成する
            particle.Initialize(
                transform,
                velocity,
                lifeTime,
                color
            );

            if (particle.IsAlive()) {
                particles_.push_back(particle);
                particleTexturePaths_.push_back(texturePath);
            }
        }
    }

    void ParticleManager::Update(
        float deltaTime,
        CameraManager *camera) {

        assert(camera);

        camera_ = camera;

        // 生存時間を更新し、死亡したパーティクルとテクスチャ情報を同時に削除する。
        for (size_t i = 0; i < particles_.size();) {
            particles_[i].Update(deltaTime);

            if (!particles_[i].IsAlive()) {
                particles_.erase(particles_.begin() + i);
                particleTexturePaths_.erase(
                    particleTexturePaths_.begin() + i
                );
            } else {
                ++i;
            }
        }
    }

    void ParticleManager::Draw(
        ID3D12GraphicsCommandList *commandList) {

        assert(commandList);

        if (particles_.empty() || !camera_) {
            return;
        }

        assert(material_);
        assert(transformationBuffer_);

        // マテリアルとメッシュは全パーティクルで共有する。
        commandList->SetGraphicsRootConstantBufferView(
            0,
            material_->GetGPUVirtualAddress()
        );

        mesh_.BindBuffers(commandList);

        // 同じテクスチャを使うパーティクルをまとめて描画する。
        std::unordered_set<std::string> drawnTextures;

        for (size_t first = 0; first < particles_.size(); ++first) {
            const std::string &texturePath =
                particleTexturePaths_[first];

            if (drawnTextures.contains(texturePath)) {
                continue;
            }

            drawnTextures.insert(texturePath);

            const auto textureIt = particleTextures_.find(texturePath);

            assert(textureIt != particleTextures_.end());

            const D3D12_GPU_DESCRIPTOR_HANDLE textureHandle =
                textureIt->second.gpuHandle;

            std::vector<TransformationMatrix> matrices;
            matrices.reserve(particles_.size());

            // このテクスチャを使用するパーティクルの行列だけを作る。
            for (size_t i = 0; i < particles_.size(); ++i) {
                if (particleTexturePaths_[i] != texturePath) {
                    continue;
                }

                const Transform &transform =
                    particles_[i].GetTransform();

                TransformationMatrix matrix{};

                matrix.World = Math::MakeAffineMatrix(
                    transform.scale,
                    transform.rotate,
                    transform.translate
                );

                matrix.WVP = Math::Multiply(
                    matrix.World,
                    camera_->GetViewProjectionMatrix()
                );

                matrix.color = particles_[i].GetColor();

                matrices.push_back(matrix);
            }

            if (matrices.empty()) {
                continue;
            }

            // CPU側の変換行列をGPU側の構造化バッファへ転送する。
            transformationBuffer_->Update(
                matrices.data(),
                matrices.size() * sizeof(TransformationMatrix)
            );

            // Root Parameter 1: TransformationMatrixのSRV
            commandList->SetGraphicsRootDescriptorTable(
                1,
                transformationBuffer_->GetGPUHandle()
            );

            // Root Parameter 2: パーティクルのテクスチャ
            commandList->SetGraphicsRootDescriptorTable(
                2,
                textureHandle
            );

            // 同じテクスチャのパーティクルをインスタンシング描画する。
            commandList->DrawIndexedInstanced(
                mesh_.GetIndexCount(),
                static_cast<UINT>(matrices.size()),
                0,
                0,
                0
            );
        }
    }

    uint32_t ParticleManager::GetAliveCount() const {
        return static_cast<uint32_t>(particles_.size());
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleManager::LoadParticleTexture(
        const std::string &texturePath) {

        // すでに読み込まれている場合は再利用する
        auto it = particleTextures_.find(texturePath);

        if (it != particleTextures_.end()) {
            return it->second.gpuHandle;
        }

        // 新しいテクスチャを読み込む
        ParticleTexture particleTexture;
        particleTexture.texture = std::make_unique<Texture>();

        TextureData textureData = textureLoader_.LoadTexture(
            commandList_,
            texturePath,
            *particleTexture.texture
        );

        particleTexture.gpuHandle = textureData.gpuHandle;

        assert(particleTexture.gpuHandle.ptr != 0);

        // 読み込み済みテクスチャとして登録
        auto [insertedIt, inserted] = particleTextures_.emplace(
            texturePath,
            std::move(particleTexture)
        );

        assert(inserted);

        return insertedIt->second.gpuHandle;
    }
}