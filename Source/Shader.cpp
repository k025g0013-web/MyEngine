#include "Shader.h"

#include "Logger.h"
#include "Utils/ConvertString.h"

#include <cassert>
#include <format>

// 変数の実体定義
Logger *Shader::sLogger_ = nullptr;
Microsoft::WRL::ComPtr<IDxcUtils> Shader::sDxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Shader::sDxcCompiler_ = nullptr;
IDxcIncludeHandler *Shader::sIncludeHandler_ = nullptr;

void Shader::InitializeCompiler(Logger *logger) {
    sLogger_ = logger;

    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(sDxcUtils_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(sDxcCompiler_.GetAddressOf()));
    assert(SUCCEEDED(hr));

    // includeに対応するための設定
    hr = sDxcUtils_->CreateDefaultIncludeHandler(&sIncludeHandler_);
    assert(SUCCEEDED(hr));

    (void)hr;
}

void Shader::Compile(const std::wstring &filePath, const wchar_t *profile) {
    // これからシェーダーをコンパイルする旨をログに出す
    sLogger_->Log(ConvertString(std::format(L"Begin Compileshader, path:{}, profile:{}\n", filePath, profile)));

    // hlslファイルを読む
    Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource;
    HRESULT hr = sDxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
    assert(SUCCEEDED(hr));

    // 読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer{};
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

    // Compileするオプション
    LPCWSTR arguments[] = {
        filePath.c_str(),
        L"-E", L"main",
        L"-T", profile, 
        L"-Zi", L"-Qembed_debug",
        L"-Od",
        L"-Zpr",
    };

    // 実際にshaderをコンパイルする
    Microsoft::WRL::ComPtr<IDxcResult> shaderResult;
    hr = sDxcCompiler_->Compile(
        &shaderSourceBuffer,
        arguments,
        _countof(arguments),
        sIncludeHandler_,
        IID_PPV_ARGS(&shaderResult)
    );
    assert(SUCCEEDED(hr));

    // 警告・エラーがでていないか確認する
    Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        sLogger_->Log(shaderError->GetStringPointer());
        assert(false && "シェーダーコンパイルエラーが発生しました。詳細はログを確認してください。");
    }

    // 成功したら実行用バイナリを自分自身に保存
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob_), nullptr);
    assert(SUCCEEDED(hr));

    // 成功したログを出す
    sLogger_->Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

    // 一時リソースの解放
    shaderSource->Release();
    shaderResult->Release();
  
    (void)hr;
}