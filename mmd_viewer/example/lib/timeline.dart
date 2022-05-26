import 'dart:math';
import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:mmd_viewer_example/track.dart';
import 'playback_state.dart';
import 'morph_animation.dart';

class Timeline extends ConsumerWidget {
  const Timeline({super.key});

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final length = ref.watch(playbackTimeProvider).toDouble();
    final position = ref.watch(playingPositionProvider).toDouble();
    final trackIndex = ref.watch(editingTrackIndexProvider);

    var rangeMin = max(0.0, position - 90.0);
    var rangeMax = min(length, rangeMin + 120.0);
    rangeMin = rangeMax - 120.0;

    final spots = trackIndex == -1
        ? <FlSpot>[]
        : ref.watch(morphTrackProviders[trackIndex]).keys.map((TrackKey key) => FlSpot(key.frame.toDouble(), key.value)).toList();

    final data = LineChartBarData(spots: spots);
    return ClipRect(
        child: LineChart(
      LineChartData(
          lineBarsData: [data],
          minX: rangeMin,
          maxX: rangeMax,
          minY: -0.00,
          maxY: 1.00,
          baselineX: position,
          titlesData: FlTitlesData(show: false),
          extraLinesData: ExtraLinesData(
              verticalLines: [VerticalLine(x: position, label: VerticalLineLabel(show: true, alignment: Alignment.topRight))]),
          borderData: FlBorderData(show: false),
          gridData: FlGridData(show: true, drawVerticalLine: false),
          backgroundColor: const Color(0x2040a0f0)),
      swapAnimationDuration: const Duration(),
    ));
  }
}
