#include "pch.h"
#include "EngineThread.h"
#include <ranges>

namespace headless_mmd {

template<typename Result, typename Parameter>
bool ThreadRequest<Result, Parameter>::Query(const Parameter& parameter, Result& result) {
	parameter_ = &parameter;
	result_ = &result;
	promise_ = std::promise<bool>();
	query_ = true;

	return promise_.get_future().get();
}

template<typename Result, typename Parameter>
void ThreadRequest<Result, Parameter>::Process() {
	if (!query_) {
		return;
	}

	query_ = false;
	promise_.set_value(func_(*parameter_, *result_));
}

EngineThread::EngineThread() :
	model_request_([this](const std::wstring& path, ModelInfo& info)->bool {return LoadModelInThread(path, info); }),
	animation_request_([this](const std::wstring& path, AnimationInfo& info)->bool {return LoadAnimationInThread(path, info); }) {
}

EngineThread::~EngineThread() {
	Stop();
}

bool EngineThread::Start(HWND hwnd) {
	if (!core_->Start(hwnd)) {
		return false;
	}

	std::promise<bool> init_promise{};
	auto init_future = init_promise.get_future();

	run_ = true;
	th_ = std::thread(&EngineThread::Proc, this, std::move(init_promise));

	return init_future.get();
}

void EngineThread::Stop() {
	run_ = false;
	if (th_.joinable()) {
		th_.join();
	}
}

void EngineThread::Update(int frame) {
	std::unique_lock<std::mutex> lock(mtx_);

	frame_ = frame;
}

void EngineThread::Update(int frame, int morph_index, float morph_value) {
	std::unique_lock<std::mutex> lock(mtx_);

	frame_ = frame;
	morph_values_.at(morph_index) = morph_value;
}

void EngineThread::Update(int frame, const std::vector<float>& morph_values) {
	std::unique_lock<std::mutex> lock(mtx_);

	frame_ = frame;

	if (morph_values_.size() == morph_values.size()) {
		std::memcpy(morph_values_.data(), morph_values.data(), sizeof(float) * morph_values.size());
	}
}

bool EngineThread::LoadModel(const std::wstring& path, ModelInfo& info) {
	return model_request_.Query(path, info);
}

bool EngineThread::LoadScene(const std::wstring& path, AnimationInfo& info) {
	return animation_request_.Query(path, info);
}

bool EngineThread::LoadModelInThread(const std::wstring& path, ModelInfo& info){
	auto model = core_->LoadModel(path);
	if (!model) {
		return false;
	}

	auto& morph_names = model->GetMesh()->GetMorphNames();
	info.morph_names = morph_names;

	auto& morph_panels = model->GetMesh()->GetMorphPanels();
	std::transform(morph_panels.begin(), morph_panels.end(), std::back_inserter(info.morph_categories), [](portable_mmd::PmxMorphPanel panel) { return static_cast<int>(panel); });

	std::unique_lock<std::mutex> lock(mtx_);
	morph_values_.resize(morph_names.size());
	
	return true;
}

bool EngineThread::LoadAnimationInThread(const std::wstring& path, AnimationInfo& info) {
	auto animation = core_->LoadScene(path);
	if (!animation) {
		return false;
	}

	info.num_frames = animation->GetNumFrames();
	info.morph_animation = animation->GetMorphAnimation();

	return true;
}

void EngineThread::Proc(std::promise<bool> init_promise) {
	init_promise.set_value(SUCCEEDED(CoInitializeEx(NULL, COINIT_MULTITHREADED)));

	auto t0 = std::chrono::system_clock::now();
	while (run_) {
		model_request_.Process();
		animation_request_.Process();

		std::unique_lock<std::mutex> lock(mtx_);
		core_->Update(frame_, morph_values_);
		lock.unlock();

		core_->Draw();

		std::this_thread::sleep_until(t0 + std::chrono::milliseconds(10));
		t0 = std::chrono::system_clock::now();
	}

	CoUninitialize();
}

}
