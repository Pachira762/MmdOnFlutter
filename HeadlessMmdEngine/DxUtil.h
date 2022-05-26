#pragma once
#include <tuple>
#include <d3dcompiler.h>
#include "DxClasses.h"

#define RETURN_IF_FAILED(hr, message) if(FAILED(hr)) { ELOG(message); return false; }

namespace headless_mmd {

IDXBlobPtr CompileShader(const std::wstring& path, const char* entry, const char* target);

std::tuple<IDXBlobPtr, IDXBlobPtr> CompilePass(const std::wstring& path);

inline D3D12_SHADER_BYTECODE ShaderBytecode(IDXBlobPtr& blob) {
	return {
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
	};
}

}