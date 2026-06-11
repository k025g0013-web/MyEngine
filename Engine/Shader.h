#pragma once
#include <wrl.h>
#include <dxcapi.h>

class Shader {
public:
    void SetBlob(Microsoft::WRL::ComPtr<IDxcBlob> blob);

    // getter
    IDxcBlob *GetBlob() const {return shaderBlob_.Get();}

private:
    Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob_;
};