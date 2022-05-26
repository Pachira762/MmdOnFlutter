import 'dart:async';
import 'dart:isolate';

class PlaybackClock {
  static final _playerClock = _PlaybackClock();
  static DateTime epoch = DateTime.now();
  static Duration offset = const Duration();
  static StreamSubscription<dynamic>? _subscription;
  static void Function(Duration) _onChanged = (value) {};

  static void pause() {
    if (_playerClock.playing) {
      _playerClock.pause();
      offset = offset + DateTime.now().difference(epoch);
    }
  }

  static void resume() {
    if (!_playerClock.playing) {
      epoch = DateTime.now();
      _playerClock.resume();
    }
  }

  static void setPosition(Duration position) {
    epoch = DateTime.now();
    offset = position;

    if (!_playerClock.playing) {
      _onChanged(position);
    }
  }

  static void listen(void Function(Duration) onPositionChanged) {
    _onChanged = onPositionChanged;

    _subscription ??= _playerClock.listen((_) {
      _onChanged(offset + DateTime.now().difference(epoch));
    });
  }

  static bool isPlaying() {
    return _playerClock.playing;
  }
}

class _PlaybackClock {
  final _receivePort = ReceivePort();
  late final Isolate _isolate;
  Capability? _capability;
  bool _playing = true;

  _PlaybackClock() {
    _initAsync();
  }

  bool get playing => _playing;

  void _initAsync() async {
    _isolate = await Isolate.spawn((SendPort sendPort) {
      Timer.periodic(const Duration(milliseconds: 16), (timer) {
        sendPort.send(0);
      });
    }, _receivePort.sendPort);

    pause();
  }

  void pause() {
    _capability = _isolate.pause();
    _playing = false;
  }

  void resume() {
    if (_capability != null) {
      _isolate.resume(_capability!);
    }
    _playing = true;
  }

  StreamSubscription<dynamic> listen(void Function(dynamic) onData) {
    return _receivePort.listen(onData);
  }
}
