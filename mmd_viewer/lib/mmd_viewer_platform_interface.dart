import 'dart:typed_data';

import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'mmd_viewer_method_channel.dart';

abstract class MmdViewerPlatform extends PlatformInterface {
  /// Constructs a MmdViewerPlatform.
  MmdViewerPlatform() : super(token: _token);

  static final Object _token = Object();

  static MmdViewerPlatform _instance = MethodChannelMmdViewer();

  /// The default instance of [MmdViewerPlatform] to use.
  ///
  /// Defaults to [MethodChannelMmdViewer].
  static MmdViewerPlatform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [MmdViewerPlatform] when
  /// they register themselves.
  static set instance(MmdViewerPlatform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }

  Future<void> createChildWindow(int width, int height, int marginLeft, int marginTop, int marginRight, int marginBottom) {
    throw UnimplementedError('createChildWindow() has not been implemented.');
  }

  Future<dynamic> openModel(String path) {
    throw UnimplementedError('openModel() has not been implemented.');
  }

  Future<dynamic> openScene(String path) {
    throw UnimplementedError('openScene() has not been implemented.');
  }

  Future<void> update(int frame, Float32List values) {
    throw UnimplementedError('update() has not been implemented.');
  }

  Future<bool?> saveAnimation(
      String path, List<String> morphNames, List<Int32List> frameTracks, List<Float32List> valueTracks, String modelName) {
    throw UnimplementedError('saveAnimation() has not been implemented.');
  }
}
