#pragma region include

#include <windows.h>

// Core
#include "CrashHandler.h"	
#include "WinApp.h"	
#include "Logger.h"	
#include "ConvertString.h"

// DirectX
// #include "DirectXCommon.h"
#include "DirectXDevice.h"      // Device
#include "CommandContext.h"     // Command系統
#include "SwapChain.h"          // SwapChain
#include "Fence.h"              // Fence
#include "DescriptorHeap.h"     // DescriptorHeap
#include "RenderTargetViews.h"  // rtv
#include "CompileShader.h"      // CompileShader

// Pipeline
#include "GraphicsPipeline.h"      // GraphicsPipeline
#include "RootSignature.h"      // RootSignature
#include "InputLayout.h"		// InputLayout
#include "BlendState.h"			// BlendState
#include "RasterizerState.h"	// RasterizerState
#include "DepthStencilState.h"  // DepthStencilState

// Resource
#include "Shader.h"	// Shader

#define _USE_MATH_DEFINES
#include <cmath>

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

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx12.h>
#include <imgui/imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/d3dx12.h>
#include <vector>

#pragma endregion

#pragma region 構造体

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix3x3 {
	float m[3][3];
};

struct Matrix4x4 {
	float m[4][4];
};

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

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
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

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

#pragma endregion

#pragma region 関数

#pragma region Math

// 長さ
float Length(const Vector3 &v) {
	return sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2));
}

// 正規化
Vector3 Normalize(const Vector3 &v) {
	float length = Length(v);
	Vector3 result = {
		v.x / length,
		v.y / length,
		v.z / length,
	};

	return result;
}

// 単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		result.m[i][i] = 1;
	}
	return result;
}

// 行列の積
Matrix4x4 Multiply(const Matrix4x4 &m1, const Matrix4x4 &m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			for (int k = 0; k < 4; k++) {
				result.m[row][column] += m1.m[row][k] * m2.m[k][column];
			}
		}
	}
	return result;
}

// 平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3 &translate) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;

	return result;
}

// 拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3 &scale) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

	return result;
}

// x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[1][1] = std::cos(radian);
	result.m[1][2] = std::sin(radian);

	result.m[2][1] = -std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
};

// y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = std::cos(radian);
	result.m[0][2] = -std::sin(radian);

	result.m[2][0] = std::sin(radian);
	result.m[2][2] = std::cos(radian);

	return result;
};

// z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = MakeIdentity4x4();

	result.m[0][0] = std::cos(radian);
	result.m[0][1] = std::sin(radian);

	result.m[1][0] = -std::sin(radian);
	result.m[1][1] = std::cos(radian);

	return result;
};

// アフィン変換
Matrix4x4 MakeAffineMatrix(const Vector3 &scale, const Vector3 &rotation, const Vector3 &translation) {

	Matrix4x4 s = MakeScaleMatrix(scale);

	Matrix4x4 r = Multiply(Multiply(
		MakeRotateXMatrix(rotation.x),
		MakeRotateYMatrix(rotation.y)),
		MakeRotateZMatrix(rotation.z)
	);

	Matrix4x4 t = MakeTranslateMatrix(translation);

	return Multiply(Multiply(s, r), t);
}

// 行列式
float Det3(
	float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) {

	return
		a1 * (b2 * c3 - b3 * c2) -
		a2 * (b1 * c3 - b3 * c1) +
		a3 * (b1 * c2 - b2 * c1);
}

// 逆行列
Matrix4x4 Inverse(const Matrix4x4 &m) {
	Matrix4x4 result = {};

	// 余因子行列
	Matrix4x4 cofactor = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			float sub[3][3]{};
			int r = 0;

			// 元の行列を走査
			for (int row = 0; row < 4; ++row) {
				if (row == i) continue;
				int c = 0;

				for (int col = 0; col < 4; ++col) {
					if (col == j) continue;
					sub[r][c] = m.m[row][col];
					c++;
				}
				r++;
			}

			// 3x3行列式
			float minor = Det3(
				sub[0][0], sub[0][1], sub[0][2],
				sub[1][0], sub[1][1], sub[1][2],
				sub[2][0], sub[2][1], sub[2][2]
			);

			// 符号付き余因子
			float sign;
			if ((i + j) % 2 == 0) {
				sign = 1.0f;
			} else {
				sign = -1.0f;
			}

			cofactor.m[i][j] = sign * minor;
		}
	}

	// 転置行列
	Matrix4x4 adjugate = {};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			adjugate.m[i][j] = cofactor.m[j][i];
		}
	}

	// 行列式
	float det = 0.0f;
	for (int j = 0; j < 4; ++j) {
		det += m.m[0][j] * cofactor.m[0][j];
	}

	// ゼロチェック
	if (det == 0.0f) {
		return result;
	}

	// 逆行列
	float invDet = 1.0f / det;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = adjugate.m[i][j] * invDet;
		}
	}

	return result;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};

	float fov = 1.0f / std::tan(fovY / 2.0f);

	result.m[0][0] = fov / aspectRatio;
	result.m[1][1] = fov;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return result;
};

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {};

	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][3] = 1.0f;

	result.m[3][0] = -(right + left) / (right - left);
	result.m[3][1] = -(top + bottom) / (top - bottom);
	result.m[3][2] = -nearClip / (farClip - nearClip);

	return result;
};

#pragma endregion

// Resource作成
ID3D12Resource *CreateBufferResource(ID3D12Device *device, size_t sizeInBytes) {
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際に頂点リソース
	ID3D12Resource *resource = nullptr;
	HRESULT hr =
		device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
			&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	(void)hr;

	return resource;
}

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

MaterialData LoadMaterialTemplateFile(const std::string &directoryPath, const std::string &fileName) {
	// 中で必要となる変数の宣言
	MaterialData materialData;	// 構築するMaterialData
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
	ID3D12Debug1 *debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

#pragma endregion

	// DirectX初期化
#pragma region DirectX初期化

	/*
	DirectXCommon directXCommon;
	directXCommon.Initialize(&winApp, &logger, kClientWidth, kClientHeight);
	*/

	// Device生成
	DirectXDevice directXDevice;
	directXDevice.Initialize(&logger);
	ID3D12Device *device = directXDevice.GetDevice();

	// Command系統生成
	CommandContext commandContext;
	commandContext.Initialize(&directXDevice);
	ID3D12GraphicsCommandList *commandList = commandContext.GetCommandList();

	// SwapChain生成
	SwapChain swapChain;
	swapChain.Initialize(&directXDevice, &commandContext, &winApp, kClientWidth, kClientHeight);

	// DescriptorHeap生成
	DescriptorHeap rtvDescriptorHeap;	// rtv
	rtvDescriptorHeap.Initialize(directXDevice.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	DescriptorHeap srvDescriptorHeap;	// srv
	srvDescriptorHeap.Initialize(directXDevice.GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	// rtv
	RenderTargetViews renderTargetViews;
	renderTargetViews.Initialize(directXDevice.GetDevice(), &swapChain, &rtvDescriptorHeap);

	// Fence
	Fence fence;
	fence.Initialize(directXDevice.GetDevice());

	// CompileShader
	CompileShader compileShader;
	compileShader.Initialize(&logger);

#pragma endregion
	
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
	Shader vertexShader = compileShader.Compile(L"Object3D.VS.hlsl", L"vs_6_0");
	
	// PixelShader
	Shader pixelShader = compileShader.Compile(L"Object3D.PS.hlsl", L"ps_6_0");

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

	//===============
	// 頂点データの作成とビュー
	//===============
	// モデル読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	// 頂点リソースを作る
	ID3D12Resource *vertexResource = CreateBufferResource(device, sizeof(VertexData) * modelData.vertices.size());

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();	// リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);	// 1頂点あたりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData *vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void **>(&vertexData));	// 書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());	// 頂点データをリリースにコピー

	/*
	const uint32_t indexCount = kSubdivision * kSubdivision * 6;

	ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * indexCount);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * indexCount;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	// 書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr,
		reinterpret_cast<void**>(&vertexData));

	// 頂点インデックス
	ID3D12Resource* indexResource = CreateBufferResource(device, sizeof(uint32_t) * indexCount);

	// Spriteとほぼ同様。サイズのみ変更
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * indexCount;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t* indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	const float kLonEvery = static_cast<float>(M_PI) * 2.0f / float(kSubdivision);	// 経度分割1つ分の角度
	const float kLatEvery = static_cast<float>(M_PI) / float(kSubdivision);			// 緯度分割1つ分の角度

	// 緯度の方向に分割 -pi/2 ~ pi/2
	for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

		// 経度の方向に分割 0 ~ 2pi
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			// 頂点生成
			uint32_t index = latIndex * (kSubdivision + 1) + lonIndex;

			// world座標を求める
			Vector3 pos = {
				std::cos(lat) * std::cos(lon),
				std::sin(lat),
				std::cos(lat) * std::sin(lon),
			};

			// Texcoordを計算する
			float u = float(lonIndex) / kSubdivision;
			float v = 1.0f - float(latIndex) / kSubdivision;

			// 頂点データの作成
			vertexData[index] = {
				.position{pos.x, pos.y, pos.z, 1.0f},
				.texcoord{u, v},
				.normal{pos.x, pos.y, pos.z},
			};

			// インデックスデータの生成
			if (latIndex < kSubdivision && lonIndex < kSubdivision) {
				uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

				// 四角形の4頂点
				uint32_t a = index;
				uint32_t b = (latIndex + 1) * (kSubdivision + 1) + lonIndex;
				uint32_t c = latIndex * (kSubdivision + 1) + (lonIndex + 1);
				uint32_t d = (latIndex + 1) * (kSubdivision + 1) + (lonIndex + 1);

				// インデックスリソースにデータを書き込む
				indexData[start + 0] = a;	indexData[start + 1] = b;	indexData[start + 2] = c;
				indexData[start + 3] = c;	indexData[start + 4] = b;	indexData[start + 5] = d;
			}
		}
	}
	*/

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ID3D12Resource *transformationMatrixResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix *transformationMatrixData = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixData));
	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();

	// 頂点インデックス
	ID3D12Resource *indexResourceSprite = CreateBufferResource(device, sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックスから6つ分のサイズ
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	uint32_t *indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&indexDataSprite));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	// Sprite用の頂点リソースを作る
	ID3D12Resource *vertexResourceSprite = CreateBufferResource(device, sizeof(VertexData) * 4);

	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点あたりのサイズ
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	// 頂点リソースにデータを書き込む
	VertexData *vertexDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&vertexDataSprite));
	// 頂点データ
	vertexDataSprite[0] = { {0.0f,   360.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {0,0,-1} };
	vertexDataSprite[1] = { {0.0f,     0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {0,0,-1} };
	vertexDataSprite[2] = { {640.0f, 360.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {0,0,-1} };
	vertexDataSprite[3] = { {640.0f,   0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {0,0,-1} };

	//===============
	// マテリアル
	//===============
	// マテリアル用のリソースを作る
	ID3D12Resource *materialResource = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	Material *materialData = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void **>(&materialData));
	// 今回は赤を書き込んでみる
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();

	// マテリアル用のリソースを作る
	ID3D12Resource *materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	// マテリアルにデータを書き込む
	Material *materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&materialDataSprite));
	// 今回は赤を書き込んでみる
	materialDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = MakeIdentity4x4();
	Transform uvTransformSprite{
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	};

	// Sprite用のTransformationMatrix用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ID3D12Resource *transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix *transformationMatrixDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void **>(&transformationMatrixDataSprite));
	// 単位行列を書きこんでおく
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();

	// 並行光源用のResourceの作成
	ID3D12Resource *directionalLightResource = CreateBufferResource(device, sizeof(DirectionalLight));
	// データを書き込む
	DirectionalLight *directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void **>(&directionalLightData));
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

	//===============
	// ImGuiの初期化
	//===============
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX12_Init(
		device,
		swapChain.GetBufferCount(),
		renderTargetViews.GetRTVDesc().Format,
		srvDescriptorHeap.GetDescriptorHeap(),
		srvDescriptorHeap.GetCPUDescriptorHandle(0),
		srvDescriptorHeap.GetGPUDescriptorHandle(0)
	);
	ImGuiIO &io = ImGui::GetIO();
	io.Fonts->Build();
#endif

	//===============
	// Textureを組み込む
	//===============
	// Textureを読んで転送する
	DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata &metadata = mipImages.GetMetadata();
	ID3D12Resource *textureResource = CreateTextureResource(device, metadata);
	ID3D12Resource *intermediateResource = UploadTextureData(textureResource, mipImages, device, commandList);

	// 2枚目のTextureを読んで転送する
	DirectX::ScratchImage mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata &metadata2 = mipImages2.GetMetadata();
	ID3D12Resource *textureResource2 = CreateTextureResource(device, metadata2);
	ID3D12Resource *intermediateResource2 = UploadTextureData(textureResource2, mipImages2, device, commandList);

	// metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	uint32_t descriptorSizeRSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap.GetDescriptorHeap(), descriptorSizeRSV, 1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap.GetDescriptorHeap(), descriptorSizeRSV, 1);
	// SRVの作成
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);

	// metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap.GetDescriptorHeap(), descriptorSizeRSV, 2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap.GetDescriptorHeap(), descriptorSizeRSV, 2);
	// SRVの作成
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);

	// DepthStencilTextureをウィンドウのサイズで作成
	ID3D12Resource *depthStencilResource = CreateDepthStencilTextureResource(device, kClientWidth, kClientHeight);
	// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	DescriptorHeap dsvDescriptorHeap;
	dsvDescriptorHeap.Initialize(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	// DSVHeapの先頭にDSVをつくる
	device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap.GetCPUDescriptorHandle(0));

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
		materialDataSprite->uvTransform = uvTransformMatrix;

#ifdef USE_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

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
		ImGui::ColorEdit4("color", &materialData->color.x);
		ImGui::Checkbox("useMonsterBall", &useMonsterBall);

		// Sprite
		ImGui::ColorEdit4("colorSprite", &materialDataSprite->color.x);
		ImGui::SliderFloat3("translateSprite", &transformSprite.translate.x, 0.0f, 500.0f);

		// Lighting
		bool enableLighting = materialData->enableLighting != 0;
		ImGui::Checkbox("enableLighting", &enableLighting);
		materialData->enableLighting = enableLighting ? 1 : 0;

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
		ImGui::Render();
#endif

		// DirectX毎フレーム処理
		// directXCommon.BeginFrame();

		//===============
		// 画面に描けるようにする
		//===============
		// これから書き込むバックバッファのインデックスを取得
		UINT backBufferIndex = swapChain.GetCurrentBackBufferIndex();

		// TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrier{};
		// 今回のバリアはTranslation
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		// Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		// バリアを張る対象のリソース。現在のバックバッファに対して行う
		barrier.Transition.pResource = swapChain.GetBackBuffer(backBufferIndex);
		// 遷移前(現在)のResourceState
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		// 遷移後のResourceState
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		// TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		// 描画先のRTVとDSVを設定する
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap.GetCPUDescriptorHandle(0);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTargetViews.GetRTVHandle(backBufferIndex);
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
		// 指定した色で画面全体をクリアする
		float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
		commandList->ClearRenderTargetView(renderTargetViews.GetRTVHandle(backBufferIndex), clearColor, 0, nullptr);

		// 指定した深度で画面全体をクリアする
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// 描画用DescriptorHeapの設定
		ID3D12DescriptorHeap *descriptorHeaps[] = { srvDescriptorHeap.GetDescriptorHeap() };
		commandList->SetDescriptorHeaps(1, descriptorHeaps);

		commandList->RSSetViewports(1, &viewport);		// Viewportを設定
		commandList->RSSetScissorRects(1, &scissorRect);// Scissorを設定
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		commandList->SetGraphicsRootSignature(rootSignature.GetRootSignature());
		commandList->SetPipelineState(graphicsPipeline.GetPipelineState());
		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		// commandList->IASetIndexBuffer(&indexBufferView);
		// 形状を設定。PSOに設定している者とはまた別。同じものを設定すると考えておけば良い。
		commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 
		commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

		// マテリアルCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		// wvp用のCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

		// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
		commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);

		// 描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今度
		// commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
		commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());

		// Spriteを常にuvCheckerにする
		commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

		// Spriteの描画。変更が必要なものだけ変更する
		commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
		commandList->IASetIndexBuffer(&indexBufferViewSprite);	// IBVを設定する
		// TransformationMatrixCBufferの場所を設定
		commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
		// 描画!(DrawCall/ドローコール)
		// commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

#ifdef USE_IMGUI
			// 実際のcommandListのImGuiの描画コマンドを積む
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif

		// 画面に描く処理はすべて終わり、画面に映すので、状態を遷移
		// 今回はRenderTargetからPresentにする
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		// TransitionBarrierを張る
		commandList->ResourceBarrier(1, &barrier);

		// 画面入れ替え
		//=================
		// directXCommon.EndFrame();
		// コマンドリストの内容を確定させる。すべてのコマンドを詰んでからClearすること
		commandContext.Close();

		// GPUにコマンドリストの実行を行わせる
		commandContext.Execute();

		// GPUとOSに画面の交換を行うよう通知する
		swapChain.Present();

		fence.Wait(commandContext.GetCommandQueue());

		// 次のフレーム用のコマンドリストを準備
		commandContext.Reset();
	}

	//===============
	// COMの終了
	//===============
	CoUninitialize();

	// WindowsAPI後始末
	winApp.Finalize();

#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

#ifdef _DEBUG
	debugController->Release();
#endif

	// リソースリークチェック
	IDXGIDebug1 *debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	vertexResource->Release();

	materialResource->Release();

	intermediateResource->Release();
	intermediateResource2->Release();

	depthStencilResource->Release();

	materialResourceSprite->Release();

	directionalLightResource->Release();

	return 0;
}