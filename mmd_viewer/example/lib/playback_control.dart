import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'playback_state.dart';

class PlaybackControl extends ConsumerWidget {
  const PlaybackControl({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final playing = ref.watch(playingProvider);

    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Slider(
          max: ref.watch(playbackTimeProvider).toDouble(),
          value: ref.watch(playingPositionProvider).toDouble(),
          onChanged: (value) => ref.read(playbackStateProvider.notifier).position = value.toInt(),
        ),
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            SizedBox(
                width: 60,
                child: IconButton(
                    icon: const Icon(FluentIcons.previous),
                    onPressed: () {
                      ref.read(playbackStateProvider.notifier).offset(-1);
                    })),
            SizedBox(
                width: 60,
                child: IconButton(
                    icon: Icon(playing ? FluentIcons.pause : FluentIcons.play),
                    onPressed: () {
                      ref.read(playbackStateProvider.notifier).playing = !playing;
                    })),
            SizedBox(
                width: 60,
                child: IconButton(
                    icon: const Icon(FluentIcons.next),
                    onPressed: () {
                      ref.read(playbackStateProvider.notifier).offset(1);
                    })),
          ],
        )
      ],
    );
  }
}
