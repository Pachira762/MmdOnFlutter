import 'dart:typed_data';

import 'package:flutter/cupertino.dart';

import 'mmd_viewer_platform_interface.dart';

class MmdViewer {
  Future<String?> getPlatformVersion() {
    return MmdViewerPlatform.instance.getPlatformVersion();
  }

  Future<void> createChildWindow(int width, int height, {EdgeInsets margin = EdgeInsets.zero}) {
    return MmdViewerPlatform.instance
        .createChildWindow(width, height, margin.left.toInt(), margin.top.toInt(), margin.right.toInt(), margin.bottom.toInt());
  }

  Future<dynamic> openModel(String path) {
    return MmdViewerPlatform.instance.openModel(path);
  }

  Future<dynamic> openScene(String path) {
    return MmdViewerPlatform.instance.openScene(path);
  }

  Future<void> update(int frame, Float32List values) {
    return MmdViewerPlatform.instance.update(frame, values);
  }

  Future<bool?> saveAnimation(
      String path, List<String> morphNames, List<Int32List> frameTracks, List<Float32List> valueTracks, String modelName) {
    return MmdViewerPlatform.instance.saveAnimation(path, morphNames, frameTracks, valueTracks, modelName);
  }
}
