#pragma once
#include <vector>
#include <map>
#include "Common.h"
#include "DxContext.h"
#include "Model.h"
#include "MmdScene.h"
#include "CameraAnimation.h"
#include "HeadlessMmdEngine.h"

namespace headless_mmd {

using AssetId = std::uintptr_t;
inline constexpr AssetId NullId = reinterpret_cast<AssetId>(nullptr);

class AssetHolder {
public:
	AssetId	ImportPmxModel(DxContext* context, const std::wstring& path);
	void ImportMmdScene(DxContext* context, const std::wstring& path, const std::shared_ptr<Model>& model, AssetId& animation_id, AssetId& camera_id);

private:
	std::map<AssetId, std::shared_ptr<Model>>			model_assets_{};
	std::map<AssetId, std::shared_ptr<Animation>>		animation_assets_{};
	std::map<AssetId, std::shared_ptr<CameraAnimation>>	camera_animation_assets_{};

	template<typename T>
	std::map<AssetId, std::shared_ptr<T>>& GetAssetContainer();

	template<typename T>
	const std::map<AssetId, std::shared_ptr<T>>& GetAssetContainer() const;

	template<typename T>
	AssetId AddAsset(const std::shared_ptr<T>& asset) {
		if (asset) {
			auto id = reinterpret_cast<AssetId>(asset.get());
			GetAssetContainer<T>()[id] = asset;
			return id;
		}
		else {
			return NullId;
		}
	}

	template<typename T>
	std::shared_ptr<T> GetAsset(AssetId id) {
		auto container = GetAssetContainer<T>();
		if (auto it = container.find(id); it != container.end()) {
			return it->second;
		}
		else {
			return nullptr;
		}
	}

	template<typename T>
	std::vector<AssetId> GetAssetIds() const {
		std::vector<AssetId> ids{};

		for (const auto& [id, asset] : GetAssetContainer<T>()) {
			ids.emplace_back(id);
		}

		return ids;
	}

public:
	auto GetModel(AssetId id) {
		return GetAsset<Model>(id);
	}

	std::vector<AssetId> GetModelIds() const {
		return GetAssetIds<Model>();
	}

	auto GetAnimation(AssetId id) {
		return GetAsset<Animation>(id);
	}

	auto GetCameraAnimation(AssetId id) {
		return GetAsset<CameraAnimation>(id);
	}
};

template<>
inline std::map<AssetId, std::shared_ptr<Model>>& AssetHolder::GetAssetContainer() {
	return model_assets_;
}

template<>
inline const std::map<AssetId, std::shared_ptr<Model>>& AssetHolder::GetAssetContainer() const {
	return model_assets_;
}

template<>
inline std::map<AssetId, std::shared_ptr<Animation>>& AssetHolder::GetAssetContainer() {
	return animation_assets_;
}

template<>
inline const std::map<AssetId, std::shared_ptr<Animation>>& AssetHolder::GetAssetContainer() const {
	return animation_assets_;
}

template<>
inline std::map<AssetId, std::shared_ptr<CameraAnimation>>& AssetHolder::GetAssetContainer() {
	return camera_animation_assets_;
}

template<>
inline const std::map<AssetId, std::shared_ptr<CameraAnimation>>& AssetHolder::GetAssetContainer() const {
	return camera_animation_assets_;
}

}

