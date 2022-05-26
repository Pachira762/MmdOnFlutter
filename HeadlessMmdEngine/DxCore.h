#pragma once
#include "DxClasses.h"

namespace headless_mmd {

class DxCore {
public:
    ~DxCore();

    bool Init();

    IDXGraphicsCommandList* BeginDirectCommand(IDXPipelineState* state = nullptr);
    bool ExececuteDirectCommand(bool wait = true);
    void WaitDirectCommand();

    IDXGraphicsCommandList* BeginCopyCommand();
    bool ExecuteCopyCommand(bool wait = true);
    void WaitCopyCommand();

private:
    struct CommandContext {
        IDXCommandQueuePtr          command_queue{};
        IDXCommandAllocatorPtr      command_allocator{};
        IDXGraphicsCommandListPtr   command_list{};
        
        IDXFencePtr fence{};
        HANDLE fence_event{};
        UINT64 fence_value{};
    };

    bool InitCommandContext(CommandContext& context, D3D12_COMMAND_LIST_TYPE type);
    void CloseCommandContext(CommandContext& context);

    bool BeginCommandList(CommandContext& context, IDXPipelineState* state);
    bool ExecuteCommandList(CommandContext& context, bool wait = true);
    void WaitCommandQueue(CommandContext& context);

    IDXDevicePtr device_{};
    CommandContext direct_command_context_{};
    CommandContext copy_command_context_{};

public:
    IDXDevice* GetDevice() const {
        return device_.Get();
    }

    IDXCommandQueue* GetDirectommandQueue() const {
        return direct_command_context_.command_queue.Get();
    }
};

}