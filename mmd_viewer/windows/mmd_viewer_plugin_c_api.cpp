#include "include/mmd_viewer/mmd_viewer_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "mmd_viewer_plugin.h"

void MmdViewerPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  mmd_viewer::MmdViewerPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
