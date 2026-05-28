#include "Triangle.h"

#include "MathFunctions.h"

void Triangle::Initialize(
    ID3D12Device *device,
    Vector3 left, Vector3 top, Vector3 right,
    uint32_t color
) {
    render_.CreateTriangleVertices(vertices_, left, top, right);

    render_.CreateVertexBuffer(device, vertexBuffer_, vertices_);

    render_.CreateTransformationMatrixBuffer(
        device, transformationMatrixBuffer_, transformationMatrixData_);

    material_.Initialize(device, color, true);
}

void Triangle::Update(Camera *camera) {
    Matrix4x4 worldMatrix = MakeWorldMatrix(transform_);

    Matrix4x4 worldViewProjectionMatrix =
        Multiply(worldMatrix, camera->GetViewProjectionMatrix());

    transformationMatrixData_->WVP = worldViewProjectionMatrix;
    transformationMatrixData_->World = worldMatrix;
}

void Triangle::Draw(ID3D12GraphicsCommandList *commandList, Texture &texture) {
    commandList->SetGraphicsRootConstantBufferView(0, material_.GetGPUVirtualAddress());

    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixBuffer_.GetGPUVirtualAddress());

    commandList->SetGraphicsRootDescriptorTable(2, texture.GetGPUHandle());

    commandList->IASetVertexBuffers(0, 1, &vertexBuffer_.GetView());

    // 描画!(DrawCall/ドローコール)
    commandList->DrawInstanced(3, 1, 0, 0);
}