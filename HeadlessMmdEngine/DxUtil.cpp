#include "pch.h"
#include "DxUtil.h"

namespace headless_mmd {

IDXBlobPtr CompileShader(const std::wstring& path, const char* entry, const char* target) {
    IDXBlobPtr code{}, error{};
    auto hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, IsDebug ? D3DCOMPILE_DEBUG : 0, 0, &code, &error);
    if (FAILED(hr)) {
        if (error && error->GetBufferPointer()) OutputDebugStringA((const char*)error->GetBufferPointer());
        // return {};
    }
    return code;
}

std::tuple<IDXBlobPtr, IDXBlobPtr> CompilePass(const std::wstring& path) {
    return {
        CompileShader(path, "MainVS", "vs_5_0"),
        CompileShader(path, "MainPS", "ps_5_0"),
    };
}

}