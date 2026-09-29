#pragma once
#include "PipelineList.h"

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

namespace Kizuna {

    class Logger;
    class PipelineList;

    class PipelineManager {
    public:
        PipelineManager();
        ~PipelineManager();

        PipelineManager(const PipelineManager &) = delete;
        PipelineManager &operator=(const PipelineManager &) = delete;

        void Initialize(
            ID3D12Device *device,
            Logger *logger
        );

        const PipelineSet *GetPipeline(
            PipelineType type
        ) const;

        PipelineConfig &GetConfig(
            PipelineType type
        );

        void RebuildPipeline(
            PipelineType type
        );

    private:
        std::unique_ptr<PipelineList> pipelineList_;
    };

}