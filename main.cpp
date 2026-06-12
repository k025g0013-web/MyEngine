#pragma region include

#include <windows.h>
#include <wrl.h>

#include <cassert>

#include <algorithm>

#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

// Core
#include "CrashHandler.h"
#include "WinApp.h"
#include "Logger.h"
#include "DirectXCommon.h"

// Pipeline
#include "GraphicsPipeline.h"
#include "RootSignature.h"
#include "InputLayout.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"

// Resource
#include "Shader.h"
#include "Lighting.h"
#include "Texture.h"
#include "Sound.h"

// ImGui
#include "ImGuiManager.h"

// Object
#include "Object3D.h"
#include "Sprite.h"
#include "Camera.h"
#include "DebugCamera.h"

// Math
#include "MathFunctions.h"

// Input
#include "InputKey.h"
#include "InputMouse.h"

#pragma endregion

struct D3DResourceLeakChecker {
#ifdef _DEBUG
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
#endif
};

const int32_t kSubdivision = 16;

// クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	D3DResourceLeakChecker leakChecker;

	// WindowAPI初期化
#pragma region WindowAPI初期化

	// COMの初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	// CrashHandler
	CrashHandler::Initialize();

	// ログ
	Logger logger;
	logger.Initialize();

	// ウィンドウ生成
	WinApp winApp;
	winApp.Initialize(L"CG2", kClientWidth, kClientHeight);
	HWND hwnd = winApp.GetHWND();

#ifdef _DEBUG
	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma endregion

	// DirectX初期化
	DirectXCommon directXCommon;
	directXCommon.Initialize(&winApp, &logger, kClientWidth, kClientHeight);
	ID3D12Device *device = directXCommon.GetDevice();
	ID3D12GraphicsCommandList *commandList = directXCommon.GetCommandList();

	// サウンドマネージャ初期化
	Sound sound;
	sound.Initialize();

	// 入力マネージャ初期化
	InputKey keyboard;
	keyboard.Initialize(&winApp);
	InputMouse mouse;
	mouse.Initialize(hwnd);
	winApp.SetInputMouse(&mouse);

	// PSO
#pragma region PSO

	// rootSignature
	RootSignature rootSignature;
	rootSignature.Initialize(device, &logger);

	// InputLayout
	InputLayout inputLayout;
	inputLayout.Initialize();

	// BlendState
	BlendState blendState;
	blendState.Initialize();

	// RasterizerState
	RasterizerState rasterizerState;
	rasterizerState.Initialize();

	// VertexShader
	Shader vertexShader = directXCommon.GetCompileShader()->Compile(L"Object3D.VS.hlsl", L"vs_6_0");

	// PixelShader
	Shader pixelShader = directXCommon.GetCompileShader()->Compile(L"Object3D.PS.hlsl", L"ps_6_0");

	// DepthStencilState
	DepthStencilState depthStencilState;
	depthStencilState.Initialize();

	// PSO生成
	GraphicsPipeline graphicsPipeline;
	graphicsPipeline.Initialize(device,
		rootSignature, inputLayout,
		blendState, rasterizerState,
		depthStencilState,
		vertexShader, pixelShader
	);

#pragma endregion

	/*
	// モデル
	Object3D model;
	model.CreateModel(device, "resourcrs", "axis.obj", 0xFFFFFFFF, true);
	Transform modelTransform{
		.scale{1.0f, 1.0f, 1.0f}, .rotate{}, .translate{}
	};

	// 平面三角形
	Object3D triangle;
	triangle.CreatePlaneTriangle(device,
		{-0.5f, -0.5f}, {0.0f, 0.5f}, {0.5f, -0.5f}, 0xFFFFFFFF, true);
	Transform triangleTransform{
		.scale{1.0f, 1.0f, 1.0f}, .rotate{}, .translate{}
	};

	// 球
	Object3D sphere;
	sphere.CreateSphere(device, kSubdivision, 0xFFFFFFFF, true);
	Transform sphereTransform{
		.scale{1.0f, 1.0f, 1.0f}, .rotate{}, .translate{}
	};
	*/

	std::vector<Object3D> models;
	std::vector<Transform> transforms;
	std::vector<int> textureIndices;

	// モデル用テクスチャ
	Texture texture[4];
	texture[0].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/uvChecker.png");
	texture[1].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/monsterBall.png");
	texture[2].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/sample.png");
	texture[3].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/axis.jpg");

	Object3D object3D;
	object3D.CreateSphere(
		device,
		kSubdivision,
		0xFFFFFFFF,
		true
	);
	models.push_back(std::move(object3D));
	transforms.push_back({
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
		});

	textureIndices.push_back(0);

	// スプライト
	Sprite sprite;
	sprite.Initialize(device, 0.0f, 0.0f, 640.0f, 360.0f, 0xFFFFFFFF);

	// スプライト用テクスチャ
	Texture spriteTexture;
	spriteTexture.Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/uvChecker.png");

	// 天球
	Texture skydomeTexture;
	skydomeTexture.Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/uvChecker.png");
	Object3D skydomeModel;
	skydomeModel.CreateModel(device, "resources", "skydome.obj", 0xFFFFFFFF, false);
	Transform skydomeTransform{};

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
		directXCommon.GetSwapChain()->GetBufferCount(),
		directXCommon.GetRenderTargetView()->GetRTVDesc().Format,
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
	camera.Initialize(float(kClientWidth), float(kClientHeight));
	Transform cameraTransform{ .scale{1.0f,1.0f,1.0f}, .rotate{0.3f,0.0f,0.0f}, .translate{0.0f,2.0f,-5.0f} };

	DebugCamera debugCamera;
	debugCamera.Initialize(&keyboard, &mouse);

	// int currentTextureIndex = 0;
	const char *textureItems[] = {
		"resources/uvChecker.png",
		"resources/monsterBall.png",
		"resources/sample.png",
		"resources/axis.jpg",
	};

	SoundData audioHandle = sound.SoundLoadWave("resources/fanfare.wav");
	sound.SoundPlayWave(audioHandle);
	

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
		// カメラ
		camera.Update(cameraTransform);
		debugCamera.Update();

		// モデル更新
		for (size_t i = 0; i < models.size(); i++) {
			models[i].Update(&debugCamera, transforms[i]);
		}

		skydomeModel.Update(&debugCamera, skydomeTransform);

		/*
		// スプライト更新
		sprite.Update(kClientWidth, kClientHeight);
		*/

#ifdef USE_IMGUI

		ImGuiManager::GetInstance()->BeginFrame();

		ImGui::DockSpaceOverViewport(
			ImGui::GetMainViewport()->ID,
			nullptr,
			ImGuiDockNodeFlags_PassthruCentralNode
		);

		ImGui::Begin("Settings");
		
		static int currentObjectIndex = 0;
		static int createType = 0;

		const char *createItems[] = {
			"PlaneTriangle",
			"Sphere",
			"Model"
		};

		//
		// 作成するモデル種類
		//
		ImGui::Combo(
			"Model",
			&createType,
			createItems,
			IM_ARRAYSIZE(createItems)
		);

		//
		// Create
		//
		if (ImGui::Button("Create")) {

			Object3D object;

			switch (createType) {

			case 0:
				object.CreatePlaneTriangle(
					device,
					{ -0.5f,-0.5f },
					{ 0.0f, 0.5f },
					{ 0.5f,-0.5f },
					0xFFFFFFFF,
					true
				);
				break;

			case 1:
				object.CreateSphere(
					device,
					kSubdivision,
					0xFFFFFFFF,
					true
				);
				break;

			case 2:
				object.CreateModel(
					device,
					"resources",
					"axis.obj",
					0xFFFFFFFF,
					true
				);
				break;
			}

			models.push_back(std::move(object));

			transforms.push_back({
				{1.0f,1.0f,1.0f},
				{0.0f,0.0f,0.0f},
				{0.0f,0.0f,0.0f}
				});

			textureIndices.push_back(0);

			currentObjectIndex =
				static_cast<int>(models.size()) - 1;
		}

		//
		// Object一覧
		//
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

		//
		// 選択中オブジェクト
		//
		if (!models.empty()) {

			currentObjectIndex = std::clamp(
				currentObjectIndex,
				0,
				static_cast<int>(models.size()) - 1
			);

			//
			// Object
			//
			if (ImGui::CollapsingHeader(
				"Object",
				ImGuiTreeNodeFlags_DefaultOpen
			)) {

				Transform &transform =
					transforms[currentObjectIndex];

				ImGui::DragFloat3(
					"Translate",
					&transform.translate.x,
					0.1f
				);

				ImGui::DragFloat3(
					"Rotate",
					&transform.rotate.x,
					0.01f
				);

				ImGui::DragFloat3(
					"Scale",
					&transform.scale.x,
					0.01f
				);
			}

			//
			// Delete
			//
			if (models.size() > 1) {
				if (ImGui::Button("Delete")) {

					models.erase(
						models.begin() + currentObjectIndex
					);

					transforms.erase(
						transforms.begin() + currentObjectIndex
					);

					textureIndices.erase(
						textureIndices.begin() + currentObjectIndex
					);

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

			//
			// Material
			//
			if (ImGui::CollapsingHeader(
				"Material",
				ImGuiTreeNodeFlags_DefaultOpen
			)) {

				ImGui::ColorEdit4(
					"Color",
					&models[currentObjectIndex]
					.GetMaterial()
					.GetMaterialData()
					->color.x
				);

				bool enableLighting =
					models[currentObjectIndex]
					.GetMaterial()
					.GetMaterialData()
					->enableLighting != 0;

				if (ImGui::Checkbox(
					"Lighting",
					&enableLighting
				)) {

					models[currentObjectIndex]
						.GetMaterial()
						.GetMaterialData()
						->enableLighting =
						enableLighting ? 1 : 0;
				}

				if (!models.empty())
				{
					ImGui::Combo(
						"Texture",
						&textureIndices[currentObjectIndex],
						textureItems,
						IM_ARRAYSIZE(textureItems)
					);
				}
			}
		}

		//
		// Light
		//
		if (ImGui::CollapsingHeader(
			"Light",
			ImGuiTreeNodeFlags_DefaultOpen
		)) {

			ImGui::ColorEdit4(
				"LightColor",
				&lighting.GetLightingData()->color.x
			);

			ImGui::SliderFloat3(
				"Direction",
				&lighting.GetLightingData()->direction.x,
				-1.0f,
				1.0f
			);

			ImGui::DragFloat(
				"Intensity",
				&lighting.GetLightingData()->intensity,
				0.05f,
				0.0f,
				10.0f
			);
		}

		lighting.Update();

		ImGui::End();

		ImGuiManager::GetInstance()->EndFrame();

#endif

		/*
#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");
		// モデル
		ImGui::SliderAngle("SphereRotateX", &modelTransform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &modelTransform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &modelTransform.rotate.z);

		// カメラ
		ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.1f);
		ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
		ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
		ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);


		// マテリアル
		ImGui::ColorEdit4("color", &model[currentModelIndex].GetMaterial().GetMaterialData()->color.x);
		ImGui::Combo(
			"Texture", &currentTextureIndex, textureItems, IM_ARRAYSIZE(textureItems));

		ImGui::Combo(
			"Model", &currentModelIndex, modelItems, IM_ARRAYSIZE(modelItems));


		// Sprite
		ImGui::ColorEdit4("colorSprite", &sprite.GetMaterial().GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &sprite.GetTransform().translate.x, 0.0f, 500.0f);

		// Lighting
		bool enableLighting = model[currentModelIndex].GetMaterial().GetMaterialData()->enableLighting != 0;
		ImGui::Checkbox("enableLighting", &enableLighting);
		model[currentModelIndex].GetMaterial().GetMaterialData()->enableLighting = enableLighting ? 1 : 0;

		ImGui::ColorEdit4("LightColor", &lighting.GetLightingData()->color.x);
		ImGui::SliderFloat3("LightDirection", &lighting.GetLightingData()->direction.x, -1.0f, 1.0f);
		lighting.Update();
		ImGui::DragFloat("Intensity", &lighting.GetLightingData()->intensity, 0.05f, 0.0f, 10.0f);

		// uvTransform
		ImGui::DragFloat2("UVTransform", &sprite.GetUVTransform().translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &sprite.GetUVTransform().scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &sprite.GetUVTransform().rotate.z);

		ImGui::End();

		// ImGuiの内部コマンドを生成する
		ImGuiManager::GetInstance()->EndFrame();
#endif
		*/

		//===============
		// 描画処理
		//===============
		directXCommon.BeginFrame();

		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline.GetPipelineState());

		// 形状を設定。PSOに設定している者とはまた別。同じものを設定すると考えておけば良い。
		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Lighting
		lighting.Bind(3, commandList);

		// モデル描画
		// model.Draw(commandList, texture[currentTextureIndex]);
		for (size_t i = 0; i < models.size(); i++) {
			if (i < textureIndices.size()) {
				models[i].Draw(
					commandList,
					texture[textureIndices[i]]
				);
			}
		}

		skydomeModel.Draw(commandList, skydomeTexture);

		// スプライト描画
		// sprite.Draw(commandList, spriteTexture);

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

	sound.SoundUnload(&audioHandle);

	sound.Finalize();

	CoUninitialize();

	return 0;
}