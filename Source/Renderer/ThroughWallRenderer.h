#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>

#include "Math/Vector.h"

class Object3D;
struct TextureData;

struct ThroughWallObject {
    Object3D *object;

    Vector4 color;
};

class ThroughWallRenderer {
public:

    void AddObject(Object3D *object, uint32_t color);

    void Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture);

private:
    std::vector<ThroughWallObject> objects_;
};