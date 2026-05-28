#pragma region include

#include <windows.h>
#include <wrl.h>

#include <cassert>

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

// ImGui
#include "ImGuiManager.h"

// Object
#include "Model.h"
#include "Triangle.h"
#include "Sprite.h"
#include "Camera.h"

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

	// モデル
	Model axis;
	axis.Initialize(
		device, "resources", "axis.obj", 0xFFFFFFFF, true
	);

	// モデル用テクスチャ
	Texture axisTexture[4];
	axisTexture[0].Initialize(device, commandList, directXCommon.GetSRVHeap(), axis.GetModelData().material.textureFilePath);
	axisTexture[1].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/monsterBall.png");
	axisTexture[2].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/test0.png");
	axisTexture[3].Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/test1.png");

	Model plane;
	plane.Initialize(device,
		"resources", "plane.obj", 0xFFFFFFFF, true
	);

	// 三角形
	Triangle triangle[2];
	triangle[0].Initialize(device, { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.5f, 0.0f }, { 0.5f, -0.5f,  0.0f }, 0xFFFFFFFF);
	triangle[1].Initialize(device, { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, -0.5f, -0.5f }, 0xFFFFFFFF);

	// スプライト
	Sprite sprite;
	sprite.Initialize(device, 0.0f, 0.0f, 640.0f, 360.0f, 0xFFFFFFFF);

	// スプライト用テクスチャ
	Texture spriteTexture;
	spriteTexture.Initialize(device, commandList, directXCommon.GetSRVHeap(), "resources/uvChecker.png");

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

	int currentTextureIndex = 0;
	const char *textureItems[] = {
		"resources/uvChecker.png",
		"resources/monsterBall.png",
		"resources/Test0.png",
		"resources/Test1.png"
	};

	//===============
	// メインループ
	//===============
	// ウィンドウの×ボタンが押されるまでループ
	while (winApp.ProcessMessage()) {

		// ゲームの処理
		//===============
		// カメラ処理
		camera.Update();

		// モデル更新
		axis.Update(&camera);
		plane.Update(&camera);

		triangle[0].Update(&camera);
		triangle[1].Update(&camera);

		// スプライト更新
		sprite.Update(kClientWidth, kClientHeight);

#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		ImGui::Begin("Setting");
		// モデル
		ImGui::SliderAngle("SphereRotateX", &axis.GetTransform().rotate.x);
		ImGui::SliderAngle("SphereRotateY", &axis.GetTransform().rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &axis.GetTransform().rotate.z);

		// カメラ
		ImGui::DragFloat3("CameraTranslate", &camera.GetTransform().translate.x, 0.1f);
		ImGui::SliderAngle("CameraRotateX", &camera.GetTransform().rotate.x);
		ImGui::SliderAngle("CameraRotateY", &camera.GetTransform().rotate.y);
		ImGui::SliderAngle("CameraRotateZ", &camera.GetTransform().rotate.z);

		// マテリアル
		ImGui::ColorEdit4("color", &axis.GetMaterial().GetMaterialData()->color.x);
		ImGui::Combo(
			"Texture", &currentTextureIndex, textureItems, IM_ARRAYSIZE(textureItems));

		// Sprite
		ImGui::ColorEdit4("colorSprite", &sprite.GetMaterial().GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &sprite.GetTransform().translate.x, 0.0f, 500.0f);

		// Lighting
		bool enableLighting = axis.GetMaterial().GetMaterialData()->enableLighting != 0;
		ImGui::Checkbox("enableLighting", &enableLighting);
		axis.GetMaterial().GetMaterialData()->enableLighting = enableLighting ? 1 : 0;

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

		//===============
		// 画面に描けるようにする
		//===============
		// DirectX毎フレーム処理
		directXCommon.BeginFrame();

		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline.GetPipelineState());

		// 形状を設定。PSOに設定している者とはまた別。同じものを設定すると考えておけば良い。
		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Lighting
		lighting.Bind(3, commandList);

		// モデル描画
		axis.Draw(commandList, axisTexture[currentTextureIndex]);
		plane.Draw(commandList, axisTexture[currentTextureIndex]);

		// スプライト描画
		sprite.Draw(commandList, spriteTexture);

		triangle[0].Draw(commandList, axisTexture[currentTextureIndex]);
		triangle[1].Draw(commandList, axisTexture[currentTextureIndex]);

#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->Draw(commandList);
#endif

		// 画面入れ替え
		//=================
		directXCommon.EndFrame();
	}

	//===============
	// COMの終了
	//===============
	// WindowsAPI後始末
	winApp.Finalize();

#ifdef USE_IMGUI
	ImGuiManager::GetInstance()->Finalize();
#endif

	CoUninitialize();

	return 0;
}