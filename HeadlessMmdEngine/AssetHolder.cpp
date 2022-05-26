#include "pch.h"
#include "AssetHolder.h"
#include "MmdScene.h"

namespace {

template<typename T>
headless_mmd::AssetId MakeId(const std::shared_ptr<T>& asset) {
	return reinterpret_cast<headless_mmd::AssetId>(asset.get());
}

}

namespace headless_mmd {

AssetId AssetHolder::ImportPmxModel(DxContext* context, const std::wstring& path) {
	Pmx pmx{};
	if (!portable_mmd::LoadPmx(path, pmx)) {
		return NullId;
	}

	auto model = ModelImporter().Import(context, pmx, path);
	if (!model) {
		return NullId;
	}

	return AddAsset(model);
}

void AssetHolder::ImportMmdScene(DxContext* context, const std::wstring& path, const std::shared_ptr<Model>& model, AssetId& animation_id, AssetId& camera_id) {
	(void)context;
	animation_id = NullId;
	camera_id = NullId;

	auto scene = MmdSceneImporter().Import(path);
	if (!scene) {
		return;
	}

	if (scene->HasAnimationTrack() && model) {
		auto animation = AnimationImporter().Import(scene->GetAnimationTrack(0), *model->GetSkeleton(), model->GetMesh()->GetMorphNames());
		if (animation) {
			animation_id = AddAsset(animation);
		}
	}

	if (scene->HasCameraTrack()) {
		auto camera_anim = CameraAnimationImporter().Import(*scene);
		if (camera_anim) {
			camera_id = AddAsset(camera_anim);
		}
	}
}

}