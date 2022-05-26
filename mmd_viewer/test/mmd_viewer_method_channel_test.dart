import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:mmd_viewer/mmd_viewer_method_channel.dart';

void main() {
  MethodChannelMmdViewer platform = MethodChannelMmdViewer();
  const MethodChannel channel = MethodChannel('mmd_viewer');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
