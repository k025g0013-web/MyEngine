#include "KizunaEngine.h"
#include "Manager/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Object/Common/Object3D.h"
#include "External/ImGuiManager.h"

#include "Object/Primitive/TriangleObject.h"
#include "Object/Primitive/PlaneObject.h"
#include "Object/Primitive/SphereObject.h"

#include "Manager/ParticleManager.h"

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

	engine->Initialize(L"KizunaEngine", kClientWidth, kClientHeight);

	ID3D12Device *device = engine->GetDevice();

	ID3D12GraphicsCommandList *commandList = engine->GetCommandList();

	//-------------------------------------------------------------------------
	// データ生成・初期リソースのセットアップ
	//-------------------------------------------------------------------------
	PrimitiveManager *primitiveManager = engine->GetPrimitiveManager();

    ModelManager *modelManager = engine->GetModelManager();

    SpriteManager *spriteManager = engine->GetSpriteManager();
    assert(engine != nullptr);
    assert(spriteManager != nullptr);

	//-------------------------------------------------------------------------
	// テクスチャの読み込み
	//-------------------------------------------------------------------------
	TextureManager *textureManager =
		engine->GetTextureManager();

	// 外部テクスチャとして使用するもの
	TextureData textures[6];

	textures[0] = textureManager->LoadTexture(
		commandList, "resources/uvChecker.png");

	textures[1] = textureManager->LoadTexture(
		commandList, "resources/monsterBall.png");

	textures[2] = textureManager->LoadTexture(
		commandList, "resources/cube.jpg");

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

    ParticleManager particleManager;

    particleManager.Initialize(
        device,
        commandList,
        engine->GetDirectXCommon()->GetSRVHeap(),
        engine->GetTextureManager()
    );



    constexpr uint32_t kNumInstance = 10;

    Transform particleTransform{};
    particleTransform.scale = { 1.0f, 1.0f, 1.0f };
    particleTransform.rotate = { 0.0f, 0.0f, 0.0f };
    particleTransform.translate = { 0.0f, 0.0f, 0.0f };

    Vector3 particleVelocity = { 0.0f, 0.0f, 0.0f };

    // 基準となる位置から斜め方向にずらして生成する
    for (uint32_t index = 0; index < kNumInstance; ++index) {
        Transform transform = particleTransform;

        const float offset = static_cast<float>(index) * 0.1f;

        transform.translate.x += offset;
        transform.translate.y += offset;
        transform.translate.z += offset;

        particleManager.Emit(
            "resources/uvChecker.png",
            1,
            transform,
            particleVelocity,
            1000.0f
        );
    }



	//-------------------------------------------------------------------------
	// 初期オブジェクト
	//-------------------------------------------------------------------------
	// スプライト
	std::vector<std::unique_ptr<Sprite>> sprites;
	std::vector<int> spriteTextureIndices;

	// 初期モデル
    modelManager->CreateModel("fence");

	//-------------------------------------------------------------------------
	// ライト / カメラ
	//-------------------------------------------------------------------------

	Lighting lighting;
	lighting.Initialize(device);

	CameraManager camera;

	camera.Initialize(
		float(kClientWidth),
		float(kClientHeight),
        &engine->Input());

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

        //=========================================================================
        // 入力更新
        //=========================================================================

        engine->UpdateInput();

        //=========================================================================
        // カメラ更新
        //=========================================================================

#ifdef _DEBUG

        if (engine->Input().TriggerKey(DIK_Q) ||
            engine->Input().TriggerButton(XINPUT_GAMEPAD_A)) {
            camera.ToggleCamera();
        }

#endif

        camera.Update(cameraTransform);

        constexpr float kDeltaTime = 1.0f / 60.0f;
        particleManager.Update(kDeltaTime, &camera);

		//-------------------------------------------------------------------------
		// ImGui
		//-------------------------------------------------------------------------
#ifdef USE_IMGUI

        //=========================================================================
        // ImGui Begin
        //=========================================================================

        ImGuiManager::GetInstance()->BeginFrame();

        ImGui::DockSpaceOverViewport(
            ImGui::GetMainViewport()->ID,
            nullptr,
            ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::Begin("Setting");

        //=========================================================================
        // ImGui State
        //=========================================================================

        enum class SelectedObjectType {
            Model,
            Primitive
        };

        struct SelectedObject {
            SelectedObjectType type = SelectedObjectType::Model;
            int index = 0;
        };

        static SelectedObject selectedObject;

        static int createType = 0;
        static int currentSpriteIndex = 0;

        //=========================================================================
        // Camera
        //=========================================================================

        ImGui::SeparatorText("Camera");

        ImGui::Text(
            "Camera Mode : %s",
            camera.GetStateName());

        ImGui::Text(
            "Q : Keyboard / A : GamePad");

        //=========================================================================
        // Create Object
        //=========================================================================

        ImGui::SeparatorText("Create Object");

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
            "Type",
            &createType,
            createItems,
            IM_ARRAYSIZE(createItems));

        if (ImGui::Button("Create Object")) {

            switch (createType) {

                //=================================================================
                // Triangle
                //=================================================================

            case 0: {

                primitiveManager->CreateTriangle(
                    Vector3{ -0.5f, -0.5f, 0.0f },
                    Vector3{ 0.0f,  0.5f, 0.0f },
                    Vector3{ 0.5f, -0.5f, 0.0f },
                    textures[0]);

                // 作成したPrimitiveを選択
                selectedObject.type =
                    SelectedObjectType::Primitive;

                selectedObject.index =
                    static_cast<int>(
                        primitiveManager->GetObjectCount()) - 1;

                break;
            }

                  //=================================================================
                  // Plane
                  //=================================================================

            case 1: {

                modelManager->CreateModel("plane", textures[0]);

                // 作成したModelを選択
                selectedObject.type =
                    SelectedObjectType::Model;

                selectedObject.index =
                    static_cast<int>(
                        modelManager->GetModelCount()) - 1;

                break;
            }

                  //=================================================================
                  // Cube
                  //=================================================================

            case 2: {

                modelManager->CreateModel("cube", textures[2]);

                // 作成したModelを選択
                selectedObject.type =
                    SelectedObjectType::Model;

                selectedObject.index =
                    static_cast<int>(
                        modelManager->GetModelCount()) - 1;

                break;
            }

                  //=================================================================
                  // Sphere
                  //=================================================================

            case 3: {

                primitiveManager->CreateSphere(
                    kSubdivision,
                    textures[0]);

                // 作成したPrimitiveを選択
                selectedObject.type =
                    SelectedObjectType::Primitive;

                selectedObject.index =
                    static_cast<int>(
                        primitiveManager->GetObjectCount()) - 1;

                break;
            }

                  //=================================================================
                  // Teapot
                  //=================================================================

            case 4: {
                modelManager->CreateModel("teapot", textures[4]);

                // 作成したModelを選択
                selectedObject.type =
                    SelectedObjectType::Model;

                selectedObject.index =
                    static_cast<int>(
                        modelManager->GetModelCount()) - 1;

                break;
            }

                  //=================================================================
                  // Bunny
                  //=================================================================

            case 5: {

                modelManager->CreateModel("bunny", textures[0]);

                // 作成したModelを選択
                selectedObject.type =
                    SelectedObjectType::Model;

                selectedObject.index =
                    static_cast<int>(
                        modelManager->GetModelCount()) - 1;

                break;
            }

                  //=================================================================
                  // Suzanne
                  //=================================================================

            case 6: {
                modelManager->CreateModel("suzanne", textures[5]);

                // 作成したModelを選択
                selectedObject.type =
                    SelectedObjectType::Model;

                selectedObject.index =
                    static_cast<int>(
                        modelManager->GetModelCount()) - 1;
                break;
            }
            }
        }

        //=========================================================================
        // Objects
        //=========================================================================

        ImGui::SeparatorText("Objects");

        //-------------------------------------------------------------------------
        // Object List
        //-------------------------------------------------------------------------

        if (ImGui::BeginListBox(
            "Object List",
            ImVec2(-FLT_MIN, 150.0f))) {

            //=====================================================================
            // Models
            //=====================================================================

            for (int i = 0;
                i < static_cast<int>(
                    modelManager->GetModelCount());
                    ++i) {

                char label[64];

                sprintf_s(
                    label,
                    "Model %d",
                    i);

                bool selected =
                    selectedObject.type ==
                    SelectedObjectType::Model &&
                    selectedObject.index == i;

                if (ImGui::Selectable(
                    label,
                    selected)) {

                    selectedObject.type =
                        SelectedObjectType::Model;

                    selectedObject.index = i;
                }
            }

            //=====================================================================
            // Primitives
            //=====================================================================

            for (int i = 0;
                i < static_cast<int>(
                    primitiveManager->GetObjectCount());
                ++i) {

                char label[64];

                sprintf_s(
                    label,
                    "Primitive %d",
                    i);

                bool selected =
                    selectedObject.type ==
                    SelectedObjectType::Primitive &&
                    selectedObject.index == i;

                if (ImGui::Selectable(
                    label,
                    selected)) {

                    selectedObject.type =
                        SelectedObjectType::Primitive;

                    selectedObject.index = i;
                }
            }

            ImGui::EndListBox();
        }

        //=========================================================================
        // Selected Object
        //=========================================================================

        bool hasSelectedObject = false;

        //-------------------------------------------------------------------------
        // 選択状態の補正
        //-------------------------------------------------------------------------

        if (selectedObject.type ==
            SelectedObjectType::Model) {

            if (modelManager->GetModelCount() > 0) {

                selectedObject.index =
                    std::clamp(
                        selectedObject.index,
                        0,
                        static_cast<int>(
                            modelManager->GetModelCount()) - 1);

                hasSelectedObject = true;
            }

        } else {

            if (primitiveManager->GetObjectCount() > 0) {

                selectedObject.index =
                    std::clamp(
                        selectedObject.index,
                        0,
                        static_cast<int>(
                            primitiveManager->GetObjectCount()) - 1);

                hasSelectedObject = true;
            }
        }

        //-------------------------------------------------------------------------
        // Transform
        //-------------------------------------------------------------------------

        if (hasSelectedObject) {

            Transform *transform = nullptr;

            if (selectedObject.type ==
                SelectedObjectType::Model) {

                transform =
                    &modelManager->GetModels()
                    [selectedObject.index]
                    ->GetTransform();
            
            } else {

                transform =
                    &primitiveManager->GetTransforms()[
                        selectedObject.index];
            }

            if (ImGui::CollapsingHeader(
                "Transform",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                ImGui::DragFloat3(
                    "Translate",
                    &transform->translate.x,
                    0.1f);

                ImGui::DragFloat3(
                    "Rotate",
                    &transform->rotate.x,
                    0.01f);

                ImGui::DragFloat3(
                    "Scale",
                    &transform->scale.x,
                    0.01f);
            }

            //=====================================================================
            // Material
            //=====================================================================

            if (ImGui::CollapsingHeader(
                "Material",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                Material *material = nullptr;

                if (selectedObject.type ==
                    SelectedObjectType::Model) {

                    material =
                        &modelManager
                        ->GetModels()[selectedObject.index]
                        ->GetMaterial();

                } else {

                    material =
                        &primitiveManager
                        ->GetObjects()[selectedObject.index]
                        ->GetMaterial();
                }

                ImGui::ColorEdit4(
                    "Color",
                    &material
                    ->GetMaterialData()
                    ->color.x);
            }

            //=====================================================================
            // UV Transform
            //=====================================================================

            if (selectedObject.type ==
                SelectedObjectType::Model) {

                Model &model =
                    *modelManager->GetModels()[selectedObject.index];

                if (ImGui::CollapsingHeader(
                    "UV Transform",
                    ImGuiTreeNodeFlags_DefaultOpen)) {

                    Transform &uv =
                        model.GetUVTransform();

                    ImGui::DragFloat2(
                        "UV Translate",
                        &uv.translate.x,
                        0.01f);

                    ImGui::SliderAngle(
                        "UV Rotate",
                        &uv.rotate.z);

                    ImGui::DragFloat2(
                        "UV Scale",
                        &uv.scale.x,
                        0.01f);
                }
            }
            //=====================================================================
            // Lighting
            //=====================================================================

            if (ImGui::CollapsingHeader(
                "Lighting",
                ImGuiTreeNodeFlags_DefaultOpen)) {

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

            //=====================================================================
            // Delete
            //=====================================================================
            if (modelManager->GetModelCount() > 1) {

                if (ImGui::Button(
                    "Delete Model")) {

                    modelManager->DeleteModel(
                        selectedObject.index);

                    if (modelManager->GetModelCount() > 0) {

                        selectedObject.index =
                            (std::min)(
                                selectedObject.index,
                                static_cast<int>(
                                    modelManager->GetModelCount()) - 1);

                    } else {

                        selectedObject.index = 0;
                    }
                }

            } else {

                ImGui::BeginDisabled();

                ImGui::Button(
                    "Delete Model");

                ImGui::EndDisabled();
            }
        }

        //=========================================================================
        // Light
        //=========================================================================

        ImGui::SeparatorText("Light");

        if (ImGui::CollapsingHeader(
            "Directional Light",
            ImGuiTreeNodeFlags_DefaultOpen)) {

            ImGui::ColorEdit4(
                "Light Color",
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

        //=========================================================================
        // Sprites
        //=========================================================================

        ImGui::SeparatorText("Sprites");

        //-------------------------------------------------------------------------
        // Create Sprite
        //-------------------------------------------------------------------------

        if (ImGui::Button("Create Sprite")) {

            spriteManager->CreateSprite(
                0.0f, 0.0f, float(kClientWidth / 2), float(kClientHeight / 2), textures[0], 0);

            currentSpriteIndex =
                static_cast<int>(
                    spriteManager->GetSpriteCount()) - 1;
        }

        //-------------------------------------------------------------------------
        // Sprite List
        //-------------------------------------------------------------------------

        if (ImGui::BeginListBox(
            "Sprite List",
            ImVec2(-FLT_MIN, 120.0f))) {

            for (int i = 0; i < static_cast<int>(spriteManager->GetSpriteCount()); ++i) {
                char label[64];

                sprintf_s(label, "Sprite %d", i);

                if (ImGui::Selectable(label, currentSpriteIndex == i)) {
                    currentSpriteIndex = i;
                }
            }

            ImGui::EndListBox();
        }

        //-------------------------------------------------------------------------
        // Selected Sprite
        //-------------------------------------------------------------------------

        if (spriteManager->GetSpriteCount() > 0) {

            currentSpriteIndex = std::clamp(
                currentSpriteIndex,
                0,
                static_cast<int>(
                    spriteManager->GetSpriteCount()) - 1);

            Sprite &sprite =
                *spriteManager->GetSprite(
                    currentSpriteIndex);

            // Texture
            const char *textureItems[] = {
                "UVChecker",
                "Cube",
                "MonsterBall",
            };

            // 選択肢の順番から実際のテクスチャ配列のインデックスへの対応
            const int textureMap[] = {
                0, // UVChecker
                2, // Cube
                1, // MonsterBall
            };

            int textureIndex =
                spriteManager->GetTextureIndex(
                    currentSpriteIndex);

            if (ImGui::Combo(
                "Texture",
                &textureIndex,
                textureItems,
                IM_ARRAYSIZE(textureItems))) {

                spriteManager->SetTexture(
                    currentSpriteIndex,
                    textures[textureMap[textureIndex]],
                    textureIndex);
            }

            // Transform
            if (ImGui::CollapsingHeader(
                "Sprite Transform",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                Transform &transform =
                    sprite.GetTransform();

                ImGui::DragFloat3(
                    "Translate##Sprite",
                    &transform.translate.x,
                    0.1f);

                ImGui::DragFloat3(
                    "Rotate##Sprite",
                    &transform.rotate.x,
                    0.01f);

                ImGui::DragFloat3(
                    "Scale##Sprite",
                    &transform.scale.x,
                    0.01f);
            }

            // Material
            if (ImGui::CollapsingHeader(
                "Sprite Material",
                ImGuiTreeNodeFlags_DefaultOpen)) {

                Material &material =
                    sprite.GetMaterial();

                ImGui::ColorEdit4(
                    "Sprite Color",
                    &material.GetMaterialData()->color.x);

                Transform &uv = sprite.GetUVTransform();

                ImGui::DragFloat2(
                    "UV Translate##Sprite",
                    &uv.translate.x,
                    0.01f);

                ImGui::SliderAngle(
                    "UV Rotate##Sprite",
                    &uv.rotate.z);

                ImGui::DragFloat2(
                    "UV Scale##Sprite",
                    &uv.scale.x,
                    0.01f);
            }

            // Delete
            ImGui::Separator();

            if (ImGui::Button("Delete Sprite")) {

                spriteManager->DeleteSprite(
                    currentSpriteIndex);

                if (spriteManager->GetSpriteCount() > 0) {
                    currentSpriteIndex = (std::min)(
                        currentSpriteIndex,
                        static_cast<int>(
                            spriteManager->GetSpriteCount()) - 1);
                } else {
                    currentSpriteIndex = 0;
                }
            }
        }

        //=========================================================================
        // Pipeline
        //=========================================================================

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
            static_cast<int>(
                pipelineConfig.blend);

        if (ImGui::Combo(
            "Blend Mode",
            &blendMode,
            blendModeNames,
            IM_ARRAYSIZE(blendModeNames))) {

            pipelineConfig.blend =
                static_cast<BlendMode>(
                    blendMode);

            engine->RebuildPipeline(
                PipelineType::Object3dOpaque);
        }

        //=========================================================================
        // ImGui End
        //=========================================================================

        ImGui::End();

        ImGuiManager::GetInstance()->EndFrame();

#endif
        lighting.Update();

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

			// プリミティブ
			primitiveManager->Update(
				&camera);

			primitiveManager->Draw(
				commandList);


			// モデル
            modelManager->Update(&camera);
            modelManager->Draw(commandList);
		}

        // パーティクル（板ポリ10枚）
        {
            engine->SetPipeline(PipelineType::Particle);

            lighting.Bind(3, commandList);

            particleManager.Draw(commandList);
        }

		// 2Dオブジェクト
		{
            engine->SetPipeline(PipelineType::Object2dOpaque);

            spriteManager->Update(
                kClientWidth,
                kClientHeight);

            spriteManager->Draw(commandList);
		}

		engine->EndFrame();

		// ESC
        if (engine->Input().PushKey(DIK_ESCAPE)) {
            break;
        }
	}

	//-------------------------------------------------------------------------
	// 後処理
	//-------------------------------------------------------------------------

	engine->Finalize();

	return 0;
}