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

// RootSignatureとPSOを管理する構造体
struct PipelineSet {
    std::unique_ptr<RootSignature> rootSignature;
    std::unique_ptr<GraphicsPipeline> pipeline;
};

class PipelineManager {
public:
    PipelineManager();
    ~PipelineManager();

    // コピー禁止
    PipelineManager(const PipelineManager &) = delete;
    PipelineManager &operator=(const PipelineManager &) = delete;

    void Initialize(ID3D12Device *device, Logger *logger);

    // 外部追加
    void RegisterPipeline(
        const std::string &name,
        std::unique_ptr<RootSignature> rootSig,
        std::unique_ptr<GraphicsPipeline> pipeline
    );

    // 描画時に名前を指定してパイプラインのセットを取得する
    const PipelineSet *GetPipeline(const std::string &name) const;

private:
    // パイプライン群を文字列（キー）で一括管理
    std::unordered_map<std::string, PipelineSet> pipelines_;
};