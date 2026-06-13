#include "RootSignature.h"
#include "Logger.h"

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include <cassert>

void RootSignature::Initialize(ID3D12Device *device, Logger *logger, Type type) {
	// RootSignatureDesc
	//========================
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// descriptorRange
	//========================
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.BaseShaderRegister = 0;
	descriptorRange.NumDescriptors = 1;
	descriptorRange.RangeType =
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV;    // SRVを使う
	descriptorRange.OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	// Offsetを自動計算

	// RootParameter生成
	//========================
	D3D12_ROOT_PARAMETER rootParameters[4]{};
	// Material
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader
	rootParameters[0].Descriptor.ShaderRegister = 0;                    // レジスタ番号0とバインド
	// Transform
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShader
	rootParameters[1].Descriptor.ShaderRegister = 0;					// レジスタ番号0とバインド
	// Texture
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// Descript0rTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; 			// PixelShader
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange;			// Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = 1;						// Tableの利用する
	// Light
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShader
	rootParameters[3].Descriptor.ShaderRegister = 1;					// レジスタ番号1を使う

	if (type == Type::Skinny2D) {
		descriptionRootSignature.NumParameters = 3;
	} else {
		descriptionRootSignature.NumParameters = 4;
	}
	descriptionRootSignature.pParameters = rootParameters;

	// Sampler
	//========================
	D3D12_STATIC_SAMPLER_DESC staticSamplers{};
	staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;    // バイリニアフィルタ

	staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;  // 0~1の範囲外をリピート
	staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;    // 比較しない
	staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;  // ありったけのMipmapを使う

	staticSamplers.ShaderRegister = 0;  // レジスタ番号0を使う
	staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;    // PixelShader

	descriptionRootSignature.pStaticSamplers = &staticSamplers;
	descriptionRootSignature.NumStaticSamplers = 1;

	// Serialize
	//========================
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob
	);

	if (FAILED(hr)) {
		if (errorBlob) {
			logger->Log(reinterpret_cast<char *>(errorBlob->GetBufferPointer()));
		}
		assert(false);
	}

	// Create
	//========================
	hr = device->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}