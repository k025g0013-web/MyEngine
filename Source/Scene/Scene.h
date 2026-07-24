#pragma once

#include <memory>

namespace Kizuna {
    /// <summary>
    /// シーンの基底クラス
    /// </summary>
    /// <remarks>
    /// すべてのシーンで共通となる
    /// 初期化、更新、描画の管理を行う。
    /// </remarks>
    class Scene {
    public:
        virtual ~Scene() = default;

        /// <summary>
        /// シーンを初期化する
        /// </summary>
        virtual void Initialize() = 0;

        /// <summary>
        /// シーンを更新する
        /// </summary>
        virtual void Update() = 0;

        /// <summary>
        /// シーンを描画する
        /// </summary>
        virtual void Draw() = 0;

        /// <summary>
        /// シーンが終了したか取得する
        /// </summary>
        virtual bool IsFinished() const = 0;

        /// <summary>
        /// </summary>
        virtual std::unique_ptr<Scene> NextScene() = 0;
    };

}