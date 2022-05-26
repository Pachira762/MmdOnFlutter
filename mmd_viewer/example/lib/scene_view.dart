import 'dart:typed_data';

import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:mmd_viewer_example/morph_animation.dart';
import 'package:mmd_viewer_example/playback_state.dart';
import 'morph_state.dart';
import 'package:mmd_viewer/mmd_viewer.dart';

class SceneView extends ConsumerWidget {
  final double width;
  final double height;

  const SceneView(this.width, this.height, {super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final plugin = MmdViewer();
    final position = ref.watch(playingPositionProvider);

    final numMorphs = ref.watch(morphStateProvider).totalCount;
    var values = Float32List(numMorphs);
    for (int i = 0; i < numMorphs; ++i) {
      values[i] = ref.watch(morphValueProviders[i]);
    }

    plugin.update(position, values);

    return FittedBox(
      child: SizedBox(
        width: width,
        height: height,
      ),
    );
  }
}
