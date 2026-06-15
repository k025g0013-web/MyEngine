#include "KizunaEngine.h"

// === Core ===
#include "Core/WinApp.h"
#include "Core/Logger.h"
#include "Core/DebugManager.h"

// === Graphics ===
#include "Graphics/DirectXCommon.h"

// Pipeline
#include "Graphics/Pipeline/PipelineManager.h"
#include "Graphics/Pipeline/RootSignature.h"
#include "Graphics/Pipeline/PipelineElements.h"
#include "Graphics/Pipeline/GraphicsPipeline.h"

// Resource
#include "Graphics/Resource/Shader.h"
#include "Graphics/Resource/Texture.h"

// === RenderCore ===
#include "RenderCore/Camera.h"
#include "RenderCore/Lighting.h"

// === Renderer ===
#include "Renderer/Sprite.h"

// === Audio ===
#include "Audio/Audio.h"

// === External ===
#include "External/ImGuiManager.h"

// === Object ===
#include "Object/Object3D.h"

// === Math ===
#include "Math/Functions.h"

// === Input ===
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

const int32_t kSubdivision = 16;

// クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	/*
	KizunaEngine* engine = KizunaEngine::GetInstance();
	engine->Initialize(L"CG2", kClientWidth, kClientHeight);

	WinApp *winApp = engine->GetWinApp();
	DirectXCommon *directXCommon = engine->GetDirectXCommon();
	PipelineManager *pipelineManager = engine->GetPipelineManager();
	Keyboard *keyboard = engine->GetKeyboard();
	Mouse *mouse = engine->GetMouse();

	HWND hwnd = winApp->GetHWND();
	ID3D12Device *device = directXCommon->GetDevice();
	ID3D12GraphicsCommandList *commandList = directXCommon->GetCommandList();
	*/

	DebugManager debugManager;

	// ログ
	Logger logger;
	logger.Initialize();

	// ウィンドウ生成
	WinApp winApp;
	winApp.Initialize(L"CG2", kClientWidth, kClientHeight);
	HWND hwnd = winApp.GetHWND();

	// デバッグレイヤーを有効化する
	debugManager.EnableDebugLayer();

	// DirectX初期化
	DirectXCommon directXCommon;
	directXCommon.Initialize(&winApp, &logger, kClientWidth, kClientHeight);
	ID3D12Device *device = directXCommon.GetDevice();
	ID3D12GraphicsCommandList *commandList = directXCommon.GetCommandList();


	// 入力マネージャ初期化
	Keyboard keyboard;
	keyboard.Initialize(&winApp);
	Mouse mouse;
	mouse.Initialize(hwnd);
	winApp.SetInputMouse(&mouse);

	// PSO
	PipelineManager pipelineManager;
	pipelineManager.Initialize(device, &logger);

	// サウンドマネージャ初期化
	Audio sound;
	sound.Initialize();

	// 球
	Object3D model;
	model.CreateSphere(device, kSubdivision, 0xFFFFFFFF, true);
	Transform modelTransform{ {1.0f, 1.0f, 1.0f}, {}, {}, };

	// 天球
	Object3D skydomeModel;
	skydomeModel.CreateModel(device, "Resources", "skydome.obj", 0xFFFFFFFF, false);
	Transform skydomeTransform{ {1.0f, 1.0f, 1.0f}, {}, {}, };

	// スプライト
	Sprite sprite;
	sprite.Initialize(device, 0.0f, 0.0f, 640.0f, 360.0f, 0xFFFFFFFF);

	// テクスチャ
	Texture texture;
	texture.Initialize(device, commandList, directXCommon.GetSRVHeap(), "Resources/uvChecker.png");

	// Lighting
	//===============
	Lighting lighting;
	lighting.Initialize(device);

	// ImGuiの初期化
	//===============
#ifdef USE_IMGUI
	ImGuiManager::GetInstance()->Initialize(
		hwnd,
		device,
		directXCommon.GetRenderOutput()->GetBufferCount(),
		directXCommon.GetRenderOutput()->GetRTVDesc().Format,
		directXCommon.GetSRVHeap()->GetDescriptorHeap(),
		directXCommon.GetSRVHeap()->GetCPUDescriptorHandle(0),
		directXCommon.GetSRVHeap()->GetGPUDescriptorHandle(0)
	);

	// ImGuiドッキング
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif

	//===============
	// ゲームループ内変数の初期化
	//===============
	Camera camera;
	camera.Initialize(float(kClientWidth), float(kClientHeight), &keyboard, &mouse);
	Transform cameraTransform{ .scale{1.0f,1.0f,1.0f}, .rotate{0.3f,0.0f,0.0f}, .translate{0.0f,1.5f,-5.0f} };

	// 音声読み込み/再生
	SoundData audioHandle = sound.LoadAudio("Resources/fanfare.wav");
	sound.PlayAudio(audioHandle, 0, 1.0f);

	//===============
	// メインループ
	//===============
	// ウィンドウの×ボタンが押されるまでループ
	while (winApp.ProcessMessage()) {
		keyboard.Update();
		mouse.Update();

		//===============
		// 更新処理
		//===============
		// カメラ切り替え
#ifdef _DEBUG
		if (keyboard.TriggerKey(DIK_Q)) {
			camera.ToggleMode();
		}
#endif

		// カメラ
		camera.Update(cameraTransform);

		// モデル更新
		model.Update(&camera, modelTransform);
		skydomeModel.Update(&camera, skydomeTransform);

		// スプライト更新
		sprite.Update(kClientWidth, kClientHeight);
		
#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");

		// カメラの種類表示
		if (camera.GetMode() == Camera::Mode::Normal) {
			ImGui::Text("Camera Mode: NORMAL (Q Key to Toggle)");
		} else {
			ImGui::Text("Camera Mode: DEBUG [WASD / Mouse] (Q Key to Toggle)");
		}
		ImGui::Separator();

		// モデル
		ImGui::SliderAngle("SphereRotateX", &modelTransform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &modelTransform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &modelTransform.rotate.z);
		ImGui::Separator();

		// Sprite
		ImGui::ColorEdit4("colorSprite", &sprite.GetMaterial().GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &sprite.GetTransform().translate.x, 0.0f, 500.0f);

		ImGui::ColorEdit4("LightColor", &lighting.GetLightingData()->color.x);
		ImGui::SliderFloat3("LightDirection", &lighting.GetLightingData()->direction.x, -1.0f, 1.0f);
		lighting.Update();
		ImGui::DragFloat("Intensity", &lighting.GetLightingData()->intensity, 0.05f, 0.0f, 10.0f);
		ImGui::Separator();

		// uvTransform
		ImGui::DragFloat2("UVTransform", &sprite.GetUVTransform().translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &sprite.GetUVTransform().scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &sprite.GetUVTransform().rotate.z);

		ImGui::End();

		// ImGuiの内部コマンドを生成する
		ImGuiManager::GetInstance()->EndFrame();
#endif
		
		//===============
		// 描画処理
		//===============
		directXCommon.BeginFrame();

		{	// Object3d描画
			const PipelineSet *opaque3D = pipelineManager.GetPipeline("Object3dOpaque");
			commandList->SetGraphicsRootSignature(opaque3D->rootSignature->GetRootSignature());
			commandList->SetPipelineState(opaque3D->pipeline->GetPipelineState());
			commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 形状を設定

			lighting.Bind(3, commandList);		// Lighting

			// モデル描画
			model.Draw(commandList, texture);
			skydomeModel.Draw(commandList, texture);
		}

		{	// Object2d描画
			const PipelineSet *opaque2D = pipelineManager.GetPipeline("Object2dOpaque");
			commandList->SetGraphicsRootSignature(opaque2D->rootSignature->GetRootSignature());
			commandList->SetPipelineState(opaque2D->pipeline->GetPipelineState());
			commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 形状を設定

			// スプライト描画
			sprite.Draw(commandList, texture);
		}

#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->Draw(commandList);
#endif
		directXCommon.EndFrame();

		mouse.EndFrame();

		// ESCキーで強制終了
		if (keyboard.PushKey(DIK_ESCAPE)) {
			return 0;
		}
	}

	// COMの終了
	//===============
	// WindowsAPI後始末
	winApp.Finalize();

#ifdef USE_IMGUI
	ImGuiManager::GetInstance()->Finalize();
#endif

	sound.Finalize();

	return 0;
}