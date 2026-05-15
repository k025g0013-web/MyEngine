#include "Shader.h"

void Shader::SetBlob(Microsoft::WRL::ComPtr<IDxcBlob> blob) {
    shaderBlob_ = blob;
}