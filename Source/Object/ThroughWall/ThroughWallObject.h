#pragma once

#include "ThroughWallStyle.h"

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

#include <cstdint>

#include "Math/Vector.h"

namespace Kizuna {

    class Object3D;

    /// <summary>
    /// 壁越し描画を行うオブジェクト
    /// </summary>
    class ThroughWallObject {
    public:

        ThroughWallObject(
            Object3D *object,
            Vector4 color,
            Style style
        )
            : object_(object),
            color_(color),
            style_(style) {}

        /// <summary>
        /// 壁越し描画を行う
        /// </summary>
        void Draw(ID3D12GraphicsCommandList *commandList);

        Object3D *GetObject() const {
            return object_;
        }

        Vector4 &GetColor() {
            return color_;
        }

        Style GetStyle() const {
            return style_;
        }

        void SetStyle(Style style) {
            style_ = style;
        }

    private:

        // 壁越し描画対象
        Object3D *object_ = nullptr;

        // 壁越し描画色
        Vector4 color_{};

        // 壁越し描画スタイル
        Style style_ = Style::Solid;
    };
}