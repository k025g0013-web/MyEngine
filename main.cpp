#include "KizunaEngine.h"

#include "RenderCore/Camera.h"
#include "RenderCore/Lighting.h"

#include "Object/Object3D.h"

#include "Renderer/Sprite.h"
#include "Renderer/ThroughWallRenderer.h"

#include "External/ImGuiManager.h"

#include <cstdint>

// 球モデル分割数
const int32_t kSubdivision = 16;

// クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// 基盤初期化
	auto engine = std::make_unique<KizunaEngine>();
	engine->Initialize(L"CG2", kClientWidth, kClientHeight);

	ID3D12Device *device = engine->GetDevice();
	ID3D12GraphicsCommandList *commandList = engine->GetCommandList();

	// データ生成
	//===============
	// 球1
	Object3D model1;
	model1.CreateSphere(device, kSubdivision, 0xFFFFFFFF, true);
	Transform modelTransform{ {0.5f, 0.5f, 0.5f}, {}, {} };

	// 球2
	Object3D model2;
	model2.CreateSphere(device, kSubdivision, 0xFFFFFFFF, true);
	Transform modelTransform2{ {0.5f, 0.5f, 0.5f}, {}, {0,0,1} };

	// 天球
	Object3D skydomeModel;
	skydomeModel.CreateModel(device, "skydome", 0xFFFFFFFF, false);
	Transform skydomeTransform{ {1.0f, 1.0f, 1.0f}, {}, {} };

	// 箱
	Object3D cubeModel;
	cubeModel.CreateModel(device, "cube", 0xFFFFFFFF, false);
	Transform cubeTransform{ {1.0f, 1.0f, 1.0f}, {}, {0,0,-2} };

	// 壁越し描画対象の選別
	ThroughWallRenderer throughWallRenderer;
	throughWallRenderer.AddObject(&model1, 0xFFFF00FF);
	throughWallRenderer.AddObject(&model2, 0x0000FFFF);

	// スプライト
	Sprite sprite;
	sprite.Initialize(device, 0.0f, 0.0f, 640.0f, 360.0f, 0xFFFFFFFF);

	// テクスチャ
	TextureData uvTexture = engine->GetTextureManager()->LoadTexture(commandList, "Resources/uvChecker.png");

	// ライト/カメラ
	Lighting lighting;
	lighting.Initialize(device);

	Camera camera;
	camera.Initialize(float(kClientWidth), float(kClientHeight), engine->GetKeyboard(), engine->GetMouse(), engine->GetGamePad());
	Transform cameraTransform{ .scale{1.0f,1.0f,1.0f}, .rotate{0.3f,0.0f,0.0f}, .translate{0.0f,1.5f,-5.0f} };

	// オーディオ
	AudioData audioHandle = engine->GetAudioManager()->LoadAudio("Resources/fanfare.wav");
	AudioData audioHandleMusic = engine->GetAudioManager()->LoadAudio("Resources/music.mp3");

	// ファンファーレ再生
	engine->GetAudioManager()->PlayAudio(audioHandle, 0, 1.0f);

	//===============
	// メインループ
	//===============
	// ウィンドウの×ボタンが押されるまでループ
	while (engine->ProcessMessage()) {
		engine->UpdateInput();	// 入力デバイスの更新

		//===============
		// 更新処理
		//===============
		// カメラ切り替え
#ifdef _DEBUG
		if (engine->GetKeyboard()->TriggerKey(DIK_Q) ||
			engine->GetGamePad()->TriggerButton(XINPUT_GAMEPAD_A)) {
			camera.ToggleMode();
		}
#endif

		// カメラ
		camera.Update(cameraTransform);

		// モデル更新
		model1.Update(&camera, modelTransform);
		model2.Update(&camera, modelTransform2);
		skydomeModel.Update(&camera, skydomeTransform);
		cubeModel.Update(&camera, cubeTransform);

		// スプライト更新
		sprite.Update(kClientWidth, kClientHeight);

#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");
		if (camera.GetMode() == Camera::Mode::Normal) {
			ImGui::Text("Camera Mode: NORMAL (Q Key to Toggle)");
		} else {
			ImGui::Text("Camera Mode: DEBUG [WASD / Mouse] (Q Key to Toggle)");
		}
		ImGui::Separator();

		ImGui::SliderAngle("SphereRotateX", &modelTransform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &modelTransform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &modelTransform.rotate.z);
		ImGui::Separator();

		ImGui::ColorEdit4("colorSprite", &sprite.GetMaterial().GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &sprite.GetTransform().translate.x, 0.0f, 500.0f);

		ImGui::DragFloat2("UVTransform", &sprite.GetUVTransform().translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &sprite.GetUVTransform().scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &sprite.GetUVTransform().rotate.z);
		ImGui::Separator();

		Lighting::LightingType currentType = lighting.GetLightType();
		const char *lightTypeNames[] = { "None", "Lambert", "Half-Lambert" };
		int currentItem = static_cast<int>(currentType);

		if (ImGui::Combo("Light Type", &currentItem, lightTypeNames, IM_ARRAYSIZE(lightTypeNames))) {
			lighting.SetLightType(static_cast<Lighting::LightingType>(currentItem));
		}

		ImGui::ColorEdit4("LightColor", &lighting.GetLightingData()->color.x);
		ImGui::SliderFloat3("LightDirection", &lighting.GetLightingData()->direction.x, -1.0f, 1.0f);
		lighting.Update();
		ImGui::DragFloat("Intensity", &lighting.GetLightingData()->intensity, 0.05f, 0.0f, 10.0f);
		ImGui::Separator();
		ImGui::End();

		ImGuiManager::GetInstance()->EndFrame();
#endif

		//===============
		// 描画処理
		//===============
		engine->BeginFrame();

		{   // 3Dオブジェクト描画
			engine->SetPipeline(PipelineType::Object3dOpaque);

			lighting.Bind(3, commandList);

			skydomeModel.Draw(commandList, uvTexture);
			cubeModel.Draw(commandList, uvTexture);
		}

		{   // 壁越し3Dオブジェクト描画
			engine->SetPipeline(PipelineType::Object3dThroughWall);

			lighting.Bind(3, commandList);

			throughWallRenderer.Draw(commandList, uvTexture);

		}

		{	// プレイヤー通常描画
			engine->SetPipeline(PipelineType::Object3dOpaque);

			lighting.Bind(3, commandList);

			model1.Draw(commandList, uvTexture);
			model2.Draw(commandList, uvTexture);
		}

		{   // 2Dオブジェクト描画
			engine->SetPipeline(PipelineType::Object2dOpaque);
		//	sprite.Draw(commandList, uvTexture);
		}

		engine->EndFrame();

		// ESCキーで終了
		if (engine->GetKeyboard()->PushKey(DIK_ESCAPE)) {
			break;
		}
	}

	// 後処理
	//===============
	engine->Finalize();
	return 0;
}