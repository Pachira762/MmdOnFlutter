import 'dart:typed_data';
import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'mmd_viewer_platform_interface.dart';

/// An implementation of [MmdViewerPlatform] that uses method channels.
class MethodChannelMmdViewer extends MmdViewerPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('mmd_viewer');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<void> createChildWindow(int width, int height, int marginLeft, int marginTop, int marginRight, int marginBottom) async {
    return await methodChannel.invokeMethod<void>("createChildWindow", {
      'width': width,
      'height': height,
      'margin_left': marginLeft,
      'margin_top': marginTop,
      'margin_right': marginRight,
      'margin_bottom': marginBottom
    });
  }

  @override
  Future<dynamic> openModel(String path) {
    return methodChannel.invokeMethod<void>('openModel', {'path': path});
  }

  @override
  Future<dynamic> openScene(String path) {
    return methodChannel.invokeMethod<void>('openScene', {'path': path});
  }

  @override
  Future<void> update(int frame, Float32List values) {
    return methodChannel.invokeMethod('update', {'frame': frame, 'values': values});
  }

  @override
  Future<bool?> saveAnimation(
      String path, List<String> morphNames, List<Int32List> frameTracks, List<Float32List> valueTracks, String modelName) {
    return methodChannel.invokeMethod<bool>('saveAnimation',
        {'path': path, 'morph_names': morphNames, 'frame_tracks': frameTracks, 'value_tracks': valueTracks, 'model_name': modelName});
  }
}
