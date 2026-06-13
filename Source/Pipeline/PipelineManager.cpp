#include "PipelineManager.h"

#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "PipelineElements.h"
#include "Shader.h"

#include "Logger.h"

#include <cassert>

void PipelineManager::Initialize(ID3D12Device* device, Logger* logger) {
    //====================
    // 3Dオブジェクト用PSO
    //====================
    {   // === 標準的な3Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->Initialize(device, logger, RootSignature::Type::Skinny3D);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        
        // InputLayOutをローカル変数へ移行
        auto inputLayout = PipelineElements::CreateDefaultInputLayout(); 

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateDefaultRasterizer(),
            PipelineElements::CreateDefaultDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline("Object3dOpaque", std::move(rootSignature), std::move(pipeline));
    }

    {   // === 半透明の3Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->Initialize(device, logger, RootSignature::Type::Skinny3D);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();

        // InputLayOutをローカル変数へ移行
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateNoCullRasterizer(),
            PipelineElements::CreateAlphaDepthStencil(),
            PipelineElements::CreateAlphaBlendState()
        );

        // マネージャーに登録
        RegisterPipeline("Object3dAlpha", std::move(rootSignature), std::move(pipeline));
    }

    {   // === ワイヤーフレーム描画の3Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->Initialize(device, logger, RootSignature::Type::Skinny3D);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();

        // InputLayOutをローカル変数へ移行
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateDefaultRasterizer(),
            PipelineElements::CreateDefaultDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline("Object3dWireframe", std::move(rootSignature), std::move(pipeline));
    }

    //====================
    // 2Dオブジェクト用PSO
    //====================
    {   // === 不透明の2Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object2d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object2d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->Initialize(device, logger, RootSignature::Type::Skinny2D);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();

        // InputLayOutをローカル変数へ移行
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateNoCullRasterizer(),
            PipelineElements::Create2DDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline("Object2dOpaque", std::move(rootSignature), std::move(pipeline));
    }

}

void PipelineManager::RegisterPipeline(
    const std::string& name, 
    std::unique_ptr<RootSignature> rootSig, 
    std::unique_ptr<GraphicsPipeline> pipeline
) {
    // 既に同じ名前があれば警告
    assert(pipelines_.find(name) == pipelines_.end() && "そのパイプライン名は既に登録されています");
    
    pipelines_[name] = PipelineSet{ std::move(rootSig), std::move(pipeline) };
}

const PipelineSet* PipelineManager::GetPipeline(const std::string& name) const {
    auto it = pipelines_.find(name);
    if (it != pipelines_.end()) {
        return &it->second;
    }
    
    // 見つからなければアサートで即座にバグを発見できるようにする
    assert(false && "指定された名前のパイプラインは存在しません");
    return nullptr;
}