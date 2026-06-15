#pragma once

#include <string>

#include "Asset/ModelLoader.h"

#include "Graphics/Resource/MeshBuffer.h"
#include "Graphics/Resource/Texture.h"
#include "RenderCore/Camera.h"
#include "RenderCore/Material.h"
#include "RenderCore/Mesh.h"
#include "Renderer/Renderer.h"


class Object3D {
public:
    // 形状生成(実質Initialize)
    void CreatePlaneTriangle(   // 平面三角形
        ID3D12Device *device,
        Vector3 left, Vector3 top, Vector3 right,
        uint32_t color, bool enableLighting
    );

    void CreateSphere(
        ID3D12Device *device,
        uint32_t subdivision,
        uint32_t color, bool enableLighting
    );

    void CreateModel(           // モデル
        ID3D12Device *device,
        const std::string &directoryPath, const std::string &fileName,
        uint32_t color, bool enableLighting
    );

    void Update(Camera *camera, Transform transform);

    void Draw(ID3D12GraphicsCommandList *commandList, Texture &texture);

    // getter
    ModelData &GetModelData() { return modelData_; }
    Material &GetMaterial() { return material_; }

private:
    ModelData modelData_;

    Renderer render_;
    Material material_;

    Mesh mesh_;
    std::vector<VertexData> vertices_;
    std::vector<uint32_t> indices_;

    MeshBuffer transformationMatrixBuffer_;
    TransformationMatrix *transformationMatrixData_ = nullptr;
};