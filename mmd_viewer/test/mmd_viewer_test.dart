import 'package:flutter_test/flutter_test.dart';
import 'package:mmd_viewer/mmd_viewer.dart';
import 'package:mmd_viewer/mmd_viewer_platform_interface.dart';
import 'package:mmd_viewer/mmd_viewer_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockMmdViewerPlatform 
    with MockPlatformInterfaceMixin
    implements MmdViewerPlatform {

  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final MmdViewerPlatform initialPlatform = MmdViewerPlatform.instance;

  test('$MethodChannelMmdViewer is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelMmdViewer>());
  });

  test('getPlatformVersion', () async {
    MmdViewer mmdViewerPlugin = MmdViewer();
    MockMmdViewerPlatform fakePlatform = MockMmdViewerPlatform();
    MmdViewerPlatform.instance = fakePlatform;
  
    expect(await mmdViewerPlugin.getPlatformVersion(), '42');
  });
}
