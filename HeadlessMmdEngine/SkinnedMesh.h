#pragma once
#include <memory>
#include <memory>
#include "Common.h"
#include "Mesh.h"
#include "Material.h"

namespace headless_mmd {

class Model;

class SkinnedMesh {
public:
	static constexpr D3D12_INPUT_ELEMENT_DESC InputLayout[] = {
		{},
	};

	void SetMorph(int index, float value);
	void Update();

private:
	struct StaticVertex {
		float position[3];
		float uv[2];
		float normal[3];
		int32_t bone_indices[4];
		float	bone_weights[4];
	};

	struct DynamicVertex {
		Float3 offset;
	};

	struct VertexMorph {
		using Data = Pmx::VertexMorphData;

		std::vector<Data> data{};
	};

	Model* parent;

	int num_vertices_{};
	IDXResourcePtr static_buffer_{};
	IDXResourcePtr dynamic_buffer_{};
	DynamicVertex* dynamic_vertices_{};
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_views_[2];
	std::vector<Vector> offsets_{};

	int num_indices_{};
	IDXResourcePtr index_buffer_{};
	D3D12_INDEX_BUFFER_VIEW  index_buffer_view_{};

	std::vector<std::wstring> morph_names_{};
	std::vector<portable_mmd::PmxMorphPanel> morph_panels_{};
	std::vector<VertexMorph>  morphs_{};
	std::vector<float> morph_values_{};

	std::shared_ptr<MaterialSet> material_set_{};
	std::vector<Mesh> meshes_{};

	friend class SkinnedMeshImporter;

public:
	const Model* GetParent() const {
		return parent;
	}

	const D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferViews() const {
		return vertex_buffer_views_;
	}

	int GetNumVertexBuffers() const {
		return _countof(vertex_buffer_views_);
	}

	const D3D12_INDEX_BUFFER_VIEW* GetIndexBufferView() const {
		return &index_buffer_view_;
	}

	int GetNumMorphs() const {
		return static_cast<int>(morph_names_.size());
	}

	const std::vector<std::wstring>& GetMorphNames() const {
		return morph_names_;
	}

	const std::vector<portable_mmd::PmxMorphPanel> GetMorphPanels() const {
		return morph_panels_;
	}

	const std::vector<Mesh>& GetMeshes() const {
		return meshes_;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetConstantBufferLocation() const {
		return material_set_->GetConstantBufferLocation();
	}
};

class SkinnedMeshImporter {
public:
	std::shared_ptr<SkinnedMesh> Import(DxContext* context, const Pmx& pmx, const std::wstring& model_path, Model* parent);
};

}