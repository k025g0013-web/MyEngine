#pragma once

#include "Math/Transform.h"

namespace Kizuna {

    /// <summary>
    /// パーティクル1個分の状態と更新処理を管理するクラス
    /// </summary>
    class Particle {
    public:

        /// <summary>
        /// パーティクルを初期化する
        /// </summary>
        void Initialize(
            const Transform &transform,
            const Vector3 &velocity,
            float lifeTime
        );

        /// <summary>
        /// パーティクルを更新する
        /// </summary>
        void Update(float deltaTime);

        /// <summary>
        /// 生存しているか取得する
        /// </summary>
        bool IsAlive() const {
            return isAlive_;
        }

        /// <summary>
        /// Transformを取得する
        /// </summary>
        const Transform &GetTransform() const {
            return transform_;
        }

        /// <summary>
        /// Transformを変更する
        /// </summary>
        Transform &GetTransform() {
            return transform_;
        }

        /// <summary>
        /// 速度を取得する
        /// </summary>
        const Vector3 &GetVelocity() const {
            return velocity_;
        }

        /// <summary>
        /// 残り寿命を取得する
        /// </summary>
        float GetLifeTime() const {
            return lifeTime_;
        }

    private:

        // パーティクルのTransform
        Transform transform_{
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        };

        // 移動速度
        Vector3 velocity_{};

        // 残り寿命（秒）
        float lifeTime_ = 0.0f;

        // 生存フラグ
        bool isAlive_ = false;
    };
}