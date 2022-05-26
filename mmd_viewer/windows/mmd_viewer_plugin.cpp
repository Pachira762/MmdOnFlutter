#include "mmd_viewer_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>
#include <format>

#include "HeadlessMmdEngine.h"

namespace {
auto engine = headless_mmd::HeadlessMmdEngine::GetInstance();

HWND hmain = nullptr;
HWND hview  = nullptr;
HWND hchild = nullptr;

SIZE content_size{1280, 720};
RECT margin{0,800,0,400};

std::string ToString(const std::wstring& wstr){
  char buf[1024]{};
  WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()), buf, static_cast<int>(_countof(buf)), NULL, FALSE);
  return buf;
}

std::wstring ToWstring(const std::string& str){
  wchar_t buf[1024]{};
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), buf, static_cast<int>(_countof(buf)));
  return buf;
}

std::string ToSjis(const std::string& utf8) {
  auto utf16 = ToWstring(utf8);

  char buf[1024]{};
  WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), static_cast<int>(utf16.length()), buf, static_cast<int>(_countof(buf)), NULL, FALSE);
  return buf;  
}

float CalcAspect(const SIZE& size){
  return static_cast<float>(size.cx)/static_cast<float>(size.cy);
}

// x, y, width, height
std::tuple<int, int, int, int> CalcChildWindowPos(int cx, int cy) {
  const auto cw = cx - margin.left - margin.right;
  const auto ch = cy - margin.top - margin.bottom;
  const auto content_aspect = CalcAspect(content_size);
  const auto aspect = CalcAspect({cw, ch});
  int x = 0;
  int y = 0;
  int width = cw;
  int height = ch;
  if(aspect > content_aspect){
    width = static_cast<int>(content_aspect * height);
    x = (cw - width)/2;
  }
  else{
    height = static_cast<int>(width / content_aspect);
    y = (ch - height)/2;
  }

  return {x + margin.left, y + margin.top, width, height};
}

void AdjustChildWindow(int cx, int cy){
  auto [x,y,width,height] = CalcChildWindowPos(cx, cy);
  SetWindowPos(hchild, NULL, x, y, width, height, SWP_NOZORDER);
}

void CreateChildWindow(){
  DWORD style = static_cast<DWORD>(GetWindowLongPtr(hview, GWL_STYLE));
  auto ret = SetWindowLongPtr(hview, GWL_STYLE, style | WS_CLIPCHILDREN);
  if(ret == 0){
    MessageBox(NULL, L"Failed apply WS_CLIPCHILDREN", L"Error", MB_OK);
  }

  //HINSTANCE hInsta
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"ChildWindowClass";
	if (RegisterClassEx(&wc) == INVALID_ATOM) {
    MessageBox(NULL, L"Failed to registrar child window class", L"Error", MB_OK);
		return;
	}

  RECT rc{};
  GetClientRect(hview, &rc);
  auto [x,y,width,height] = CalcChildWindowPos(rc.right, rc.bottom);
	hchild = CreateWindowEx(0, L"ChildWindowClass", L"Child", WS_VISIBLE | WS_CHILD | WS_DISABLED,
	  x,y,width,height, hview, NULL, GetModuleHandle(NULL), nullptr);

	if (!hchild) {
    std::wstring msg = L"Failed to create child window for " + std::to_wstring(reinterpret_cast<std::uintptr_t>(hview));
    MessageBox(NULL, msg.c_str(), L"Error", MB_OK);
		return;
	}

  if(!engine->Start(hchild)){
    MessageBox(NULL, L"Failed to start engine", L"Error", MB_OK);
    return;
  }
}

}

namespace mmd_viewer {

template<typename T>
T GetArgument(const flutter::EncodableValue* args, const char* key){
  const auto& arg_map = std::get<flutter::EncodableMap>(*args);
  return std::get<T>(arg_map.at(flutter::EncodableValue(key)));
}

// static
void MmdViewerPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
  auto channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "mmd_viewer",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<MmdViewerPlugin>();

  channel->SetMethodCallHandler([plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  hview = registrar->GetView()->GetNativeWindow();
  hmain = GetParent(hview);

  registrar->RegisterTopLevelWindowProcDelegate([](HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) -> std::optional<LRESULT> {
    switch(msg){
      case WM_SIZE:
        AdjustChildWindow(LOWORD(lp), HIWORD(lp));
        break;
    }

    return std::nullopt;
  });
  
  registrar->AddPlugin(std::move(plugin));
}

MmdViewerPlugin::MmdViewerPlugin() {}

MmdViewerPlugin::~MmdViewerPlugin() {}

void MmdViewerPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  const auto& method_name = method_call.method_name();
  auto args = method_call.arguments();

  if (method_name.compare("getPlatformVersion") == 0) {
    std::ostringstream version_stream;
    version_stream << "Windows ";
    if (IsWindows10OrGreater()) {
      version_stream << "10+";
    } else if (IsWindows8OrGreater()) {
      version_stream << "8";
    } else if (IsWindows7OrGreater()) {
      version_stream << "7";
    }
    result->Success(flutter::EncodableValue(version_stream.str()));
  }
  else if(method_name.compare("createChildWindow") == 0) {
    if(!method_call.arguments()){
      result->NotImplemented();
      return;
    }

    if(hchild){
      result->Success();
      return;
    }

    content_size.cx = GetArgument<int32_t>(args, "width");
    content_size.cy = GetArgument<int32_t>(args, "height");
    margin.left = GetArgument<int32_t>(args, "margin_left");
    margin.top = GetArgument<int32_t>(args, "margin_top");
    margin.right = GetArgument<int32_t>(args, "margin_right");
    margin.bottom = GetArgument<int32_t>(args, "margin_bottom");
    CreateChildWindow();

    result->Success();
  }
  else if(method_name.compare("openModel") == 0) {
    auto path = ToWstring(GetArgument<std::string>(args, "path"));
    headless_mmd::ModelInfo info{};
    if(engine->LoadModel(path, info)){
      flutter::EncodableList morph_names;
      for(const auto& name : info.morph_names){
        morph_names.push_back(flutter::EncodableValue(ToString(name)));
      }

      flutter::EncodableList morph_categories;
      for(const auto& category : info.morph_categories){
        morph_categories.push_back(flutter::EncodableValue(category));
      }

      result->Success(flutter::EncodableMap{
        {flutter::EncodableValue("morph_names"), flutter::EncodableValue(morph_names)},
        {flutter::EncodableValue("morph_categories"), flutter::EncodableValue(morph_categories)},

      });
    }
    else{
      result->Success(flutter::EncodableMap{
        {flutter::EncodableValue("morph_names"), flutter::EncodableValue(flutter::EncodableList())},
        {flutter::EncodableValue("morph_categories"), flutter::EncodableValue(flutter::EncodableList())},
      });
    }
  }
  else if(method_name.compare("openScene") == 0) {
    auto path = ToWstring(GetArgument<std::string>(args, "path"));
    headless_mmd::AnimationInfo info{};
    
    if(engine->LoadScene(path, info)){
      flutter::EncodableList frame_tracks{}, value_tracks{};
      for(auto& track : info.morph_animation) {
        const auto num_keys = static_cast<int>(track.keys.size());
        std::vector<int32_t> frames(num_keys);
        std::vector<float> values(num_keys);

        for(int i = 0; i < num_keys; ++i){
          frames[i] = track.keys[i].frame;
          values[i] = track.keys[i].value;
        }

        frame_tracks.push_back(flutter::EncodableValue(frames));
        value_tracks.push_back(flutter::EncodableValue(values));
      }

      result->Success(flutter::EncodableMap{
        {flutter::EncodableValue("num_frames"), flutter::EncodableValue(info.num_frames)},
        {flutter::EncodableValue("frame_tracks"), flutter::EncodableValue(frame_tracks)},
        {flutter::EncodableValue("value_tracks"), flutter::EncodableValue(value_tracks)},
      });
    }
    else{
      result->Success(flutter::EncodableMap{
        {flutter::EncodableValue("num_frames"), flutter::EncodableValue(info.num_frames)},
        {flutter::EncodableValue("frame_tracks"), flutter::EncodableValue(flutter::EncodableList())},
        {flutter::EncodableValue("value_tracks"), flutter::EncodableValue(flutter::EncodableList())},
      });
    } 
  }
  else if(method_name.compare("update") == 0) {
    auto frame = GetArgument<int>(args, "frame");
    auto values = GetArgument<std::vector<float>>(args, "values");
    engine->Update(frame, values);
    
    result->Success();
  }
  else if(method_name.compare("saveAnimation") == 0) {
    auto path = GetArgument<std::string>(args, "path");
    auto flutter_morph_names = GetArgument<flutter::EncodableList>(args, "morph_names");
    auto flutter_frame_tracks = GetArgument<flutter::EncodableList>(args, "frame_tracks");
    auto flutter_value_tracks = GetArgument<flutter::EncodableList>(args, "value_tracks");
    auto model_name = GetArgument<std::string>(args, "model_name");

    std::vector<std::string> morph_names(flutter_morph_names.size());
    for(std::size_t i = 0; i < flutter_morph_names.size(); ++i){
      morph_names[i] = ToSjis(std::get<std::string>(flutter_morph_names[i]));
    }

    const auto num_tracks = flutter_frame_tracks.size();
    if(num_tracks != flutter_value_tracks.size()){
      MessageBox(NULL, L"Invalid track and values count", L"error", MB_OK);
      result->Success(flutter::EncodableValue(false));
      return;
    }

    std::vector<std::vector<int32_t>> frame_tracks(num_tracks);
    std::vector<std::vector<float>> value_tracks(num_tracks);
    for(std::size_t i = 0; i < num_tracks; ++i){
      frame_tracks[i] = std::get<std::vector<int32_t>>(flutter_frame_tracks[i]);
      value_tracks[i] = std::get<std::vector<float>>(flutter_value_tracks[i]);
    }

    auto saved = engine->SaveMorphAnimation(ToWstring(path), morph_names, frame_tracks, value_tracks, model_name);
    if(saved){
      MessageBeep(MB_ICONASTERISK);
    }

    result->Success(flutter::EncodableValue(saved));
  }
  else {
    result->NotImplemented();
  }
}

}  // namespace mmd_viewer
