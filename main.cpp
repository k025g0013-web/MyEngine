#pragma region include

#include <wrl.h>
#include <windows.h>

// Core
#include "CrashHandler.h"	
#include "WinApp.h"	
#include "Logger.h"	
#include "ConvertString.h"

#include "DirectXCommon.h"		// DirectX初期化

// Pipeline
#include "GraphicsPipeline.h"   // GraphicsPipeline
#include "RootSignature.h"      // RootSignature
#include "InputLayout.h"		// InputLayout
#include "BlendState.h"			// BlendState
#include "RasterizerState.h"	// RasterizerState
#include "DepthStencilState.h"  // DepthStencilState

// Resource
#include "Material.h"	// Material
#include "Shader.h"		// Shader

// Buffer
#include "VertexBuffer.h"	// VertexBuffer
#include "indexBuffer.h"	// IndexBuffer
#include "ConstantBuffer.h"	// ConstantBuffer

#include "Texture.h"	// Texture

// Math
#include "MathFunctions.h"	// MathFunctions

#include "Vector2.h"	// Vector2
#include "Vector3.h"	// Vector3
#include "Vector4.h"	// Vector4
#include "Matrix3x3.h"	// Matrix3x3
#include "Matrix4x4.h"	// Matrix4x4

// Utility
#include "ResourceUtils.h"	// ResourceUtils

// ImGui
#include"ImGuiManager.h"

#include <fstream>
#include <sstream>
#include <chrono>

#include <dbghelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <strsafe.h>

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/d3dx12.h>
#include <vector>

#pragma endregion

#pragma region 構造体

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct MaterialSource {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialSource material;
};

#pragma endregion

#pragma region 関数

// Textureデータを読む
DirectX::ScratchImage LoadTexture(const std::string &filePath) {
	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作製
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミニマップ付きのデータを返す
	return mipImages;
}

// DirectX12のTextureResourceを作る
ID3D12Resource *CreateTextureResource(ID3D12Device *device, const DirectX::TexMetadata &metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);		// Textureの幅
	resourceDesc.Height = UINT(metadata.height);	// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);		// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;	// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;		// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは2次元

	// 利用するHeapの設定。VRAM上に作成する
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceを生成する
	ID3D12Resource *resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	(void)hr;

	return resource;
}

ID3D12Resource *CreateDepthStencilTextureResource(ID3D12Device *device, int32_t width, int32_t height) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;		// Textureの幅
	resourceDesc.Height = height;	// Textureの高さ
	resourceDesc.MipLevels = 1;		// mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;		// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;	// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;	// 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// フォーマット。Resourceと合わせる

	// Resourceの生成
	ID3D12Resource *resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // データ転送される設定
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	(void)hr;

	return resource;
}

// TextureResourceにデータを転送する
void UploadTextureData(ID3D12Resource *texture, const DirectX::ScratchImage &mipImages) {
	// Meta情報を取得
	const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image *img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
		(void)hr;
	}
}

// UploadTextureDataを書き換える
[[nodiscard]]	// 属性
ID3D12Resource *UploadTextureData(
	ID3D12Resource *texture, const DirectX::ScratchImage &mipImages,
	ID3D12Device *device, ID3D12GraphicsCommandList *commandList
) {
	// IntermediateResource(中間リソース)
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource *intermediateResource = CreateBufferResource(device, intermediateSize);

	// データ転送をコマンドに積む
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap *descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap *descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

MaterialSource LoadMaterialTemplateFile(const std::string &directoryPath, const std::string &fileName) {
	// 中で必要となる変数の宣言
	MaterialSource materialData;	// 構築するMaterialData
	std::string line;	// ファイルから読んだ1行を格納するもの

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + fileName);
	assert(file.is_open());	// とりあえず開けなかったら止める

	// 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFileName;
			s >> textureFileName;

			// 連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFileName;
		}
	}

	// materialDataを返す
	return materialData;
}

ModelData LoadObjFile(const std::string &directoryPath, const std::string &fileName) {
	// 中で必要となる変数の宣言
	ModelData modelData;	// 構築するModelData
	std::vector<Vector4> positions;	// 位置
	std::vector<Vector3> normals;	// 法線
	std::vector<Vector2> texcoords;	// テクスチャ座標
	std::string line;	// ファイルから読んだ1行を格納するもの

	// ファイルを開く
	std::ifstream file(directoryPath + "/" + fileName);
	assert(file.is_open());	// とりあえず開けなかったら止める

	// 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;	// 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "v") {		// 位置 
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);

		} else if (identifier == "vt") {	// テクスチャ座標
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);

		} else if (identifier == "vn") {	// 法線
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);

		} else if (identifier == "f") {	// 面
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');	// 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで周り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib") {	// Material読み込み
			// materialTemplateLibraryファイルの名前を取得する
			std::string  materialFileName;
			s >> materialFileName;

			// 基本的にobjファイルと同一階層にmtlは存在させるのでディレクトリ名とファイル名を探す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFileName);
		}
	}

	// modelDataを返す
	return modelData;
}

#pragma endregion

// 分割数
const uint32_t kSubdivision = 16;

// クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
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

	// モデル用の頂点データ
#pragma region スプライト用の頂点データ

	// モデル読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");

	// 頂点リソースを作る
	VertexBuffer vertexBuffer;
	vertexBuffer.Initialize(device, sizeof(VertexData) * modelData.vertices.size(), sizeof(VertexData));

	// 書き込むためのアドレスを取得
	VertexData *vertexData = static_cast<VertexData *>(vertexBuffer.Map());

	// 頂点データをリリースにコピー
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	// WVP用のリソースを作る
	ConstantBuffer transformationMatrixBuffer;
	transformationMatrixBuffer.Initialize(device, sizeof(TransformationMatrix));

	// データを書き込む
	TransformationMatrix *transformationMatrixData =
		static_cast<TransformationMatrix *>(transformationMatrixBuffer.Map());

	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();

#pragma endregion

	// スプライト用の頂点データ
#pragma region スプライト用の頂点データ

	// 頂点インデックス
	IndexBuffer indexBufferSprite;
	indexBufferSprite.Initialize(device, sizeof(uint32_t) * 6);

	// インデックスリソースにデータを書き込む
	uint32_t *indexDataSprite = static_cast<uint32_t *>(indexBufferSprite.Map());
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	// 頂点データ
	VertexData spriteVertices[] = {
		{ {  0.0f, 360.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,-1} },
		{ {  0.0f,   0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,-1} },
		{ {640.0f, 360.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,-1} },
		{ {640.0f,   0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,-1} },
	};

	// Sprite用の頂点リソースを作る
	VertexBuffer vertexBufferSprite;
	vertexBufferSprite.Initialize(device, sizeof(spriteVertices), sizeof(VertexData));

	// 頂点リソースにデータを書き込む
	VertexData *vertexDataSprite = static_cast<VertexData *>(vertexBufferSprite.Map());

	// 頂点データをリリースにコピー
	std::memcpy(vertexDataSprite, spriteVertices, sizeof(spriteVertices));

#pragma endregion

	//===============
	// マテリアル
	//===============
	// 3D用
	Material material;
	material.Initialize(device, 0xFFFFFFFF, true);

	// Sprite用
	Material materialSprite;
	materialSprite.Initialize(device, 0xFFFFFFFF, false);

	// WVP用のリソースを作る(Sprite用)
	ConstantBuffer transformationMatrixResourceSprite;
	transformationMatrixResourceSprite.Initialize(device, sizeof(TransformationMatrix));

	// データを書き込む(Sprite用)
	TransformationMatrix *transformationMatrixDataSprite =
		static_cast<TransformationMatrix *>(transformationMatrixResourceSprite.Map());

	// 単位行列を書きこんでおく(Sprite用)
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();

	// uvTransform用の変数
	Transform uvTransformSprite{
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	};

	// 並行光源用のResourceの作成
	ConstantBuffer directionalLightResource;
	directionalLightResource.Initialize(device,sizeof(DirectionalLight));

	// データを書き込む
	DirectionalLight *directionalLightData = 
		static_cast<DirectionalLight *>(directionalLightResource.Map());
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = { 1.0f };

	//===============
	// ViewportとScissor
	//===============
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = kClientWidth;
	viewport.Height = kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = kClientHeight;

	// Textureを組み込む
	//===============
	uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Textureを読んで転送する
	Texture uvTexture;
	uvTexture.Initialize(device, commandList, directXCommon.GetSRVHeap()->GetDescriptorHeap(), descriptorSizeSRV, 1, "resources/uvChecker.png");

	Texture modelTexture;
	modelTexture.Initialize(device, commandList, directXCommon.GetSRVHeap()->GetDescriptorHeap(), descriptorSizeSRV, 2, modelData.material.textureFilePath);

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
#endif

	//===============
	// ゲームループ内変数の初期化
	//===============
	Transform transform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f}, {0.3f,0.0f,0.0f}, {0.0f,4.0f,-10.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f},  {0.0f,0.0f,0.0f} };

	bool useMonsterBall = true;

	//===============
	// メインループ
	//===============
	// ウィンドウの×ボタンが押されるまでループ
	while (winApp.ProcessMessage()) {

		// ゲームの処理
		//===============
		// カメラ処理
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(Multiply(worldMatrix, viewMatrix), projectionMatrix);
		transformationMatrixData->WVP = worldViewProjectionMatrix;
		transformationMatrixData->World = worldMatrix;

		Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
		Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
		Matrix4x4 worldViewProjectionMatrixSprite = Multiply(Multiply(worldMatrixSprite, viewMatrixSprite), projectionMatrixSprite);
		transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
		transformationMatrixDataSprite->World = worldMatrix;

		// uvTransformMatrix
		Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
		uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
		materialSprite.GetMaterialData()->uvTransform = uvTransformMatrix;

#ifdef USE_IMGUI
		ImGuiManager::GetInstance()->BeginFrame();

		ImGui::Begin("setting");

		// モデル
		ImGui::SliderAngle("SphereRotateX", &transform.rotate.x);
		ImGui::SliderAngle("SphereRotateY", &transform.rotate.y);
		ImGui::SliderAngle("SphereRotateZ", &transform.rotate.z);

		// カメラ
		ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.1f);
		ImGui::SliderAngle("CameraRotateX", &cameraTransform.rotate.x);
		ImGui::SliderAngle("CameraRotateY", &cameraTransform.rotate.y);
		ImGui::SliderAngle("CameraRotateZ", &cameraTransform.rotate.z);

		// 球
		ImGui::ColorEdit4("color", &material.GetMaterialData()->color.x);
		ImGui::Checkbox("useMonsterBall", &useMonsterBall);

		// Sprite
		ImGui::ColorEdit4("colorSprite", &materialSprite.GetMaterialData()->color.x);
		ImGui::SliderFloat3("translateSprite", &transformSprite.translate.x, 0.0f, 500.0f);

		// Lighting
		bool enableLighting = material.GetMaterialData()->enableLighting != 0;
		ImGui::Checkbox("enableLighting", &enableLighting);
		material.GetMaterialData()->enableLighting = enableLighting ? 1 : 0;

		ImGui::ColorEdit4("LightColor", &directionalLightData->color.x);
		ImGui::SliderFloat3("LightDirection", &directionalLightData->direction.x, -1.0f, 1.0f);
		directionalLightData->direction = Normalize(directionalLightData->direction);
		ImGui::DragFloat("Intensity", &directionalLightData->intensity, -100.0f, 100.0f);

		// uvTransform
		ImGui::DragFloat2("UVTransform", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);

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
		commandList->IASetVertexBuffers(0, 1, &vertexBuffer.GetView());
		// commandList->IASetIndexBuffer(&indexBufferView);
		// 形状を設定。PSOに設定している者とはまた別。同じものを設定すると考えておけば良い。
		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 
		commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.GetGPUVirtualAddress());

		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, material.GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer.GetGPUVirtualAddress());

		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? modelTexture.GetGPUHandle() : uvTexture.GetGPUHandle());

		// 描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今度
		// commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		commandList->SetGraphicsRootConstantBufferView(0, materialSprite.GetGPUVirtualAddress());

		// Spriteを常にuvCheckerにする
		commandList->SetGraphicsRootDescriptorTable(2, uvTexture.GetGPUHandle());

		// Spriteの描画。変更が必要なものだけ変更する
		commandList->IASetVertexBuffers(0, 1, &vertexBufferSprite.GetView());
		commandList->IASetIndexBuffer(&indexBufferSprite.GetView());	// IBVを設定する
		// TransformationMatrixCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite.GetGPUVirtualAddress());
		// 描画!(DrawCall/ドローコール)
		// commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

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

#ifdef _DEBUG
	// リソースリークチェック
	Microsoft::WRL::ComPtr <IDXGIDebug1> debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
#endif

	CoUninitialize();

	return 0;
}