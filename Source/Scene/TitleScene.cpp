#include "TitleScene.h"

#include "GameScene.h"
#include "Input/Keyboard.h"

#include "KizunaEngine.h"

namespace Kizuna {

    void TitleScene::Initialize()
    {
        finished_ = false;

        // タイトル用の初期化
    }

    void TitleScene::Update()
    {
    }

    void TitleScene::Draw()
    {
        // タイトル画面の描画
    }

    bool TitleScene::IsFinished() const
    {
        return finished_;
    }

    std::unique_ptr<Scene> TitleScene::NextScene()
    {
        return std::make_unique<GameScene>();
    }

}