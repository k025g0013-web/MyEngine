#include "Particle.h"

#include <algorithm>
#include <cassert>

namespace Kizuna {

    void Particle::Initialize(
        const Transform &transform,
        const Vector3 &velocity,
        float lifeTime
    ) {
        // 初期状態を設定
        transform_ = transform;
        velocity_ = velocity;
        lifeTime_ = lifeTime;

        // 寿命が正の場合のみ生存させる
        isAlive_ = lifeTime_ > 0.0f;
    }

    void Particle::Update(float deltaTime) {

        // 死亡済みのパーティクルは更新しない
        if (!isAlive_) {
            return;
        }

        assert(deltaTime >= 0.0f);

        if (deltaTime < 0.0f) {
            return;
        }

        // 位置を更新
        transform_.translate.x += velocity_.x * deltaTime;
        transform_.translate.y += velocity_.y * deltaTime;
        transform_.translate.z += velocity_.z * deltaTime;

        // 残り寿命を更新
        lifeTime_ = std::max(0.0f, lifeTime_ - deltaTime);

        // 寿命が尽きたら死亡
        if (lifeTime_ <= 0.0f) {
            isAlive_ = false;
        }
    }
}