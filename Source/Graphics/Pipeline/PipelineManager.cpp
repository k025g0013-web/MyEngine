#include "PipelineManager.h"

#include "PipelineList.h"

namespace Kizuna {

    PipelineManager::PipelineManager() = default;
    PipelineManager::~PipelineManager() = default;


    void PipelineManager::Initialize(
        ID3D12Device *device,
        Logger *logger
    ) {
        pipelineList_ =
            std::make_unique<PipelineList>();

        pipelineList_->Initialize(
            device,
            logger
        );
    }


    const PipelineSet *PipelineManager::GetPipeline(
        PipelineType type
    ) const {
        return pipelineList_->GetPipeline(type);
    }


    PipelineConfig &PipelineManager::GetConfig(
        PipelineType type
    ) {
        return pipelineList_->GetConfig(type);
    }


    void PipelineManager::RebuildPipeline(
        PipelineType type
    ) {
        pipelineList_->RebuildPipeline(type);
    }
}