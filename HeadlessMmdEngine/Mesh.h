#pragma once
#include <memory>
#include "Material.h"

namespace headless_mmd {

struct Mesh {
	class SkinnedMesh* parent_;

	int index_start_{};
	int num_indices_{};
	std::shared_ptr<Material> material_;

	const SkinnedMesh* GetParent() const {
		return parent_;
	}

	int GetNumIndices() const {
		return num_indices_;
	}

	int GetStartIndex() const {
		return index_start_;
	}

	int GetMaterialIndex() const {
		return material_->index;
	}

	std::shared_ptr<Texture> GetTexture() const {
		return material_->texture;
	}
};

}