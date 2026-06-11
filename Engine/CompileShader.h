#pragma once
#include <wrl.h>
#include <string> 

#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")

class Shader;
class Logger;

class CompileShader {
public:
    // dxcCompiler初期化
    void Initialize(Logger* logger);

    // CompileShader
    Shader Compile(const std::wstring& filePath, const wchar_t* profile);

private:
    Logger* logger_ = nullptr;

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
};