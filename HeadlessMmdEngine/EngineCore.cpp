#include "pch.h"
#include "EngineCore.h"
#include "DebugTimer.h"

namespace headless_mmd {

EngineCore::~EngineCore() {
	Stop();
}

bool EngineCore::Start(HWND hwnd) {
	Timer timer;

	if (!context_->Init(hwnd)) {
		return false;
	}
	timer.Stop(L"init context");

	if (!scene_->Init(context_.get())) {
		return false;
	}
	timer.Stop(L"init scene");

	if (!renderer_->Init(context_.get(), scene_.get())) {
		return false;
	}
	timer.Stop(L"init renderer");

	return true;
}

void EngineCore::Stop() {
	if (context_) {
		context_->Flush();
	}

	renderer_ = nullptr;
	scene_ = nullptr;
	assets_ = nullptr;
	context_ = nullptr;
}

void EngineCore::Update(int frame, const std::vector<float>& morph_values) {
	scene_->Update(context_.get(), frame, morph_values);
}

void EngineCore::Draw() {
	renderer_->Draw(context_.get(), scene_.get());
}

std::shared_ptr<Model> EngineCore::LoadModel(const std::wstring& path) {
	auto id = assets_->ImportPmxModel(context_.get(), path);
	if (id == NullId) {
		return nullptr;
	}

	auto model = assets_->GetModel(id);
	if (!model) {
		return nullptr;
	}

	scene_->AddModel(model);

	return model;
}

std::shared_ptr<Animation> EngineCore::LoadScene(const std::wstring& path) {
	std::shared_ptr<Model> model{};
	if (auto model_ids = assets_->GetModelIds(); !model_ids.empty()) {
		model = assets_->GetModel(model_ids.front());
	}

	AssetId animation_id{};
	AssetId camera_id{};
	assets_->ImportMmdScene(context_.get(), path, model, animation_id, camera_id);

	std::shared_ptr<Animation> animation = animation_id != NullId ? assets_->GetAnimation(animation_id) : nullptr;
	if (animation) {
		model->SetAnimation(animation);

	}

	if (camera_id ) {
		auto camera_animation = assets_->GetCameraAnimation(camera_id);
		scene_->SetCameraAnimation(camera_animation);
	}

	return animation;
}

} // namespace headless_mmd
