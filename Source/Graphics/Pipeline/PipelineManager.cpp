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
    {   // === 壁越し3Dオブジェクト用Pipeline ===
        // 通常の3Dモデル描画で使用するPipeline。
        // DepthStencilによる奥行き判定と標準的なBlendStateを使用する。
        
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
        // アルファ値を利用した透過描画用Pipeline。
        // Depthへの書き込みを無効化し、BlendStateによって
        // 背景と描画結果を合成する。
        
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
        // モデルの形状確認やデバッグ表示向け。
        // Rasterizerで塗りつぶしを無効化し、辺のみを描画する。
        
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
        // 壁などの遮蔽物の奥に存在するオブジェクトを描画するためのPipeline。
        // Depth比較をGREATERに変更し、通常描画では隠れる奥側のオブジェクトのみ描画する。
        // PixelShaderでは壁越し専用の表現を適用する。

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
        // スプライトなどの2D描画用Pipeline。
        // 奥行き判定を使用せず、画面上へ直接描画する。

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
        // ModelLoaderなどで読み込んだ外部モデルを描画するためのPipeline。
        // 専用InputLayoutを使用し、通常の3D描画設定で表示する。

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
    // 同じ種類のPipelineが存在すると設定が上書きされるため防止する
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