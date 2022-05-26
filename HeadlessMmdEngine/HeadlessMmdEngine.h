#pragma once
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <ranges>
#include <functional>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windef.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

namespace headless_mmd {

template<typename T>
inline T Interpolate(const T& a, const T& b, float t) {
	return (1.f - t) * a + t * b;
}

template<typename T>
struct Key {
	int	frame;
	T	value;
};

template<typename T>
struct Track {
	std::vector<Key<T>> keys;

	std::tuple<const T&, const T&, float> SearchNearValue(const int frame) const {
		if (frame <= keys.front().frame) {
			return { keys.front().value, keys.front().value, 0.f };
		}
		else if (frame >= keys.back().frame) {
			return { keys.back().value, keys.back().value, 0.f };
		}

		auto it = std::ranges::lower_bound(keys, frame, {}, &Key<T>::frame);
		if (it->frame == frame) {
			return { it->value, it->value, 0.f };
		}

		auto& key0 = *(it - 1);
		auto& key1 = *(it);
		float s = static_cast<float>(frame - key0.frame) / static_cast<float>(key1.frame - key0.frame);
		return { key0.value, key1.value, s };
	}

	T CalcAt(const int frame) const {
		auto [value0, value1, s] = SearchNearValue(frame);
		if (s == 0.f) {
			return value0;
		}
		else {
			return Interpolate(value0, value1, s);
		}
	}

	const T& GetByIndex(const int index) const {
		if (index < 0) {
			return keys.front().value;
		}
		else if (index >= keys.size()) {
			return keys.back().value;
		}
		else {
			return keys[index].value;
		}
	}

	void Insert(const int frame, const T& value) {
		auto it = std::ranges::upper_bound(keys, frame, {}, &Key<T>::frame);
		keys.emplace(it, frame, value);
	}

	bool IsEmpty() const {
		return keys.empty();
	}
};

struct ModelInfo {
	std::vector<std::wstring> morph_names;
	std::vector<int>		  morph_categories;
};

struct AnimationInfo {
	int num_frames;
	std::vector<Track<float>> morph_animation;
};


class HeadlessMmdEngine {
public:
	bool Start(HWND hwnd);
	void Stop();

	void Update(int frame);
	void Update(int frame, int morph_index, float morph_value);
	void Update(int frame, const std::vector<float>& morph_values);

	bool LoadModel(const std::wstring& path, ModelInfo& info);
	bool LoadScene(const std::wstring& path, AnimationInfo& info);

	static bool SaveMorphAnimation(const std::wstring& path, const std::vector<std::string>& morph_names, const std::vector<Track<float>>& animation, const std::string& model_name);
	static bool SaveMorphAnimation(const std::wstring& path, const std::vector<std::string>& morph_names, const std::vector<std::vector<int32_t>>& frame_tracks, const std::vector<std::vector<float>>& value_tracks, const std::string& model_name);

private:
	HeadlessMmdEngine();
	~HeadlessMmdEngine();

public:
	static HeadlessMmdEngine* GetInstance() {
		static HeadlessMmdEngine instance{};
		return &instance;
	}
};

} // namespace headless_mmd
