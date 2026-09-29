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

// Windowsアプリでのエントリーポイント(main関数)
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
	// 壁越し描画の対象
	ThroughWallRenderer throughWallRenderer;
	static bool isThroughWall = true;

	// 複数モデルのリソース初期化
	std::vector<std::unique_ptr<Object3D>> models;
	std::vector<Transform> transforms;
	std::vector<int> objectTextureIndices;

	// 複数スプライトのリソース初期化
	std::vector<std::unique_ptr<Sprite>> sprites;
	std::vector<int> spriteTextureIndices;

	{	/// 初期オブジェクト（Sphere）
		auto object = std::make_unique<ModelObject>("fence");
		object->Create(device, 0xFFFFFFFF, true);

		models.push_back(std::move(object));

		transforms.push_back({
			.scale		{1.0f, 1.0f, 1.0f},
			.rotate		{0.0f, 0.0f, 0.0f},
			.translate	{0.0f, 0.0f, 0.0f},
			});

		objectTextureIndices.push_back(0);

		throughWallRenderer.AddObject(models.back().get(), 0x00000000, Style::Solid);
	}

	// テクスチャの読み込み
	TextureData textures[6];
	textures[0] = engine->GetTextureManager()->LoadTexture(commandList, "resources/Models/fence/fence.png");
	textures[1] = engine->GetTextureManager()->LoadTexture(commandList, "resources/monsterBall.png");
	textures[2] = engine->GetTextureManager()->LoadTexture(commandList, "resources/cube.jpg");
	textures[3] = engine->GetTextureManager()->LoadTexture(commandList, "resources/axis.jpg");
	textures[4] = engine->GetTextureManager()->LoadTexture(commandList, "resources/checkerBoard.png");
	textures[5] = engine->GetTextureManager()->LoadTexture(commandList, "resources/white1x1.png");

	// ライト/カメラの初期化
	Lighting lighting;
	lighting.Initialize(device);

	CameraManager camera;
	camera.Initialize(float(kClientWidth), float(kClientHeight), engine->GetKeyboard(), engine->GetMouse(), engine->GetGamePad());
	Transform cameraTransform = { .scale{1.0f,1.0f,1.0f}, .rotate{0.3f,0.0f,0.0f}, .translate{0.0f,1.5f,-5.0f} };

	// オーディオ
	engine->GetAudioManager()->Load("fanfare", "Resources/fanfare.wav");
	engine->GetAudioManager()->Load("bgm_music", "Resources/music.mp3");

	// 起動時のファンファーレを一度だけ再生
	engine->GetAudioManager()->Play("fanfare", false, 1.0f);

	//-------------------------------------------------------------------------
	// メインループ
	//-------------------------------------------------------------------------
	// OSからの終了メッセージを受け取るまでループ
	while (engine->ProcessMessage()) {
		engine->UpdateInput();	// 入力デバイス（キーボード/マウス/PAD）の最新状態を取得

		//===============
		// 更新処理
		//===============
		// デバッグビルド時のみ、QキーまたはゲームパッドのAボタンで通常カメラ/デバッグカメラを切り替え可能にする
#ifdef _DEBUG
		if (engine->GetKeyboard()->TriggerKey(DIK_Q) ||
			engine->GetGamePad()->TriggerButton(XINPUT_GAMEPAD_A)) {
			camera.ToggleCamera();
		}
#endif

		// カメラの行列計算
		camera.Update(cameraTransform);

		// デバッグ用メニュー（ImGui）のレンダリング制御
#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		// ImGuiのドッキングフラグを立てる
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");

		ImGui::Text(
			"Camera Mode : %s (Q:KeyBoad or A:GamePad to Toggle)", camera.GetStateName());
		ImGui::Separator();

		static int currentObjectIndex = 0;
		static int createType = 0;
		
		const char *createItems[] = {
			"Triangle", "Plane", "Cube", "Sphere",
			"Teapot", "Bunny", "Suzanne",
		};

		// 作成するモデル種類
		ImGui::Combo("Model", &createType,
			createItems, IM_ARRAYSIZE(createItems));

		ImGui::Checkbox("Enable Through-Wall", &isThroughWall);

		// Create
		if (ImGui::Button("Create Model")) {
			std::unique_ptr<Object3D> object;
			int defaultTexIndex = 0;


			switch(createType) {
			case 0:	// Triangle
				object = std::make_unique<TriangleObject>(
					Vector3{ -0.5f,-0.5f,0.0f },
					Vector3{ 0.0f, 0.5f,0.0f },
					Vector3{ 0.5f,-0.5f,0.0f });
				defaultTexIndex = 0;
				break;

			case 1:	// Plane
				object = std::make_unique<ModelObject>("plane");
				defaultTexIndex = 0;
				break;

			case 2:	// Cube
				object = std::make_unique<ModelObject>("cube");
				defaultTexIndex = 0;
				break;

			case 3:	// Sphere
				object = std::make_unique<SphereObject>(kSubdivision);
				defaultTexIndex = 0;
				break;

			case 4: // Teapot
				object = std::make_unique<ModelObject>("teapot");
				defaultTexIndex = 4;
				break;

			case 5: // Bunny
				object = std::make_unique<ModelObject>("bunny");
				defaultTexIndex = 0;
				break;

			case 6: // Suzanne
				object = std::make_unique<ModelObject>("suzanne");
				defaultTexIndex = 5;
				break;
			}

			if (object) {
				// オブジェクトの初期化
				object->Create(device, 0xFFFFFFFF, true);

				// 1. 配列に所有権を移動
				models.push_back(std::move(object));

				// 2. Transformとテクスチャインデックスを追加
				transforms.push_back({ {1,1,1}, {0,0,0}, {0,0,0} });
				objectTextureIndices.push_back(defaultTexIndex);

				// 3. チェックボックスがONの場合のみ ThroughWallRenderer に登録
				if (isThroughWall) {
					throughWallRenderer.AddObject(models.back().get(), 0xFFFFFFFF, Style::Solid);
				}

				currentObjectIndex = static_cast<int>(models.size()) - 1;
			}
		}

		// Object
		if (ImGui::BeginListBox("Objects")) {
			for (int i = 0; i < static_cast<int>(models.size()); i++) {
				char label[32];
				sprintf_s(label, "Object %d", i);

				bool selected =
					(currentObjectIndex == i);

				if (ImGui::Selectable(label, selected)) {
					currentObjectIndex = i;
				}
			}

			ImGui::EndListBox();
		}

		// 選択中オブジェクト
		//
		if (!models.empty()) {

			currentObjectIndex = std::clamp(currentObjectIndex, 0, static_cast<int>(models.size()) - 1);

			// Object
			if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_DefaultOpen)) {
				Transform &transform = transforms[currentObjectIndex];

				// オブジェクト本体のTransformの調整
				ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
				ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);

				// Delete
				if (models.size() > 1) {
					if (ImGui::Button("Delete")) {

						// 削除するオブジェクト
						Object3D *deleteObject = models[currentObjectIndex].get();

						// ThroughWallRendererに登録されている場合は先に削除
						throughWallRenderer.RemoveObject(deleteObject);

						// 通常の配列から削除
						models.erase(models.begin() + currentObjectIndex);
						transforms.erase(transforms.begin() + currentObjectIndex);
						objectTextureIndices.erase(objectTextureIndices.begin() + currentObjectIndex);

						if (!models.empty()) {

							currentObjectIndex =
								(std::min)(
									currentObjectIndex,
									static_cast<int>(models.size()) - 1
									);

						} else {

							currentObjectIndex = 0;
						}
					}
				} else {
					ImGui::BeginDisabled();
					ImGui::Button("Delete");
					ImGui::EndDisabled();
				}

				if (ImGui::CollapsingHeader(
					"Material",
					ImGuiTreeNodeFlags_DefaultOpen)) {
					auto &object = models[currentObjectIndex];
					auto &material = object->GetMaterial();

					// 色
					ImGui::ColorEdit4("Color", &material.GetMaterialData()->color.x);

					// UV
					Transform &uv = object->GetUVTransform();
					ImGui::DragFloat2("UV-Translate", &uv.translate.x, 0.01f);
					ImGui::SliderAngle("UV-Rotate", &uv.rotate.z);
					ImGui::DragFloat2("UV-Scale", &uv.scale.x, 0.01f);

					// Lightingの種類をComboによって変更
					Lighting::LightingType currentType = lighting.GetLightType();
					const char *lightTypeNames[] = { "None", "Lambert", "Half-Lambert" };
					int currentItem = static_cast<int>(currentType);
					if (ImGui::Combo("Light Type", &currentItem, lightTypeNames, IM_ARRAYSIZE(lightTypeNames))) {
						lighting.SetLightType(static_cast<Lighting::LightingType>(currentItem));
					}
				}
			}
		}

		// Light
		if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::ColorEdit4("LightColor", &lighting.GetLightingData()->color.x);
			ImGui::SliderFloat3("Direction", &lighting.GetLightingData()->direction.x, -1.0f, 1.0f);
			ImGui::DragFloat("Intensity", &lighting.GetLightingData()->intensity, 0.05f, 0.0f, 10.0f);
		}
		lighting.Update();


		ImGui::Separator();


		// Create Sprite
		if (ImGui::Button("Create Sprite")) {
			auto sprite = std::make_unique<Sprite>();

			sprite->Initialize(device, 0.0f, 0.0f,
				float(kClientWidth / 2), float(kClientHeight / 2), 0xFFFFFFFF);

			sprites.push_back(std::move(sprite));
			spriteTextureIndices.push_back(0);
		}

		static int currentSpriteIndex = 0;
		if (ImGui::BeginListBox("Sprites")) {
			for (int i = 0; i < sprites.size(); i++) {
				char label[32];
				sprintf_s(label, "Sprite %d", i);

				if (ImGui::Selectable(label, currentSpriteIndex == i)) {
					currentSpriteIndex = i;
				}
			}

			ImGui::EndListBox();
		}

		if (!sprites.empty()) {
			currentSpriteIndex = std::clamp(
				currentSpriteIndex, 0, (int)sprites.size() - 1);

			Sprite &sprite = *sprites[currentSpriteIndex];

			const char *textureItems[] = {
				"UVChecker", "Cube", "MonsterBall",
			};

			ImGui::Combo("Texture", &spriteTextureIndices[currentSpriteIndex],
				textureItems, IM_ARRAYSIZE(textureItems));

			if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
				Transform &transform = sprite.GetTransform();

				ImGui::DragFloat3("Translate Sprite", &transform.translate.x, 0.1f);
				ImGui::DragFloat3("Rotate Sprite", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Scale Sprite", &transform.scale.x, 0.01f);

				// Delete
				if (ImGui::Button("Delete Sprite")) {
					sprites.erase(sprites.begin() + currentSpriteIndex);
					spriteTextureIndices.erase(spriteTextureIndices.begin() + currentSpriteIndex);

					if (!sprites.empty()) {
						currentSpriteIndex = (std::min)(
							currentSpriteIndex, static_cast<int>(sprites.size()) - 1);
					} else {
						currentSpriteIndex = 0;
					}
				}

				if (ImGui::CollapsingHeader("Sprite Material", ImGuiTreeNodeFlags_DefaultOpen)) {
					auto &material = sprite.GetMaterial();

					ImGui::ColorEdit4("Sprite Color", &material.GetMaterialData()->color.x);

					Transform &uv = sprite.GetUVTransform();
					ImGui::DragFloat2("Sprite UV-Translate", &uv.translate.x, 0.01f);
					ImGui::SliderAngle("Sprite UV-Rotate", &uv.rotate.z);
					ImGui::DragFloat2("Sprite UV-Scale", &uv.scale.x, 0.01f);
				}
			}
		}


		ImGui::Separator();

		ImGui::ColorEdit4("ThroughWall-Object Color", &throughWallRenderer.GetObject(0).color.x);

		const char *styleNames[] = { "Solid", "Dot", "Stripe" };
		int style0 = static_cast<int>(throughWallRenderer.GetObject(0).style);
		if (ImGui::Combo("ThroughWall Style", &style0, styleNames, IM_ARRAYSIZE(styleNames))) {
			throughWallRenderer.GetObject(0).style = static_cast<Style>(style0);
		}

		//====================
		// Pipeline
		//====================

		ImGui::SeparatorText("Pipeline");

		auto &pipelineConfig =
			engine->GetPipelineConfig(
				PipelineType::Object3dOpaque
			);

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
			IM_ARRAYSIZE(blendModeNames)
		)) {
			pipelineConfig.blend =
				static_cast<BlendMode>(blendMode);

			engine->RebuildPipeline(
				PipelineType::Object3dOpaque
			);
		}

		ImGui::End();
		ImGuiManager::GetInstance()->EndFrame();
#endif

		//===============
		// 描画処理
		//===============
		engine->BeginFrame();

		{// === 3Dオブジェクト描画 ===
		// 背景や遮蔽物を先に描画し、深度バッファを構築する
			engine->SetPipeline(PipelineType::Object3dOpaque);
			lighting.Bind(kLightRegisterIndex, commandList);

			for (size_t i = 0; i < models.size(); ++i) {
				auto &object = models[i];

				// オブジェクトの状態を更新
				object->Update(&camera, transforms[i]);

				int texIndex = objectTextureIndices[i];
				object->Draw(commandList, textures[texIndex]);
			}
		}

		{// === 壁越し3Dオブジェクト描画 ===
			// 深度バッファを利用して、遮蔽物の後ろにあるオブジェクトを描画する
			engine->SetPipeline(PipelineType::Object3dThroughWall);

			throughWallRenderer.Draw(commandList);
		}

		{// === 2Dオブジェクト描画 ===
			// 3Dオブジェクトの上に重ねてUIやHUDを描画する
			engine->SetPipeline(PipelineType::Object2dOpaque);

			for (size_t i = 0; i < sprites.size(); ++i) {
				sprites[i]->Update(kClientWidth, kClientHeight);
				sprites[i]->Draw(
					commandList,
					textures[spriteTextureIndices[i]]
				);
			}
		}

		engine->EndFrame();

		// ESCキーでゲームを安全に終了させるための入力検知
		if (engine->GetKeyboard()->PushKey(DIK_ESCAPE)) {
			break;
		}
	}

	//-------------------------------------------------------------------------
	// 後処理
	//-------------------------------------------------------------------------
	// DirectXの解放やウィンドウの破棄など、エンジンの終了処理を実行
	engine->Finalize();
	return 0;
}