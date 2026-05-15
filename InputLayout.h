#pragma once
#include <d3d12.h>

#include <vector>

class InputLayout {
public:
    void Initialize();

    // getter
    const D3D12_INPUT_LAYOUT_DESC &GetDesc() const {return inputLayoutDesc_;}

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs_;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
};