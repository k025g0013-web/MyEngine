#include "Shader.h"

#include "Core/Logger.h"
#include "Utils/ConvertString.h"

#include <cassert>
#include <format>

namespace Kizuna {
    // 静的メンバの実体を定義する
    Logger *Shader::sLogger_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcUtils> Shader::sDxcUtils_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcCompiler3> Shader::sDxcCompiler_ = nullptr;
    IDxcIncludeHandler *Shader::sIncludeHandler_ = nullptr;

    void Shader::InitializeCompiler(Logger *logger) {
        // ログ出力先を保持する
        sLogger_ = logger;

        // DXCユーティリティを生成する
        HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(sDxcUtils_.GetAddressOf()));
        assert(SUCCEEDED(hr));

        // DXCコンパイラを生成する
        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(sDxcCompiler_.GetAddressOf()));
        assert(SUCCEEDED(hr));

        // include文を処理するためのハンドラを生成する
        hr = sDxcUtils_->CreateDefaultIncludeHandler(&sIncludeHandler_);
        assert(SUCCEEDED(hr));

        (void)hr;
    }

    void Shader::Compile(const std::wstring &filePath, const wchar_t *profile) {
        // シェーダーコンパイル開始をログへ出力する
        sLogger_->Log(ConvertString(std::format(L"Begin Compileshader, path:{}, profile:{}\n", filePath, profile)));

        // シェーダーファイルを読み込む
        Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource;
        HRESULT hr = sDxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
        assert(SUCCEEDED(hr));

        // 読み込んだシェーダー情報を設定する
        DxcBuffer shaderSourceBuffer{};
        shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
        shaderSourceBuffer.Size = shaderSource->GetBufferSize();
        shaderSourceBuffer.Encoding = DXC_CP_UTF8;

        // コンパイルオプションを設定する
        LPCWSTR arguments[] = {
            filePath.c_str(),
            L"-E", L"main",
            L"-T", profile,
            L"-Zi", L"-Qembed_debug",
            L"-Od",
            L"-Zpr",
        };

        // シェーダーをコンパイルする
        Microsoft::WRL::ComPtr<IDxcResult> shaderResult;
        hr = sDxcCompiler_->Compile(
            &shaderSourceBuffer,
            arguments,
            _countof(arguments),
            sIncludeHandler_,
            IID_PPV_ARGS(&shaderResult)
        );
        assert(SUCCEEDED(hr));

        // コンパイルエラーや警告がないか確認する
        Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError;
        shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);

        if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
            // エラーメッセージをログへ出力する
            sLogger_->Log(shaderError->GetStringPointer());
            assert(false && "シェーダーコンパイルエラーが発生しました。詳細はログを確認してください。");
        }

        // コンパイル済みシェーダーバイナリを取得する
        hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob_), nullptr);
        assert(SUCCEEDED(hr));

        // コンパイル成功をログへ出力する
        sLogger_->Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));

        // 一時リソースを解放する
        shaderSource->Release();
        shaderResult->Release();

        (void)hr;
    }
}