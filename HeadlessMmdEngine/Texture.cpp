#include "pch.h"
#include "Texture.h"
#include "DirectXTex.h"
#include <wincodec.h>

namespace {

std::wstring GetExtension(const std::filesystem::path& path) {
	auto extension = path.extension().native();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);
	return extension;
}

std::tuple<int, int> CalcPitchAndSlice(const DirectX::TexMetadata& meta) {
	std::size_t pitch = 0, slice = 0;
	if (FAILED(DirectX::ComputePitch(meta.format, meta.width, meta.height, pitch, slice))) {
		DLOG(L"Failed cumpute pitch fmt:{} size:{}, {}", (unsigned)meta.format, meta.width, meta.height);
		return {};
	}

	return { static_cast<int>(pitch), static_cast<int>(slice) };
}

bool OpenImage(const std::filesystem::path& path, DirectX::ScratchImage& image, DirectX::TexMetadata& metadata) {
	HRESULT hr = E_FAIL;
	if (const auto extension = GetExtension(path); extension.compare(L".tga") == 0) {
		hr = DirectX::LoadFromTGAFile(path.c_str(), &metadata, image);
		DLOG(L"Load {} format:{}", extension, (int)metadata.format);
	}
	else if (extension.compare(L".ddx") == 0) {
		hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, image);
		DLOG(L"Load {} format:{}", extension, (int)metadata.format);
	}
	else {
		hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, image);
		DLOG(L"Load {} format:{}", extension, (int)metadata.format);
	}

	return SUCCEEDED(hr);
}

}

namespace headless_mmd {

Texture::Texture(const std::wstring& name) :
	name_(name) {
}

Texture::Texture(const std::wstring& name, IDXResourcePtr resource, D3D12_GPU_DESCRIPTOR_HANDLE descriptor) :
	name_(name),
	resource_(resource),
	descriptor_(descriptor) {
}

bool Texture::Load(DxContext* context, const std::wstring& path) {
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	if (!OpenImage(path, image, metadata)) {
		return false;
	}

	auto width = static_cast<int>(metadata.width);
	auto height = static_cast<int>(metadata.height);
	auto [pitch, slice] = CalcPitchAndSlice(metadata);
	return LoadInternal(context, image.GetPixels(), width, height, pitch, metadata.format);
}

bool Texture::Load(DxContext* context, const void* data, int width, int height, int pitch, DXGI_FORMAT format) {
	return LoadInternal(context, data, width, height, pitch, format);
}

bool Texture::LoadInternal(DxContext* context, const void* data, int width, int height, int pitch, DXGI_FORMAT format) {
	resource_ = context->CreateTexture(width, height, data, pitch, format);
	if (!resource_) {
		return false;
	}

	descriptor_ = context->CreateShaderResourceViewTex2D(resource_.Get(), format);

	return true;
}

std::vector<std::shared_ptr<Texture>> Texture::LoadTextures(DxContext* context, const std::vector<std::wstring>& pathes, D3D12_RESOURCE_STATES state) {
	const auto num_textures = static_cast<int>(pathes.size());
	std::vector<IDXResourcePtr> resources(num_textures), uploads(num_textures);
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> descriptors(num_textures);

	for (int i = 0; i < num_textures; ++i) {
		DirectX::ScratchImage image;
		DirectX::TexMetadata metadata;
		if (!OpenImage(pathes[i], image, metadata)) {
			DLOG(L"Failed to open {}", pathes[i]);
			continue;
		}

		auto width = static_cast<int>(metadata.width);
		auto height = static_cast<int>(metadata.height);
		auto [pitch, slice] = CalcPitchAndSlice(metadata);
		auto desc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, width, height, 1, 1, 1, 0);

		uploads[i] = context->CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0), D3D12_HEAP_FLAG_NONE, desc, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!uploads[i]) {
			DLOG(L"Failed to create upload#{}", i);
			continue;
		}

		if (FAILED(uploads[i]->Map(0, nullptr, nullptr))) {
			DLOG(L"Failed to map upload#{}", i);
			continue;
		}

		if (FAILED(uploads[i]->WriteToSubresource(0, nullptr, image.GetPixels(), pitch, slice))) {
			DLOG(L"Failed to write subresource upload#{}", i);
			continue;
		}
		uploads[i]->Unmap(0, nullptr);

		resources[i] = context->CreateResource(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, desc, D3D12_RESOURCE_STATE_COPY_DEST);
		if (!resources[i]) {
			DLOG(L"Failed to create texture resource#{}", i);
			continue;
		}

		descriptors[i] = context->CreateShaderResourceViewTex2D(resources[i].Get(), metadata.format);
	}

	context->Execute([num_textures, state, &resources, &uploads, &descriptors](IDXGraphicsCommandList* command_list) ->bool {
		for (int i = 0; i < num_textures; ++i) {
			if (resources[i]) {
				command_list->CopyResource(resources[i].Get(), uploads[i].Get());

				auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resources[i].Get(), D3D12_RESOURCE_STATE_COPY_DEST, state);
				command_list->ResourceBarrier(1, &barrier);
			}
		}
		return true;
	}, true);

	std::vector<std::shared_ptr<Texture>> textures(num_textures);
	for (int i = 0; i < num_textures; ++i) {
		if (resources[i]) {
			textures[i] = std::make_shared<Texture>(pathes[i], resources[i], descriptors[i]);
		}
		else {
			textures[i] = GetDummyTexture();
		}
	}

	return textures;
}

void Texture::CreateDummyTexture(DxContext* context) {
	dummy_texture_ = std::make_shared<Texture>(L"Dummy Texture");

	int width = 256;
	int height = 256;
	std::vector<uint8_t> buf(4 * width * height);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			bool check = ((x / 16) % 2) == ((y / 16) % 2);
			uint8_t c = check ? 0xff : 0;
			buf[4 * (width * y + x) + 0] = c;
			buf[4 * (width * y + x) + 1] = c;
			buf[4 * (width * y + x) + 2] = c;
			buf[4 * (width * y + x) + 3] = c;
		}
	}

	dummy_texture_->Load(context, buf.data(), width, height, 4 * width);
}

}