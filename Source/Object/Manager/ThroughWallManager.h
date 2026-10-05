#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>
#include <memory>
#include <cassert>

#include "Object/ThroughWall/ThroughWallObject.h"
#include "Object/ThroughWall/ThroughWallStyle.h"

namespace Kizuna {

    class Object3D;

    /// <summary>
    /// 壁越し描画専用の描画管理クラス
    /// </summary>
    /// <remarks>
    /// 壁越し描画を行うオブジェクトを登録・管理する。
    /// 実際の壁越し描画処理は ThroughWallObject が担当する。
    /// </remarks>
    class ThroughWallManager {
    public:

        /// <summary>
        /// 壁越し描画の対象オブジェクトを登録する
        /// </summary>
        /// <param name="object">
        /// 登録する3Dオブジェクト
        /// </param>
        /// <param name="color">
        /// 壁越し描画時に使用する色（0xRRGGBBAA形式）
        /// </param>
        /// <param name="style">
        /// 壁越し描画時のスタイル
        /// </param>
        void AddObject(
            Object3D *object,
            uint32_t color,
            Style style
        );

        /// <summary>
        /// 壁越し描画の対象からオブジェクトを削除する
        /// </summary>
        void RemoveObject(Object3D *object);

        /// <summary>
        /// 登録されているオブジェクトを壁越し描画する
        /// </summary>
        void Draw(ID3D12GraphicsCommandList *commandList);

        ThroughWallObject &GetObject(size_t index) {
            assert(index < objects_.size());
            return *objects_[index];
        }

        bool Empty() const {
            return objects_.empty();
        }

    private:

        /// 壁越し描画対象の一覧
        std::vector<std::unique_ptr<ThroughWallObject>> objects_;
    };
}