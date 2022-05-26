import 'dart:math';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'playback_clock.dart';

class PlaybackState {
  final bool playing;
  final int playbackTime;
  final int playingPosition;

  PlaybackState(this.playing, playbackTime, int position)
      : playbackTime = max(0, playbackTime),
        playingPosition = max(0, min(playbackTime, position));
}

class PlaybackStateNotifier extends StateNotifier<PlaybackState> {
  PlaybackStateNotifier() : super(PlaybackState(false, 0, 0)) {
    PlaybackClock.listen((timePosition) {
      final playbackTime = state.playbackTime;
      final position = 30 * timePosition.inMilliseconds ~/ 1000;

      if (playbackTime == position) {
        PlaybackClock.pause();
        state = PlaybackState(false, state.playbackTime, position);
      } else {
        state = PlaybackState(state.playing, state.playbackTime, position);
      }
    });
  }

  set playing(bool playing) {
    final playbackTime = state.playbackTime;
    var position = state.playingPosition;

    if (playing) {
      if (playbackTime == position) {
        PlaybackClock.setPosition(const Duration());
        position = 0;
      }
      PlaybackClock.resume();
    } else {
      PlaybackClock.pause();
    }

    state = PlaybackState(playing, playbackTime, position);
  }

  set playbackTime(int playbackTime) {
    state = PlaybackState(state.playing, playbackTime, 0);
  }

  set position(int frame) {
    PlaybackClock.setPosition(Duration(milliseconds: 1000 * frame ~/ 30));
  }

  void offset(int offset) {
    final newPos = state.playingPosition + offset;
    PlaybackClock.setPosition(Duration(milliseconds: 1000 * newPos ~/ 30));
    state = PlaybackState(state.playing, state.playbackTime, newPos);
  }
}

final playbackStateProvider = StateNotifierProvider<PlaybackStateNotifier, PlaybackState>((ref) {
  return PlaybackStateNotifier();
});

final playingProvider = Provider<bool>((ref) => ref.watch(playbackStateProvider).playing);
final playbackTimeProvider = Provider<int>((ref) => ref.watch(playbackStateProvider).playbackTime);
final playingPositionProvider = Provider<int>((ref) => ref.watch(playbackStateProvider).playingPosition);
