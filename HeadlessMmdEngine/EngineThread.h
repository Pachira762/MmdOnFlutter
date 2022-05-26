#pragma once
#include <future>
#include "Common.h"
#include "HeadlessMmdEngine.h"
#include "EngineCore.h"

namespace headless_mmd {

template<typename Result, typename Parameter>
class ThreadRequest {
public:
	using Func = std::function<bool(const Parameter& parameter, Result& result)>;

private:
	std::atomic_bool query_ = false;
	const Parameter* parameter_ = nullptr;
	Result* result_ = nullptr;
	std::promise<bool> promise_;
	Func func_;

public:
	ThreadRequest(Func&& func) :
		func_(func) {
	}

	bool Query(const Parameter& parameter, Result& result);
	void Process();
};

class EngineThread {
public:
	EngineThread();
	~EngineThread();

	bool Start(HWND hwnd);
	void Stop();

	void Update(int frame);
	void Update(int frame, int morph_index, float morph_value);
	void Update(int frame, const std::vector<float>& morph_values);

	bool LoadModel(const std::wstring& path, ModelInfo& info);
	bool LoadScene(const std::wstring& path, AnimationInfo& info);

private:
	std::unique_ptr<EngineCore> core_ = std::make_unique<EngineCore>();
	
	std::mutex mtx_;
	int frame_ = 0;
	std::vector<float> morph_values_;

	std::thread th_;
	std::atomic_bool run_;

	ThreadRequest<ModelInfo, std::wstring> model_request_;
	ThreadRequest<AnimationInfo, std::wstring> animation_request_;

	bool LoadModelInThread(const std::wstring& path, ModelInfo& info);
	bool LoadAnimationInThread(const std::wstring& path, AnimationInfo& info);
	void Proc(std::promise<bool> init_promise);
};

}

