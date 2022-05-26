//
//  Generated file. Do not edit.
//

// clang-format off

#include "generated_plugin_registrant.h"

#include <file_selector_windows/file_selector_windows.h>
#include <menubar/menubar_plugin.h>
#include <mmd_viewer/mmd_viewer_plugin_c_api.h>

void RegisterPlugins(flutter::PluginRegistry* registry) {
  FileSelectorWindowsRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("FileSelectorWindows"));
  MenubarPluginRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("MenubarPlugin"));
  MmdViewerPluginCApiRegisterWithRegistrar(
      registry->GetRegistrarForPlugin("MmdViewerPluginCApi"));
}
