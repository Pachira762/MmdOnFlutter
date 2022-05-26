import 'dart:math';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'track.dart';
import 'playback_state.dart';

double _clamp(double imin, double imax, double value) {
  return max(imin, min(imax, value));
}

const maxMorphCount = 256;

final morphTrackProviders = List<StateNotifierProvider<TrackNotifier, Track>>.generate(
    maxMorphCount, (index) => StateNotifierProvider<TrackNotifier, Track>((ref) => TrackNotifier()));

final morphValueProviders = List<Provider<double>>.generate(
    maxMorphCount,
    (index) => Provider<double>((ref) {
          final frame = ref.watch(playingPositionProvider);
          return _clamp(0.0, 1.0, ref.watch(morphTrackProviders[index]).calcAt(frame));
        }));

final editingTrackIndexProvider = StateProvider<int>((ref) => -1);
