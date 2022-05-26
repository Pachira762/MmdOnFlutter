#pragma once
#include <functional>
#include "DxCore.h"
#include "DescriptorHeap.h"
#include "RenderOutput.h"

namespace headless_mmd {

class DxContextBase {
public:
    virtual ~DxContextBase() = default;

    virtual bool Init(HWND hwnd);

    virtual IDXGraphicsCommandList* BeginDraw();
    virtual void EndDraw(IDXGraphicsCommandList* command_list);

    virtual bool Execute(std::function<bool(IDXGraphicsCommandList* command_list)>&& command_func, bool wait = true);
    virtual void Flush();

protected:

    std::unique_ptr<DxCore>          core_ = std::make_unique<DxCore>();
    std::unique_ptr<DescriptorHeap>  srv_heap_ = std::make_unique<DescriptorHeap>();
    std::unique_ptr<DescriptorHeap>  rtv_heap_ = std::make_unique<DescriptorHeap>();
    std::unique_ptr<DescriptorHeap>  dsv_heap_ = std::make_unique<DescriptorHeap>();
    std::unique_ptr<RenderOutput>    output_ = std::make_unique<RenderOutput>();

public:
    IDXDevice* GetDevice() {
        return core_->GetDevice();
    }

    IDXDescriptorHeap* GetDescriptorHeap() const {
        return srv_heap_->GetDescriptorHeap();
    }

    DXGI_FORMAT GetRenderTargetFormat() const {
        return RenderOutput::RenderTargetFormat;
    }

    DXGI_FORMAT GetDepthStencilFormat() const {
        return RenderOutput::DepthStencilFormat;
    }

    std::tuple<int, int> GetRenderTargetSize() const {
        return output_->GetRenderTargetSize();
    }
};

}