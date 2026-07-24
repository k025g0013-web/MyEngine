#pragma once

#include <memory>

namespace Kizuna {
    class Scene;

    class SceneManager {
    public:
        /// 初期シーン設定
        void SetFirstScene(std::unique_ptr<Scene> scene);

        /// シーン切り替え
        void ChangeScene(std::unique_ptr<Scene> scene);

        /// 更新
        void Update();

        /// 描画
        void Draw();

    private:
        std::unique_ptr<Scene> currentScene_;
    };

}