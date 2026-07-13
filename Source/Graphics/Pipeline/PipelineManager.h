#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <memory>

class RootSignature;
class GraphicsPipeline;
class Shader;
class Logger;

/// <summary>
/// 描画パイプラインの種類を管理する列挙型
/// </summary>
/// <remarks>
/// 使用するRootSignatureやGraphicsPipelineの種類を識別するために使用する。
/// 不透明描画、半透明描画、壁越し描画など用途ごとのパイプラインを管理する。
/// </remarks>
enum class PipelineType {
    Object3dOpaque,
    Object3dAlpha,
    Object3dWireframe,
    Object3dThroughWall,    // 壁越しにあるオブジェクト描画

    Object2dOpaque,

    Model3dOpaque,
};

/// <summary>
/// RootSignatureとGraphicsPipelineをまとめて管理する構造体
/// </summary>
/// <remarks>
/// 描画に必要なRootSignatureとGraphicsPipelineStateObject(PSO)を
/// 1つのセットとして保持する。
/// PipelineManagerでPipelineTypeごとに管理される。
/// </remarks>
struct PipelineSet {
    /// RootSignature管理
    std::unique_ptr<RootSignature> rootSignature;

    /// GraphicsPipeline管理
    std::unique_ptr<GraphicsPipeline> pipeline;
};

/// <summary>
/// DirectX12の描画パイプラインを管理するクラス
/// </summary>
/// <remarks>
/// RootSignatureとGraphicsPipelineStateObject(PSO)を
/// PipelineTypeごとに管理する。
///
/// 描画時には指定されたPipelineTypeから対応するパイプラインを取得し、
/// 不透明描画、透過描画、ワイヤーフレーム描画、壁越し描画など
/// 描画用途に応じた設定へ切り替える。
/// </remarks>
class PipelineManager {
public:

    /// <summary>
    /// PipelineManagerを生成する
    /// </summary>
    PipelineManager();


    /// <summary>
    /// PipelineManagerを破棄する
    /// </summary>
    /// <remarks>
    /// 管理しているRootSignatureやGraphicsPipelineは
    /// unique_ptrによって自動的に解放される。
    /// </remarks>
    ~PipelineManager();


    /// <summary>
    /// コピーコンストラクタを禁止する
    /// </summary>
    /// <remarks>
    /// GPUリソースを管理するクラスのため、
    /// コピーによる所有権の重複を防止する。
    /// </remarks>
    PipelineManager(const PipelineManager &) = delete;


    /// <summary>
    /// コピー代入演算子を禁止する
    /// </summary>
    /// <remarks>
    /// GPUリソース管理の競合を防止するため、
    /// コピー代入を無効化する。
    /// </remarks>
    PipelineManager &operator=(const PipelineManager &) = delete;


    /// <summary>
    /// 描画パイプライン管理を初期化する
    /// </summary>
    /// <param name="device">DirectX12デバイス</param>
    /// <param name="logger">ログ出力クラス</param>
    /// <remarks>
    /// RootSignatureやGraphicsPipelineを生成し、
    /// 使用可能な描画パイプラインを登録する。
    /// </remarks>
    void Initialize(ID3D12Device *device, Logger *logger);


    /// <summary>
    /// 描画パイプラインを登録する
    /// </summary>
    /// <param name="type">登録するパイプラインの種類</param>
    /// <param name="rootSig">使用するRootSignature</param>
    /// <param name="pipeline">使用するGraphicsPipeline</param>
    /// <remarks>
    /// PipelineTypeとRootSignature、GraphicsPipelineの
    /// 対応関係を管理リストへ追加する。
    /// </remarks>
    void RegisterPipeline(
        PipelineType type,
        std::unique_ptr<RootSignature> rootSig,
        std::unique_ptr<GraphicsPipeline> pipeline
    );


    /// <summary>
    /// 指定したパイプラインセットを取得する
    /// </summary>
    /// <param name="type">取得するパイプラインの種類</param>
    /// <returns>PipelineSetへのポインタ</returns>
    /// <remarks>
    /// 描画処理時に必要となるRootSignatureと
    /// GraphicsPipelineを取得するために使用する。
    /// </remarks>
    const PipelineSet *GetPipeline(PipelineType type) const;


private:

    /// PipelineTypeをキーとして管理するパイプライン一覧
    std::unordered_map<PipelineType, PipelineSet> pipelines_;
};