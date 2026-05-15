#include "CompileShader.h"

#include "Shader.h"

#include "Logger.h"
#include "ConvertString.h"

#include <cassert>
#include <format>

// dxcCompilerを初期化
void CompileShader::Initialize(Logger* logger) {
    logger_ = logger;

    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxcUtils_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxcCompiler_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // includeに対応するための設定
    hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr));
}

// CompileShader
Shader CompileShader::Compile(
	const std::wstring& filePath, const wchar_t* profile
) {
	// hlslファイルを読む
	//===============
	// これからシェーダーをコンパイルする旨をログに出す
	logger_->Log(ConvertString(std::format(L"Begin Compileshader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer{};
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;	// UTF8の文字コードであることを通知

	// Compileする
	//===============
	LPCWSTR arguments[] = {
		filePath.c_str(),	// コンパイル対象のhlslファイル名
		L"-E", L"main",		// エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile,		// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	// デバッグ用の情報を埋め込む
		L"-Od",		// 最適化を外しておく
		L"-Zpr",	// メモリレイアウトは行優先
	};
	// 実際にをshaderコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプション
		includeHandler_.Get(),	// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

	// 警告・エラーがでていないか確認する
	//===============
	// 警告・エラーが出てたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		logger_->Log(shaderError->GetStringPointer());
		// 警告・エラーダメゼッタイ
		assert(false);
	}

	// Compile結果を受け取って返す
	//===============

	// コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	logger_->Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

	Shader shader;
	shader.SetBlob(shaderBlob.Get());
	return shader;
}
