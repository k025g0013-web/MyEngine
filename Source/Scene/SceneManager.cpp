#include "SceneManager.h"

#include "Scene.h"

namespace Kizuna {

    void SceneManager::SetFirstScene(std::unique_ptr<Scene> scene)
    {
        currentScene_ = std::move(scene);

        if (currentScene_) {
            currentScene_->Initialize();
        }
    }

    void SceneManager::ChangeScene(std::unique_ptr<Scene> scene)
    {
        currentScene_ = std::move(scene);

        if (currentScene_) {
            currentScene_->Initialize();
        }
    }

    void SceneManager::Update()
    {
        if (currentScene_) {
            currentScene_->Update();
        }

        if (currentScene_ && currentScene_->IsFinished()) {
            ChangeScene(currentScene_->NextScene());
        }
    }

    void SceneManager::Draw()
    {
        if (currentScene_) {
            currentScene_->Draw();
        }
    }

}