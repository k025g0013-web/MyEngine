#include "PipelineList.h"

#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "PipelineElements.h"
#include "Graphics/Resource/Shader.h"

#include "Core/Logger.h"

#include <cassert>

namespace Kizuna {

    void PipelineList::Initialize(
        ID3D12Device *device,
        Logger *logger
    ) {
        device_ = device;
        logger_ = logger;

        //====================
        // 初期Config
        //====================

        configs_[PipelineType::Object3dOpaque] = {
            .rasterizer = RasterizerMode::Default,
            .depth = DepthMode::Default,
            .blend = BlendMode::Default,
            .inputLayout = InputLayoutMode::Default
        };

        configs_[PipelineType::Object3dAlpha] = {
            .rasterizer = RasterizerMode::NoCull,
            .depth = DepthMode::Alpha,
            .blend = BlendMode::Alpha,
            .inputLayout = InputLayoutMode::Default
        };

        configs_[PipelineType::Object3dWireframe] = {
            .rasterizer = RasterizerMode::Wireframe,
            .depth = DepthMode::Default,
            .blend = BlendMode::Default,
            .inputLayout = InputLayoutMode::Default
        };

        configs_[PipelineType::Object3dThroughWall] = {
            .rasterizer = RasterizerMode::Default,
            .depth = DepthMode::ThroughWall,
            .blend = BlendMode::Alpha,
            .inputLayout = InputLayoutMode::Default
        };

        configs_[PipelineType::Object2dOpaque] = {
            .rasterizer = RasterizerMode::NoCull,
            .depth = DepthMode::Disable,
            .blend = BlendMode::Default,
            .inputLayout = InputLayoutMode::Default
        };

        configs_[PipelineType::Model3dOpaque] = {
            .rasterizer = RasterizerMode::Default,
            .depth = DepthMode::Default,
            .blend = BlendMode::Default,
            .inputLayout = InputLayoutMode::Model3d
        };


        //====================
        // Pipeline生成
        //====================

        CreateObject3dOpaque(device, logger);
        CreateObject3dAlpha(device, logger);
        CreateObject3dWireframe(device, logger);
        CreateObject3dThroughWall(device, logger);

        CreateObject2dOpaque(device, logger);

        CreateModel3dOpaque(device, logger);
    }


    //==================================================
    // 3D Object
    //==================================================

    void PipelineList::CreateObject3dOpaque(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================

        PipelineConfig &config =
            configs_[PipelineType::Object3dOpaque];

        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Object3d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/Object3d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny3D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Object3dOpaque] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    void PipelineList::CreateObject3dAlpha(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================

        PipelineConfig &config =
            configs_[PipelineType::Object3dAlpha];

        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Object3d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/Object3d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny3D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Object3dAlpha] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    void PipelineList::CreateObject3dWireframe(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================

        PipelineConfig &config =
            configs_[PipelineType::Object3dWireframe];

        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Object3d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/Object3d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny3D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Object3dWireframe] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    void PipelineList::CreateObject3dThroughWall(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================


        PipelineConfig &config =
            configs_[PipelineType::Object3dThroughWall];


        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Object3d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/ThroughWall3d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny3D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Object3dThroughWall] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    //==================================================
    // 2D Object
    //==================================================

    void PipelineList::CreateObject2dOpaque(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================


        PipelineConfig &config =
            configs_[PipelineType::Object2dOpaque];


        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Object2d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/Object2d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny2D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Object2dOpaque] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    //==================================================
    // Model
    //==================================================

    void PipelineList::CreateModel3dOpaque(
        ID3D12Device *device,
        Logger *logger
    ) {
        //====================
        // Config
        //====================

        PipelineConfig &config =
            configs_[PipelineType::Model3dOpaque];


        //====================
        // HLSL読み込み
        //====================

        Shader vs, ps;

        vs.Compile(
            L"Resources/Shader/Model3d.VS.hlsl",
            L"vs_6_0"
        );

        ps.Compile(
            L"Resources/Shader/Model3d.PS.hlsl",
            L"ps_6_0"
        );


        //====================
        // RootSignature生成
        //====================

        auto rootSignature =
            std::make_unique<RootSignature>();

        rootSignature->CreateSkinny3D(
            device,
            logger
        );


        //====================
        // Pipeline生成
        //====================

        auto pipeline =
            std::make_unique<GraphicsPipeline>();

        auto inputLayout =
            PipelineElements::CreateInputLayout(
                config.inputLayout
            );

        pipeline->Initialize(
            device,
            rootSignature->GetRootSignature(),
            vs,
            ps,
            inputLayout,
            PipelineElements::CreateRasterizer(
                config.rasterizer
            ),
            PipelineElements::CreateDepthStencil(
                config.depth
            ),
            PipelineElements::CreateBlendState(
                config.blend
            )
        );


        //====================
        // Pipeline登録
        //====================

        pipelines_[PipelineType::Model3dOpaque] =
        {
            std::move(rootSignature),
            std::move(pipeline)
        };
    }


    //==================================================
    // Get Pipeline
    //==================================================

    const PipelineSet *PipelineList::GetPipeline(
        PipelineType type
    ) const {
        auto it = pipelines_.find(type);

        if (it != pipelines_.end()) {
            return &it->second;
        }

        assert(
            false &&
            "指定されたパイプラインは存在しません"
        );

        return nullptr;
    }

    PipelineConfig &PipelineList::GetConfig(
        PipelineType type
    ) {
        return configs_.at(type);
    }

    void PipelineList::RebuildPipeline(
        PipelineType type
    ) {
        switch (type) {

        case PipelineType::Object3dOpaque:
            CreateObject3dOpaque(device_, logger_);
            break;

        case PipelineType::Object3dAlpha:
            CreateObject3dAlpha(device_, logger_);
            break;

        case PipelineType::Object3dWireframe:
            CreateObject3dWireframe(device_, logger_);
            break;

        case PipelineType::Object3dThroughWall:
            CreateObject3dThroughWall(device_, logger_);
            break;

        case PipelineType::Object2dOpaque:
            CreateObject2dOpaque(device_, logger_);
            break;

        case PipelineType::Model3dOpaque:
            CreateModel3dOpaque(device_, logger_);
            break;
        }
    }


}