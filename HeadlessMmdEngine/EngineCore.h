#pragma once
#include <string>
#include "HeadlessMmdEngine.h"
#include "DxContext.h"
#include "Scene.h"
#include "Renderer.h"
#include "AssetHolder.h"

namespace headless_mmd {

class EngineCore {
public:
	~EngineCore();

	bool Start(HWND hwnd);
	void Stop();
	void Update(int frame, const std::vector<float>& morph_values);
	void Draw();

	std::shared_ptr<Model> LoadModel(const std::wstring& path);
	std::shared_ptr<Animation> LoadScene(const std::wstring& path);

private:
	std::unique_ptr<DxContext> context_ = std::make_unique<DxContext>();
	std::unique_ptr<Scene> scene_ = std::make_unique<Scene>();
	std::unique_ptr<Renderer> renderer_ = std::make_unique<Renderer>();
	std::unique_ptr<AssetHolder> assets_ = std::make_unique<AssetHolder>();
};

} // namespace headless_mmd