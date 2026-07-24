#include "KizunaEngine.h"
#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Object/Object3D.h"
#include "Renderer/Sprite.h"
#include "Renderer/ThroughWallRenderer.h"
#include "External/ImGuiManager.h"

#include "Object/SphereObject.h"
#include "Object/ModelObject.h"

#include <cstdint>

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
	// 球（壁越し描画の検証用ターゲット）
	SphereObject sphere[2]{
		SphereObject(kSubdivision), 
		SphereObject(kSubdivision)
	};
	sphere[0].Create(device, 0xFFFFFFFF, true);
	sphere[1].Create(device, 0xFFFFFFFF, true);

	// 天球（背景として最背面に描画されるドーム）
	ModelObject skydome("skydome");
	skydome.Create(device, 0xFFFFFFFF, false);

	// 箱（障害物。この箱の裏に球が隠れた際に「壁越し描画」を発生させるためのもの）
	ModelObject cube("cube");
	cube.Create(device, 0xFFFFFFFF, true);


	// 各リソース用Transform
	Transform transform[5]{};
	transform[0] = { .scale{ 0.5f, 0.5f, 0.5f }, .rotate{}, .translate{0, 0, 0} };	// sphere[0]
	transform[1] = { .scale{ 0.5f, 0.5f, 0.5f }, .rotate{}, .translate{0, 0, 1} };	// sphere[1]
	transform[2] = { .scale{ 1.0f, 1.0f, 1.0f }, .rotate{}, .translate{0, 0, 0} };	// skydome
	transform[3] = { .scale{ 1.0f, 1.0f, 1.0f }, .rotate{}, .translate{0, 0,-2} };	// cube

	// 壁越し描画の対象として2つの球を登録
	ThroughWallRenderer throughWallRenderer;
	throughWallRenderer.AddObject(&sphere[0], 0xFFFFFFFF, Style::Solid);
	throughWallRenderer.AddObject(&sphere[1], 0xFFFFFFFF, Style::Solid);

	// UI・2D表示用のスプライト
	Sprite sprite;
	sprite.Initialize(device, 0.0f, 0.0f, 640.0f, 360.0f, 0xFFFFFFFF);

	// テクスチャの読み込み
	TextureData uvTexture = engine->GetTextureManager()->LoadTexture(commandList, "Resources/uvChecker.png");
	TextureData cubeTexture = engine->GetTextureManager()->LoadTexture(commandList, "Resources/cube.jpg");

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
	engine->GetAudioManager()->Play("fanfare", false, 0.0f);

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

		// 各3Dモデルのワールド行列更新
		sphere[0].Update(&camera, transform[0]);
		sphere[1].Update(&camera, transform[1]);
		skydome.Update(&camera, transform[2]);
		cube.Update(&camera, transform[3]);

		// スプライト（UI）の画面サイズ追従更新
		sprite.Update(kClientWidth, kClientHeight);

		// デバッグ用メニュー（ImGui）のレンダリング制御
#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		// ImGuiのドッキングフラグを立てる
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");
		// デバッグカメラへの切り替え方法を解説
		ImGui::Text("Camera Mode : %s (Q:KeyBoad or A:GamePad to Toggle)", camera.GetStateName());
		ImGui::Separator();

		const char *styleNames[] =
		{
			"Solid",
			"Dot",
			"Stripe"
		};

		// Sphere0
		ImGui::DragFloat3("Sphere0 Position", &transform[0].translate.x, 0.1f);

		// 中央に配置された球のRotateを操作
		ImGui::SliderAngle("SphereRotateX", &transform[0].rotate.x);
		ImGui::SliderAngle("SphereRotateY", &transform[0].rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &transform[0].rotate.z);

		ImGui::ColorEdit4("Sphere0", &throughWallRenderer.GetObject(0).color.x);

		int style0 = static_cast<int>(throughWallRenderer.GetObject(0).style);
		if (ImGui::Combo("Sphere0 Style", &style0, styleNames, IM_ARRAYSIZE(styleNames))) {
			throughWallRenderer.GetObject(0).style = static_cast<Style>(style0);
		}

		ImGui::Separator();

		// Sphere1
		ImGui::DragFloat3("Sphere1 Position", &transform[1].translate.x, 0.1f);

		// 中央に配置された球のTranslateを操作
		ImGui::SliderAngle("SphereTranslateX", &transform[0].translate.x);
		ImGui::SliderAngle("SphereTranslateY", &transform[0].translate.y);
		ImGui::SliderAngle("SphereTranslateZ", &transform[0].translate.z);

		ImGui::ColorEdit4("Sphere1", &throughWallRenderer.GetObject(1).color.x);

		int style1 = static_cast<int>(throughWallRenderer.GetObject(1).style);
		if (ImGui::Combo("Sphere1 Style", &style1, styleNames, IM_ARRAYSIZE(styleNames))) {
			throughWallRenderer.GetObject(1).style = static_cast<Style>(style1);
		}

		ImGui::Separator();

		// スプライト（UI）の色や座標を変更
		ImGui::ColorEdit4("colorSprite", &sprite.GetMaterial().GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &sprite.GetTransform().translate.x, 0.0f, 500.0f);

		// スプライト（UI）のUVをTRSを操作
		ImGui::DragFloat2("UVTransform", &sprite.GetUVTransform().translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &sprite.GetUVTransform().scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &sprite.GetUVTransform().rotate.z);
		ImGui::Separator();

		// Lightingの種類をComboによって変更
		Lighting::LightingType currentType = lighting.GetLightType();
		const char *lightTypeNames[] = { "None", "Lambert", "Half-Lambert" };
		int currentItem = static_cast<int>(currentType);
		if (ImGui::Combo("Light Type", &currentItem, lightTypeNames, IM_ARRAYSIZE(lightTypeNames))) {
			lighting.SetLightType(static_cast<Lighting::LightingType>(currentItem));
		}

		// Lightの色/角度/発光量を操作
		ImGui::ColorEdit4("LightColor", &lighting.GetLightingData()->color.x);
		ImGui::SliderFloat3("LightDirection", &lighting.GetLightingData()->direction.x, -1.0f, 1.0f);
		lighting.Update();	// 角度操作によって起こるズレを即座に修正
		ImGui::DragFloat("Intensity", &lighting.GetLightingData()->intensity, 0.05f, 0.0f, 10.0f);
		ImGui::Separator();

		ImGui::End();
		ImGuiManager::GetInstance()->EndFrame();
#endif

		//===============
		// 描画処理
		//===============
		engine->BeginFrame();

		{// === 3D不透明オブジェクト描画（背景・遮蔽物） ===
			// 壁越し描画の判定基準を作るため、先に背景（天球）や遮蔽物（箱）を描画して深度バッファを確定させる
			engine->SetPipeline(PipelineType::Object3dOpaque);
			lighting.Bind(kLightRegisterIndex, commandList);

			skydome.Draw(commandList, uvTexture);
			cube.Draw(commandList, cubeTexture);
		}

		{// === 壁越し3Dオブジェクト描画パス ===
			// 遮蔽物の後ろにいると判断された部分に対して、
			// 独自のパイプラインを適用してレンダリングする
			engine->SetPipeline(PipelineType::Object3dThroughWall);

			throughWallRenderer.Draw(commandList);

		}

		{// === 対象オブジェクトの通常前面描画パス ===
			// 遮蔽物に隠れていない部分、または手前に露出している通常部分を上書き描画する
			engine->SetPipeline(PipelineType::Object3dOpaque);
			lighting.Bind(kLightRegisterIndex, commandList);

			sphere[0].Draw(commandList, uvTexture);
			sphere[1].Draw(commandList, uvTexture);
		}

		{// === 2Dオブジェクト（UI・HUDなど）描画パス ===
			// すべての3D表現の上に重ねる必要があるため、3Dの描画が完全に終わった後に実行する
			engine->SetPipeline(PipelineType::Object2dOpaque);
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