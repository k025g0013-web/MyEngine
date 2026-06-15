#pragma once

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxcompiler.lib")

#include <wrl.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <string>

class Logger;

class Shader {
public:
    // DXCコンパイラ全体の初期化
    static void InitializeCompiler(Logger *logger);

    // 特定のシェーダーファイルをコンパイルして自身のBlobに保持
    void Compile(const std::wstring &filePath, const wchar_t *profile);

    // === getter ===
    IDxcBlob *GetBlob() const { return shaderBlob_.Get(); }
    D3D12_SHADER_BYTECODE GetBytecode() const {
        return { shaderBlob_->GetBufferPointer(), shaderBlob_->GetBufferSize() };
    }

private:
    // === 共通基盤 ===
    static Logger *sLogger_;
    static Microsoft::WRL::ComPtr<IDxcUtils> sDxcUtils_;
    static Microsoft::WRL::ComPtr<IDxcCompiler3> sDxcCompiler_;
    static IDxcIncludeHandler *sIncludeHandler_;

    // === 固有のデータ ===
    Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob_;
};