#include "ThroughWallRenderer.h"
#include "Object/Object3D.h"
#include "Utils/ColorHelper.h"

#include <iostream>

void ThroughWallRenderer::AddObject(Object3D *object, uint32_t color) {
	ThroughWallObject data;

	data.object = object;
	data.color = UintToVector4(color);

	objects_.push_back(data);
}

void ThroughWallRenderer::Draw(
	ID3D12GraphicsCommandList *commandList, TextureData &texture) {
    for (auto &data : objects_) {
        MaterialData *material =
            data.object->GetThroughWallMaterial().GetMaterialData();

        material->color = data.color;

        data.object->DrawThroughWall(commandList, texture);
    }
}