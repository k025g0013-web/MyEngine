#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <vector>

#include "Math/Vector.h"

class Object3D;
struct TextureData;

/// <summary>
/// 壁越し描画を行う対象オブジェクトの情報
/// </summary>
/// <remarks>
/// 壁越し描画時に使用するオブジェクト本体と、
/// 壁越し専用の描画色を保持する。
/// </remarks>
struct ThroughWallObject {

    /// 描画対象オブジェクト
    Object3D *object;

    /// 壁越し描画時に適用する色
    Vector4 color;
};

/// <summary>
/// 壁越し描画専用の描画管理クラス
/// </summary>
/// <remarks>
/// 壁越し描画を行うオブジェクトを登録・管理し、
/// 壁越し描画用パイプラインで一括描画を行う。
/// 通常描画とは分離することで描画順序を管理しやすくしている。
/// </remarks>
class ThroughWallRenderer {
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
    void AddObject(Object3D *object, uint32_t color);

    /// <summary>
    /// 登録された全オブジェクトを壁越し描画する
    /// </summary>
    /// <param name="commandList">
    /// 描画コマンドリスト
    /// </param>
    /// <param name="texture">
    /// 描画に使用するテクスチャ
    /// </param>
    /// <remarks>
    /// 登録済みオブジェクトへ壁越し用マテリアルを設定し、
    /// 壁越し描画専用の描画処理を実行する。
    /// </remarks>
    void Draw(ID3D12GraphicsCommandList *commandList, TextureData &texture);

private:

    /// 壁越し描画対象の一覧
    std::vector<ThroughWallObject> objects_;
};