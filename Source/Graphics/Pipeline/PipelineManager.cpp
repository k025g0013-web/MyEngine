#include "PipelineManager.h"

#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "PipelineElements.h"
#include "Graphics/Resource/Shader.h"

#include "Core/Logger.h"

#include <cassert>

PipelineManager::PipelineManager() = default;
PipelineManager::~PipelineManager() = default;

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
        rootSignature->CreateSkinny3D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateDefaultInputLayout(); 

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateDefaultRasterizer(),
            PipelineElements::CreateDefaultDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Object3dOpaque, std::move(rootSignature), std::move(pipeline));
    }

    {   // === 半透明の3Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->CreateSkinny3D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateNoCullRasterizer(),
            PipelineElements::CreateAlphaDepthStencil(),
            PipelineElements::CreateAlphaBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Object3dAlpha, std::move(rootSignature), std::move(pipeline));
    }

    {   // === ワイヤーフレーム描画の3Dオブジェクト用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Object3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->CreateSkinny3D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateWireframeRasterizer(),
            PipelineElements::CreateDefaultDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Object3dWireframe, std::move(rootSignature), std::move(pipeline));
    }

    {   // === 壁越し3Dオブジェクト用Pipeline ===
       // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Object3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/ThroughWall3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->CreateSkinny3D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateDefaultRasterizer(),
            PipelineElements::CreateThroughWallDepthStencil(),
            PipelineElements::CreateAlphaBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Object3dThroughWall, std::move(rootSignature), std::move(pipeline));
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
        rootSignature->CreateSkinny2D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateDefaultInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateNoCullRasterizer(),
            PipelineElements::Create2DDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Object2dOpaque, std::move(rootSignature), std::move(pipeline));
    }

    //====================
    // 外部モデル用PSO
    //====================
    {   // === 標準的な外部モデル用Pipeline ===
        // HLSL読み込み
        Shader vs, ps;
        vs.Compile(L"Resources/Shader/Model3d.VS.hlsl", L"vs_6_0");
        ps.Compile(L"Resources/Shader/Model3d.PS.hlsl", L"ps_6_0");

        // rootSignature生成
        auto rootSignature = std::make_unique<RootSignature>();
        rootSignature->CreateSkinny3D(device, logger);

        // Pipeline生成
        auto pipeline = std::make_unique<GraphicsPipeline>();
        auto inputLayout = PipelineElements::CreateModel3dInputLayout();

        pipeline->Initialize(
            device, rootSignature->GetRootSignature(), vs, ps, inputLayout,
            PipelineElements::CreateDefaultRasterizer(),
            PipelineElements::CreateDefaultDepthStencil(),
            PipelineElements::CreateDefaultBlendState()
        );

        // マネージャーに登録
        RegisterPipeline(PipelineType::Model3dOpaque, std::move(rootSignature), std::move(pipeline));
    }    
}

void PipelineManager::RegisterPipeline(
    PipelineType type,
    std::unique_ptr<RootSignature> rootSig, 
    std::unique_ptr<GraphicsPipeline> pipeline
) {
    // 既に同じ名前があれば警告
    assert(pipelines_.find(type) == pipelines_.end() && "そのパイプラインは既に登録されています");

    pipelines_[type] = PipelineSet{ std::move(rootSig), std::move(pipeline) };
}

const PipelineSet *PipelineManager::GetPipeline(PipelineType type) const {
    auto it = pipelines_.find(type);
    if (it != pipelines_.end()) {
        return &it->second;
    }

    assert(false && "指定されたパイプラインは存在しません");
    return nullptr;
}