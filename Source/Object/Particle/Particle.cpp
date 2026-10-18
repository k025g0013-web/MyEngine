#include "Particle.h"

#include <algorithm>
#include <cassert>

namespace Kizuna {

    void Particle::Initialize(
        const Transform &transform,
        const Vector3 &velocity,
        float lifeTime,
        const Vector4 &color
    ) {
        transform_ = transform;
        velocity_ = velocity;

        lifeTime_ = lifeTime;
        initialLifeTime_ = lifeTime;

        color_ = color;
        color_.w = 1.0f;

        isAlive_ = lifeTime_ > 0.0f;
    }

    void Particle::Update(float deltaTime) {
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
        lifeTime_ = (std::max)(0.0f, lifeTime_ - deltaTime);

        // 残り寿命の割合を透明度に反映
        if (initialLifeTime_ > 0.0f) {
            color_.w = lifeTime_ / initialLifeTime_;
        }

        // 寿命が尽きたら死亡
        if (lifeTime_ <= 0.0f) {
            isAlive_ = false;
        }
    }
}