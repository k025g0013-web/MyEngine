#pragma once

namespace Kizuna {

    /// <summary>
    /// Rasterizerの設定
    /// </summary>
    enum class RasterizerMode {
        Default,
        NoCull,
        Wireframe,
    };

    /// <summary>
    /// DepthStencilの設定
    /// </summary>
    enum class DepthMode {
        Default,
        Alpha,
        ThroughWall,
        Disable,
    };

    /// <summary>
    /// Blendの設定
    /// </summary>
    enum class BlendMode {
        Default,
        Alpha,
        Add,
        Subtract,
        Multiply,
        Screen,
    };
    
    /// <summary>
    /// InputLayoutの設定
    /// </summary>
    enum class InputLayoutMode {
        Default,
        Model3d,
    };

    /// <summary>
    /// Pipeline生成に使用する設定
    /// </summary>
    /// <remarks>
    /// ImGuiなどから設定を変更し、
    /// PipelineListでPSOを再生成するために使用する。
    /// </remarks>
    struct PipelineConfig {
        /// Rasterizer設定
        RasterizerMode rasterizer = RasterizerMode::Default;

        /// DepthStencil設定
        DepthMode depth = DepthMode::Default;

        /// Blend設定
        BlendMode blend = BlendMode::Default;

        /// InputLayout設定
        InputLayoutMode inputLayout = InputLayoutMode::Default;
    };

}