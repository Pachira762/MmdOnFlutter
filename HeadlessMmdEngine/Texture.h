#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "DxContext.h"

namespace headless_mmd {

class Texture {
public:
	Texture() = default;
	Texture(const Texture&) = default;
	Texture(Texture&&) = default;
	Texture(const std::wstring& name);
	Texture(const std::wstring& name, IDXResourcePtr resource, D3D12_GPU_DESCRIPTOR_HANDLE descriptor);

	bool Load(DxContext* context, const std::wstring& path);
	bool Load(DxContext* context, const void* data, int width, int height, int pitch, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	static std::vector<std::shared_ptr<Texture>> LoadTextures(DxContext* context, const std::vector<std::wstring>& pathes, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON);
	static void CreateDummyTexture(DxContext* context);

private:
	bool LoadInternal(DxContext* context, const void* data, int width, int height, int pitch, DXGI_FORMAT format);

	std::wstring name_{};
	IDXResourcePtr resource_{};
	D3D12_GPU_DESCRIPTOR_HANDLE descriptor_{};

	static inline std::shared_ptr<Texture> dummy_texture_{};

public:
	IDXResource* GetResource() const {
		return resource_.Get();
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptor() const {
		return descriptor_;
	}

	const std::wstring& GetName() const {
		return name_;
	}

	static std::shared_ptr<Texture> GetDummyTexture() {
		return dummy_texture_;
	}
};

}