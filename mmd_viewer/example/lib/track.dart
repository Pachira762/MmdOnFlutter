import 'dart:typed_data';
import 'package:collection/collection.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';

class TrackKey {
  final int frame;
  final double value;

  const TrackKey(this.frame, this.value);
}

class Track {
  final List<TrackKey> keys;

  int _lastFrame = -1;
  double _lastValue = 0;

  Track(this.keys);

  Track.combine(Int32List frames, Float32List values)
      : keys = <TrackKey>[for (int i = 0; i < frames.length; ++i) TrackKey(frames[i], values[i])];

  int lowerBoundIndex(int frame) {
    return lowerBound(keys, TrackKey(frame, 0.0), compare: (TrackKey k0, TrackKey k1) => k0.frame.compareTo(k1.frame));
  }

  double calcAt(int frame) {
    if (frame == _lastFrame) {
      return _lastValue;
    }

    if (keys.isEmpty) {
      _lastValue = 0.0;
    } else if (keys.first.frame >= frame) {
      _lastValue = keys.first.value;
    } else if (keys.last.frame <= frame) {
      _lastValue = keys.last.value;
    } else {
      final index = lowerBoundIndex(frame);
      final prev = keys[index - 1];
      final next = keys[index];
      final t = (frame - prev.frame) / (next.frame - prev.frame);

      _lastValue = (1.0 - t) * prev.value + t * next.value;
    }

    _lastFrame = frame;
    return _lastValue;
  }
}

class TrackNotifier extends StateNotifier<Track> {
  TrackNotifier() : super(Track([]));

  void combine(Int32List frames, Float32List values) {
    if (frames.length != values.length) {
      return;
    }

    state = Track.combine(frames, values);
  }

  void update(int frame, double value) {
    var keys = [...state.keys];
    final newKey = TrackKey(frame, value);

    final index = state.lowerBoundIndex(frame);
    if (index < keys.length && keys[index].frame == frame) {
      keys[index] = newKey;
    } else {
      keys.insert(index, newKey);
    }

    state = Track(keys);
  }
}
