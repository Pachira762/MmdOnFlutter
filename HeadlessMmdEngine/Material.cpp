#include "pch.h"
#include "Material.h"
#include "MathHelper.h"
#include <filesystem>

namespace headless_mmd {

template<typename Container, typename Index>
bool is_valid_index(const Container& container, Index index) {
	return (index >= 0) && (index < container.size());
}

std::shared_ptr<MaterialSet> MaterialImporter::Import(DxContext* context, const Pmx& pmx, const std::wstring& model_path) {
	auto material_set = std::make_shared<MaterialSet>();

	const auto num_materials = static_cast<int>(pmx.materials.size());
	if (num_materials > 256) {
		DLOG(L"Num of materials must less than 256");
		return nullptr;
	}

	std::vector<std::wstring> pathes{};
	const auto dir = std::filesystem::path(model_path).parent_path();
	for (auto& path_str : pmx.textures) {
		auto path = std::filesystem::path(path_str);
		if (path.is_absolute()) {
			pathes.emplace_back(path.wstring());
		}
		else {
			pathes.emplace_back(std::filesystem::absolute(dir / path).wstring());
		}
	}
	auto textures = Texture::LoadTextures(context, pathes);

	{
		for (int i = 0; i < num_materials; ++i) {
			auto& material = material_set->materials_.emplace_back(std::make_shared<Material>());
			auto texture_index = pmx.materials[i].base_texture_index;
			
			material->index = i;
			material->texture = (is_valid_index(textures, texture_index) && textures[texture_index]) ? textures[texture_index] : Texture::GetDummyTexture();
		}
	}

	{
		MaterialSet::MaterialConstants constants{};
		for (int i = 0; i < num_materials; ++i) {
			store(constants.diffuse[i], pmx.materials.at(i).diffuse);
		}

		auto& constant_buffer = material_set->constant_buffer_;
		constant_buffer = context->CreateBuffer(sizeof(MaterialSet::MaterialConstants), &constants);
		if (!constant_buffer) {
			return nullptr;
		}
	}

	return material_set;
}

}