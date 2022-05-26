#ifndef FLUTTER_PLUGIN_MMD_VIEWER_PLUGIN_H_
#define FLUTTER_PLUGIN_MMD_VIEWER_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace mmd_viewer {

class MmdViewerPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  MmdViewerPlugin();

  virtual ~MmdViewerPlugin();

  // Disallow copy and assign.
  MmdViewerPlugin(const MmdViewerPlugin&) = delete;
  MmdViewerPlugin& operator=(const MmdViewerPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace mmd_viewer

#endif  // FLUTTER_PLUGIN_MMD_VIEWER_PLUGIN_H_
