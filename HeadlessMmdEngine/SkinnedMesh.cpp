#include "pch.h"
#include "SkinnedMesh.h"
#include "MathHelper.h"

namespace headless_mmd {

void SkinnedMesh::SetMorph(int index, float value) {
	morph_values_.at(index) = value;
}

void SkinnedMesh::Update() {
	ZeroMemory(offsets_.data(), sizeof(Vector) * num_vertices_);

	const auto num_morphs = static_cast<int>(morphs_.size());
	for (int i = 0; i < num_morphs; ++i) {
		auto value = morph_values_[i];
		auto scale = DirectX::XMVECTOR{ value,value,value,value };

		for (auto& data : morphs_.at(i).data) {
			const auto index = data.index;
			offsets_[index] = DirectX::XMVectorMultiplyAdd(scale, data.offset, offsets_[index]);
		}
	}

	for (int i = 0; i < num_vertices_; ++i) {
		store(dynamic_vertices_[i].offset, offsets_[i]);
	}
}

std::shared_ptr<SkinnedMesh> SkinnedMeshImporter::Import(DxContext* context, const Pmx& pmx, const std::wstring& model_path, Model* parent) {
	auto skinned_mesh = std::make_shared<SkinnedMesh>();
	skinned_mesh->parent = parent;

	{
		const auto num_vertices = static_cast<int>(pmx.vertices.size());
		skinned_mesh->num_vertices_ = num_vertices;

		const auto static_buffer_size = static_cast<UINT32>(sizeof(SkinnedMesh::StaticVertex) * num_vertices);
		const auto dynamic_buffer_size = static_cast<UINT32>(sizeof(SkinnedMesh::DynamicVertex) * num_vertices);

		std::vector<SkinnedMesh::StaticVertex> static_vertices(num_vertices);
		for (int i = 0; i < num_vertices; ++i) {
			auto& pmx_vertex = pmx.vertices[i];
			auto& static_vertex = static_vertices[i];

			std::memcpy(static_vertex.position,		&pmx_vertex.position,	 sizeof(float) * 3);
			std::memcpy(static_vertex.uv,			&pmx_vertex.uv,			 sizeof(float) * 2);
			std::memcpy(static_vertex.normal,		&pmx_vertex.normal,		 sizeof(float) * 3);
			std::memcpy(static_vertex.bone_indices, pmx_vertex.bone_indices, sizeof(float) * 4);
			std::memcpy(static_vertex.bone_weights, pmx_vertex.bone_weights, sizeof(float) * 4);
		}

		auto& static_buffer = skinned_mesh->static_buffer_;
		static_buffer = context->CreateBuffer(static_buffer_size, static_vertices.data());
		if (!static_buffer) {
			return nullptr;
		}

		auto& dynamic_buffer = skinned_mesh->dynamic_buffer_;
		dynamic_buffer = context->CreateDynamicBuffer(dynamic_buffer_size);
		if (!dynamic_buffer) {
			return nullptr;
		}

		auto& dynamic_vertices = skinned_mesh->dynamic_vertices_;
		if (FAILED(dynamic_buffer->Map(0, nullptr, (void**)&dynamic_vertices))) {
			return nullptr;
		}
		ZeroMemory(dynamic_vertices, dynamic_buffer_size);

		auto& static_vertex_view = skinned_mesh->vertex_buffer_views_[0];
		static_vertex_view.BufferLocation = static_buffer->GetGPUVirtualAddress();
		static_vertex_view.SizeInBytes = static_buffer_size;
		static_vertex_view.StrideInBytes = static_cast<UINT>(sizeof(SkinnedMesh::StaticVertex));

		auto& dynamic_vertex_view = skinned_mesh->vertex_buffer_views_[1];
		dynamic_vertex_view.BufferLocation = dynamic_buffer->GetGPUVirtualAddress();
		dynamic_vertex_view.SizeInBytes = dynamic_buffer_size;
		dynamic_vertex_view.StrideInBytes = static_cast<UINT>(sizeof(SkinnedMesh::DynamicVertex));

		skinned_mesh->offsets_.resize(num_vertices);
	}

	{
		const auto num_indices = static_cast<int>(pmx.indices.size());
		skinned_mesh->num_indices_ = num_indices;

		const auto index_buffer_size = static_cast<UINT>(sizeof(UINT32) * num_indices);

		auto& index_buffer = skinned_mesh->index_buffer_;
		index_buffer = context->CreateBuffer(index_buffer_size, pmx.indices.data(), D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_COMMON);
		if (!index_buffer) {
			return nullptr;
		}

		auto& index_buffer_view = skinned_mesh->index_buffer_view_;
		index_buffer_view.BufferLocation = index_buffer->GetGPUVirtualAddress();
		index_buffer_view.Format = DXGI_FORMAT_R32_UINT;
		index_buffer_view.SizeInBytes = index_buffer_size;
	}

	{
		auto& morph_names = skinned_mesh->morph_names_;
		auto& morph_panels = skinned_mesh->morph_panels_;
		auto& morphs = skinned_mesh->morphs_;
		auto& morph_values = skinned_mesh->morph_values_;

		const auto num_morphs = static_cast<int>(pmx.vertex_morphs.size());
		morph_names.resize(num_morphs);
		morph_panels.resize(num_morphs);
		morphs.resize(num_morphs);
		morph_values.resize(num_morphs);

		for (int i = 0; i < num_morphs; ++i) {
			const auto& morph = pmx.vertex_morphs[i];

			morph_names[i] = morph.name;
			morph_panels[i] = morph.panel;
			morphs[i].data = morph.data;
			morph_values[i] = 0.f;
		}
	}

	{
		auto material_set = MaterialImporter().Import(context, pmx, model_path);
		if (!material_set) {
			return nullptr;
		}
		skinned_mesh->material_set_ = material_set;

		const auto num_materials = static_cast<int>(pmx.materials.size());
		for (int i = 0, index_offset = 0; i < num_materials; ++i) {
			auto& mesh = skinned_mesh->meshes_.emplace_back();
			auto num_indices = pmx.materials[i].num_vertices;

			mesh.parent_ = skinned_mesh.get();
			mesh.material_ = material_set->GetMaterial(i);
			mesh.num_indices_ = num_indices;
			mesh.index_start_ = index_offset;

			index_offset += num_indices;
		}
	}

	return skinned_mesh;
}

}