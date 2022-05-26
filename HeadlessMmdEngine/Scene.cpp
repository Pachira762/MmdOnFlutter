#include "pch.h"
#include "Scene.h"

namespace {

int GetCurrentFrame() {
	using std::chrono::system_clock;
	static const auto t0 = system_clock::now();

	return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now() - t0).count()) * 30 / 1000;
}

}

namespace headless_mmd {

bool Scene::Init(DxContext* context) {
	constant_buffer_ = context->CreateDynamicBuffer(sizeof(SceneConstants));
	if (!constant_buffer_) {
		DLOG(L"Failed to create scene constant buffer");
		return false;
	}

	auto hr = constant_buffer_->Map(0, nullptr, (void**)&constants_);
	if (FAILED(hr)) {
		DLOG(L"Failed to map scene constants");
		return false;
	}

	store(constants_->light_direction, vector3_normalize(0.5f, -1.f, 0.8f));
	store(constants_->shadow_color, { .3f, 0.35f, 0.4f, 1.f });

	return true;
}

void Scene::Update(DxContext* context, int frame, const std::vector<float>& morph_values) {
	camera_.Update(frame);

	for (auto& model : models_) {
		model->Update(context, frame, morph_values);
	}

	auto [width, height] = context->GetRenderTargetSize();
	camera_.SetAspect(static_cast<float>(width) / static_cast<float>(height));

	auto view = camera_.GetViewMatrix();
	auto projection = camera_.GetProjectionMatrix();
	store_transposed(constants_->view, view);
	store_transposed(constants_->projectoin, projection);
	store_transposed(constants_->world_to_screen, view * projection);
	store(constants_->eye_position, camera_.GetPosition());
	store(constants_->eye_forward, camera_.GetForward());
}

}