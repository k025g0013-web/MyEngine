#include "KizunaEngine.h"
#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Object/Object3D.h"
#include "Renderer/Sprite.h"
#include "Renderer/ThroughWallRenderer.h"
#include "External/ImGuiManager.h"

#include "Object/TriangleObject.h"
#include "Object/PlaneObject.h"
#include "Object/SphereObject.h"
#include "Object/ModelObject.h"

#include <cstdint>
#include <algorithm>

using namespace Kizuna;

namespace {
    // 球モデル分割数
    constexpr int32_t kSubdivision = 16;

    // クライアント領域のサイズ
    constexpr int32_t kClientWidth = 1280;
    constexpr int32_t kClientHeight = 720;

    // ライトバインド用の定数バッファレジスタ番号
    constexpr UINT kLightRegisterIndex = 3;
}

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    //-------------------------------------------------------------------------
    // 基盤初期化
    //-------------------------------------------------------------------------
    auto engine = std::make_unique<KizunaEngine>();

    engine->Initialize(L"CG2", kClientWidth, kClientHeight);

    ID3D12Device *device = engine->GetDevice();

    ID3D12GraphicsCommandList *commandList = engine->GetCommandList();

    //-------------------------------------------------------------------------
    // データ生成・初期リソースのセットアップ
    //-------------------------------------------------------------------------

    // 壁越し描画
    ThroughWallRenderer throughWallRenderer;

    static bool isThroughWall = true;

    // 複数モデルのリソース初期化
    std::vector<std::unique_ptr<Object3D>> models;
    std::vector<Transform> transforms;

    //-------------------------------------------------------------------------
    // テクスチャの読み込み
    //-------------------------------------------------------------------------
    Texture *textureManager =
        engine->GetTextureManager();

    // 外部テクスチャとして使用するもの
    TextureData textures[6];

    textures[0] =
        textureManager->LoadTexture(
            commandList,
            "resources/Models/fence/fence.png");

    textures[1] =
        textureManager->LoadTexture(
            commandList,
            "resources/monsterBall.png");

    textures[2] =
        textureManager->LoadTexture(
            commandList,
            "resources/cube.jpg");

    textures[3] =
        textureManager->LoadTexture(
            commandList,
            "resources/axis.jpg");

    textures[4] =
        textureManager->LoadTexture(
            commandList,
            "resources/checkerBoard.png");

    textures[5] =
        textureManager->LoadTexture(
            commandList,
            "resources/white1x1.png");

    //-------------------------------------------------------------------------
    // 初期オブジェクト
    //-------------------------------------------------------------------------

    {
        // fenceはModelObject側で
        // fence.pngを自動的に読み込む
        auto object =
            std::make_unique<ModelObject>(
                "fence",
                textureManager);

        object->Create(
            device,
            commandList,
            0xFFFFFFFF,
            true);

        models.push_back(
            std::move(object));

        transforms.push_back({
            .scale = {1.0f, 1.0f, 1.0f},
            .rotate = {0.0f, 0.0f, 0.0f},
            .translate = {0.0f, 0.0f, 0.0f}
            });

        throughWallRenderer.AddObject(
            models.back().get(),
            0x00000000,
            Style::Solid);
    }

    //-------------------------------------------------------------------------
    // ライト / カメラ
    //-------------------------------------------------------------------------

    Lighting lighting;
    lighting.Initialize(device);

    CameraManager camera;

    camera.Initialize(
        float(kClientWidth),
        float(kClientHeight),
        engine->GetKeyboard(),
        engine->GetMouse(),
        engine->GetGamePad());

    Transform cameraTransform = {
        .scale = {1.0f, 1.0f, 1.0f},
        .rotate = {0.3f, 0.0f, 0.0f},
        .translate = {0.0f, 1.5f, -5.0f}
    };

    //-------------------------------------------------------------------------
    // オーディオ
    //-------------------------------------------------------------------------

    engine->GetAudioManager()->Load(
        "fanfare",
        "Resources/fanfare.wav");

    engine->GetAudioManager()->Load(
        "bgm_music",
        "Resources/music.mp3");

    engine->GetAudioManager()->Play(
        "fanfare",
        false,
        1.0f);

    //-------------------------------------------------------------------------
    // メインループ
    //-------------------------------------------------------------------------

    while (engine->ProcessMessage()) {

        engine->UpdateInput();

        //===============
        // 更新処理
        //===============

#ifdef _DEBUG

        if (engine->GetKeyboard()->TriggerKey(DIK_Q) ||
            engine->GetGamePad()->TriggerButton(
                XINPUT_GAMEPAD_A)) {

            camera.ToggleCamera();
        }

#endif

        // カメラ更新
        camera.Update(cameraTransform);

        //-------------------------------------------------------------------------
        // ImGui
        //-------------------------------------------------------------------------

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->BeginFrame();

        ImGui::DockSpaceOverViewport(
            ImGui::GetMainViewport()->ID,
            nullptr,
            ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("Setting");

        ImGui::Text(
            "Camera Mode : %s (Q:KeyBoad or A:GamePad to Toggle)",
            camera.GetStateName());

        ImGui::Separator();

        static int currentObjectIndex = 0;
        static int createType = 0;

        const char *createItems[] = {
            "Triangle",
            "Plane",
            "Cube",
            "Sphere",
            "Teapot",
            "Bunny",
            "Suzanne",
        };

        ImGui::Combo(
            "Model",
            &createType,
            createItems,
            IM_ARRAYSIZE(createItems));

        ImGui::Checkbox(
            "Enable Through-Wall",
            &isThroughWall);

        //-------------------------------------------------------------------------
        // Create Model
        //-------------------------------------------------------------------------

        if (ImGui::Button("Create Model")) {

            std::unique_ptr<Object3D> object;

            switch (createType) {

            case 0:
                // Triangle
                object =
                    std::make_unique<TriangleObject>(
                        Vector3{ -0.5f, -0.5f, 0.0f },
                        Vector3{ 0.0f, 0.5f, 0.0f },
                        Vector3{ 0.5f, -0.5f, 0.0f });
                break;

            case 1:
                // Plane
                // plane.pngを自動読み込み
                object =
                    std::make_unique<ModelObject>(
                        "plane",
                        textureManager);
                break;

            case 2:
                // Cube
                // cube.pngを自動読み込み
                object =
                    std::make_unique<ModelObject>(
                        "cube",
                        textureManager);
                break;

            case 3:
                // Sphere
                object =
                    std::make_unique<SphereObject>(
                        kSubdivision);
                break;

            case 4:
                // Teapot
                // 外部テクスチャを使用
                object =
                    std::make_unique<ModelObject>(
                        "teapot",
                        textureManager,
                        textures[4]);
                break;

            case 5:
                // Bunny
                // 外部テクスチャを使用
                object =
                    std::make_unique<ModelObject>(
                        "bunny",
                        textureManager,
                        textures[0]);
                break;

            case 6:
                // Suzanne
                // 外部テクスチャを使用
                object =
                    std::make_unique<ModelObject>(
                        "suzanne",
                        textureManager,
                        textures[5]);
                break;
            }

            if (object) {

                // オブジェクトの初期化
                object->Create(
                    device,
                    commandList,
                    0xFFFFFFFF,
                    true);

                // 所有権を移動
                models.push_back(
                    std::move(object));

                // Transformを追加
                transforms.push_back({
                    {1, 1, 1},
                    {0, 0, 0},
                    {0, 0, 0}
                    });

                // ThroughWall登録
                if (isThroughWall) {

                    throughWallRenderer.AddObject(
                        models.back().get(),
                        0xFFFFFFFF,
                        Style::Solid);
                }

                currentObjectIndex =
                    static_cast<int>(models.size()) - 1;
            }
        }

        //-------------------------------------------------------------------------
        // Object List
        //-------------------------------------------------------------------------

        if (ImGui::BeginListBox("Objects")) {

            for (int i = 0;
                i < static_cast<int>(models.size());
                i++) {

                char label[32];

                sprintf_s(
                    label,
                    "Object %d",
                    i);

                bool selected =
                    currentObjectIndex == i;

                if (ImGui::Selectable(
                    label,
                    selected)) {

                    currentObjectIndex = i;
                }
            }

            ImGui::EndListBox();
        }

        //-------------------------------------------------------------------------
        // 選択中オブジェクト
        //-------------------------------------------------------------------------

        if (!models.empty()) {

            currentObjectIndex =
                std::clamp(
                    currentObjectIndex,
                    0,
                    static_cast<int>(models.size()) - 1);

            if (ImGui::CollapsingHeader(
                "Object",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                Transform &transform =
                    transforms[currentObjectIndex];

                ImGui::DragFloat3(
                    "Translate",
                    &transform.translate.x,
                    0.1f);

                ImGui::DragFloat3(
                    "Rotate",
                    &transform.rotate.x,
                    0.01f);

                ImGui::DragFloat3(
                    "Scale",
                    &transform.scale.x,
                    0.01f);

                // Delete
                if (models.size() > 1) {

                    if (ImGui::Button("Delete")) {

                        Object3D *deleteObject =
                            models[currentObjectIndex].get();

                        throughWallRenderer.RemoveObject(
                            deleteObject);

                        models.erase(
                            models.begin() + currentObjectIndex);

                        transforms.erase(
                            transforms.begin() + currentObjectIndex);

                        if (!models.empty()) {

                            currentObjectIndex =
                                (std::min)(
                                    currentObjectIndex,
                                    static_cast<int>(models.size()) - 1);
                        } else {

                            currentObjectIndex = 0;
                        }
                    }
                } else {

                    ImGui::BeginDisabled();
                    ImGui::Button("Delete");
                    ImGui::EndDisabled();
                }

                // Material
                if (ImGui::CollapsingHeader(
                    "Material",
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                    auto &object =
                        models[currentObjectIndex];

                    auto &material =
                        object->GetMaterial();

                    ImGui::ColorEdit4(
                        "Color",
                        &material.GetMaterialData()->color.x);

                    Transform &uv =
                        object->GetUVTransform();

                    ImGui::DragFloat2(
                        "UV-Translate",
                        &uv.translate.x,
                        0.01f);

                    ImGui::SliderAngle(
                        "UV-Rotate",
                        &uv.rotate.z);

                    ImGui::DragFloat2(
                        "UV-Scale",
                        &uv.scale.x,
                        0.01f);

                    // Lighting
                    Lighting::LightingType currentType =
                        lighting.GetLightType();

                    const char *lightTypeNames[] = {
                        "None",
                        "Lambert",
                        "Half-Lambert"
                    };

                    int currentItem =
                        static_cast<int>(currentType);

                    if (ImGui::Combo(
                        "Light Type",
                        &currentItem,
                        lightTypeNames,
                        IM_ARRAYSIZE(lightTypeNames))) {

                        lighting.SetLightType(
                            static_cast<Lighting::LightingType>(
                                currentItem));
                    }
                }
            }
        }

        //-------------------------------------------------------------------------
        // Light
        //-------------------------------------------------------------------------

        if (ImGui::CollapsingHeader(
            "Light",
            ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::ColorEdit4(
                "LightColor",
                &lighting.GetLightingData()->color.x);

            ImGui::SliderFloat3(
                "Direction",
                &lighting.GetLightingData()->direction.x,
                -1.0f,
                1.0f);

            ImGui::DragFloat(
                "Intensity",
                &lighting.GetLightingData()->intensity,
                0.05f,
                0.0f,
                10.0f);
        }

        lighting.Update();

        ImGui::Separator();

        //-------------------------------------------------------------------------
        // Sprite
        //-------------------------------------------------------------------------

        static std::vector<std::unique_ptr<Sprite>> sprites;
        static std::vector<int> spriteTextureIndices;

        if (ImGui::Button("Create Sprite")) {

            auto sprite =
                std::make_unique<Sprite>();

            sprite->Initialize(
                device,
                0.0f,
                0.0f,
                float(kClientWidth / 2),
                float(kClientHeight / 2),
                0xFFFFFFFF);

            sprites.push_back(
                std::move(sprite));

            spriteTextureIndices.push_back(0);
        }

        static int currentSpriteIndex = 0;

        if (ImGui::BeginListBox("Sprites")) {

            for (int i = 0;
                i < static_cast<int>(sprites.size());
                i++) {

                char label[32];

                sprintf_s(
                    label,
                    "Sprite %d",
                    i);

                if (ImGui::Selectable(
                    label,
                    currentSpriteIndex == i)) {

                    currentSpriteIndex = i;
                }
            }

            ImGui::EndListBox();
        }

        if (!sprites.empty()) {

            currentSpriteIndex =
                std::clamp(
                    currentSpriteIndex,
                    0,
                    static_cast<int>(sprites.size()) - 1);

            Sprite &sprite =
                *sprites[currentSpriteIndex];

            const char *textureItems[] = {
                "UVChecker",
                "Cube",
                "MonsterBall",
            };

            ImGui::Combo(
                "Texture",
                &spriteTextureIndices[currentSpriteIndex],
                textureItems,
                IM_ARRAYSIZE(textureItems));

            if (ImGui::CollapsingHeader(
                "Sprite",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                Transform &transform =
                    sprite.GetTransform();

                ImGui::DragFloat3(
                    "Translate Sprite",
                    &transform.translate.x,
                    0.1f);

                ImGui::DragFloat3(
                    "Rotate Sprite",
                    &transform.rotate.x,
                    0.01f);

                ImGui::DragFloat3(
                    "Scale Sprite",
                    &transform.scale.x,
                    0.01f);

                if (ImGui::Button("Delete Sprite")) {

                    sprites.erase(
                        sprites.begin() + currentSpriteIndex);

                    spriteTextureIndices.erase(
                        spriteTextureIndices.begin() + currentSpriteIndex);

                    if (!sprites.empty()) {

                        currentSpriteIndex =
                            (std::min)(
                                currentSpriteIndex,
                                static_cast<int>(sprites.size()) - 1);
                    } else {

                        currentSpriteIndex = 0;
                    }
                }

                if (ImGui::CollapsingHeader(
                    "Sprite Material",
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                    auto &material =
                        sprite.GetMaterial();

                    ImGui::ColorEdit4(
                        "Sprite Color",
                        &material.GetMaterialData()->color.x);

                    Transform &uv =
                        sprite.GetUVTransform();

                    ImGui::DragFloat2(
                        "Sprite UV-Translate",
                        &uv.translate.x,
                        0.01f);

                    ImGui::SliderAngle(
                        "Sprite UV-Rotate",
                        &uv.rotate.z);

                    ImGui::DragFloat2(
                        "Sprite UV-Scale",
                        &uv.scale.x,
                        0.01f);
                }
            }
        }

        ImGui::Separator();

        //-------------------------------------------------------------------------
        // ThroughWall
        //-------------------------------------------------------------------------

        ImGui::ColorEdit4(
            "ThroughWall-Object Color",
            &throughWallRenderer.GetObject(0).color.x);

        const char *styleNames[] = {
            "Solid",
            "Dot",
            "Stripe"
        };

        int style0 =
            static_cast<int>(
                throughWallRenderer.GetObject(0).style);

        if (ImGui::Combo(
            "ThroughWall Style",
            &style0,
            styleNames,
            IM_ARRAYSIZE(styleNames))) {

            throughWallRenderer.GetObject(0).style =
                static_cast<Style>(style0);
        }

        //-------------------------------------------------------------------------
        // Pipeline
        //-------------------------------------------------------------------------

        ImGui::SeparatorText("Pipeline");

        auto &pipelineConfig =
            engine->GetPipelineConfig(
                PipelineType::Object3dOpaque);

        const char *blendModeNames[] = {
            "Default",
            "Alpha",
            "Add",
            "Subtract",
            "Multiply",
            "Screen"
        };

        int blendMode =
            static_cast<int>(pipelineConfig.blend);

        if (ImGui::Combo(
            "Blend Mode",
            &blendMode,
            blendModeNames,
            IM_ARRAYSIZE(blendModeNames))) {

            pipelineConfig.blend =
                static_cast<BlendMode>(blendMode);

            engine->RebuildPipeline(
                PipelineType::Object3dOpaque);
        }

        ImGui::End();

        ImGuiManager::GetInstance()->EndFrame();

#endif

        //-------------------------------------------------------------------------
        // 描画処理
        //-------------------------------------------------------------------------

        engine->BeginFrame();

        // 3Dオブジェクト
        {
            engine->SetPipeline(
                PipelineType::Object3dOpaque);

            lighting.Bind(
                kLightRegisterIndex,
                commandList);

            for (size_t i = 0;
                i < models.size();
                ++i) {

                auto &object =
                    models[i];

                object->Update(
                    &camera,
                    transforms[i]);

                object->Draw(
                    commandList);
            }
        }

        // 壁越し3Dオブジェクト
        {
            engine->SetPipeline(
                PipelineType::Object3dThroughWall);

            throughWallRenderer.Draw(
                commandList);
        }

        // 2Dオブジェクト
        {
            engine->SetPipeline(
                PipelineType::Object2dOpaque);

            for (size_t i = 0;
                i < sprites.size();
                ++i) {

                sprites[i]->Update(
                    kClientWidth,
                    kClientHeight);

                sprites[i]->Draw(
                    commandList,
                    textures[spriteTextureIndices[i]]);
            }
        }

        engine->EndFrame();

        // ESC
        if (engine->GetKeyboard()->PushKey(DIK_ESCAPE)) {
            break;
        }
    }

    //-------------------------------------------------------------------------
    // 後処理
    //-------------------------------------------------------------------------

    engine->Finalize();

    return 0;
}