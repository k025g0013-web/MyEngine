#pragma once

#include "Scene.h"

namespace Kizuna {

    class TitleScene : public Scene {
    public:
        void Initialize() override;

        void Update() override;

        void Draw() override;

        bool IsFinished() const override;

        std::unique_ptr<Scene> NextScene() override;

    private:
        bool finished_ = false;
    };

}