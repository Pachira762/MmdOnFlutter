#pragma once
#include <memory>
#include "Common.h"
#include "Texture.h"

namespace headless_mmd {

struct Material {
	int index;
	std::shared_ptr<Texture> texture;
};

class MaterialSet {
public:

private:
	struct MaterialConstants {
		Float4 diffuse[256];
	};

	std::vector<std::shared_ptr<Material>> materials_{};
	IDXResourcePtr constant_buffer_{};

	friend class MaterialImporter;

public:
	D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferLocation() const {
		return constant_buffer_->GetGPUVirtualAddress();
	}

	int GetNumMaterials() const {
		return static_cast<int>(materials_.size());
	}

	std::shared_ptr<Material> GetMaterial(int index) const {
		return materials_.at(index);
	}
};

class MaterialImporter {
public:
	std::shared_ptr<MaterialSet> Import(DxContext* context, const Pmx& pmx, const std::wstring& model_path);
};

}

