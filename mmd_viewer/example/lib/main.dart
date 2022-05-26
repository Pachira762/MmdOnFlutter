import 'dart:math';
import 'dart:typed_data';
import 'package:fluent_ui/fluent_ui.dart' hide MenuItem;
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:menubar/menubar.dart';
import 'package:mmd_viewer/mmd_viewer.dart';
import 'scene_view.dart';
import 'timeline.dart';
import 'morph_control.dart';
import 'file_pick.dart';
import 'playback_control.dart';
import 'playback_state.dart';
import 'morph_state.dart';
import 'morph_animation.dart';

List<T> getList<T>(dynamic objList) {
  List<T> typed = [];
  for (final obj in objList) {
    typed.add(obj as T);
  }
  return typed;
}

void main() {
  runApp(const ProviderScope(child: MyApp()));
}

class MyApp extends StatelessWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    WidgetsBinding.instance.addPostFrameCallback((timeStamp) {
      MmdViewer().createChildWindow(1280, 720, margin: const EdgeInsets.fromLTRB(0, 0, 400, 320));
    });

    return const FluentApp(
        title: 'MMD Viewer',
        home: ScaffoldPage(
          padding: EdgeInsets.zero,
          content: _HomePage(),
        ));
  }
}

class _HomePage extends ConsumerStatefulWidget {
  const _HomePage();

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends ConsumerState<_HomePage> {
  void _openModel(String? path) async {
    if (path == null) {
      return;
    }

    final info = await MmdViewer().openModel(path);
    if (info == null) {
      return;
    }

    final names = getList<String>(info["morph_names"]);
    final categories = getList<int>(info["morph_categories"]);
    ref.read(morphStateProvider.notifier).state = MorphState(names, categories);
  }

  void _openScene(String? path) async {
    if (path == null) {
      return;
    }

    final info = await MmdViewer().openScene(path);
    if (info == null) {
      return;
    }

    final numFrames = info["num_frames"] as int;
    final frameTracks = getList<Int32List>(info["frame_tracks"]);
    final valueTracks = getList<Float32List>(info["value_tracks"]);
    if (numFrames == 0 || frameTracks.isEmpty || frameTracks.length != valueTracks.length) {
      return;
    }

    final morphCount = frameTracks.length;
    if (morphCount != ref.read(morphStateProvider).totalCount) {
      return;
    }

    if (morphCount > maxMorphCount) {
      return;
    }

    var maxFrame = 0;
    for (int i = 0; i < morphCount; ++i) {
      ref.read(morphTrackProviders[i].notifier).combine(frameTracks[i], valueTracks[i]);
      maxFrame = max(maxFrame, frameTracks[i].last);
    }

    ref.read(playbackStateProvider.notifier).playbackTime = maxFrame;
  }

  void _saveAnimation(String? path) async {
    if (path == null) {
      return;
    }

    final morphNames = ref.read(morphStateProvider).names;
    final numTracks = morphNames.length;
    if (numTracks == 0) {
      return;
    }

    var frameTracks = <Int32List>[];
    var valueTracks = <Float32List>[];
    for (int i = 0; i < numTracks; ++i) {
      final keys = ref.read(morphTrackProviders[i]).keys;
      final numKeys = keys.length;
      var frames = Int32List(numKeys);
      var values = Float32List(numKeys);
      for (int j = 0; j < numKeys; ++j) {
        frames[j] = keys[j].frame;
        values[j] = keys[j].value;
      }
      frameTracks.add(frames);
      valueTracks.add(values);
    }

    final result = await MmdViewer().saveAnimation(path, morphNames, frameTracks, valueTracks, "hyojo");
    if (result == null || !result) {
      return;
    }

    // succeed
  }

  @override
  void initState() {
    super.initState();

    setApplicationMenu([
      Submenu(label: 'File', children: [
        MenuItem(label: 'Open Model', onClicked: () => pickFile('last_model_path').then((path) => _openModel(path))),
        MenuItem(label: 'Open Scene', onClicked: () => pickFile('last_scene_path').then((path) => _openScene(path))),
        MenuItem(label: 'Save Morph Animation', onClicked: () => pickSaveFile('last_save_morph_path').then((path) => _saveAnimation(path))),
      ])
    ]);
  }

  @override
  Widget build(BuildContext context) {
    return Row(children: [
      Expanded(
        child: Column(children: [
          const Expanded(
            child: SceneView(1280, 720),
          ),
          const SizedBox(
            height: 1,
          ),
          SizedBox(
            height: 320,
            child: Column(children: const [
              Expanded(
                child: Timeline(),
              ),
              Padding(padding: EdgeInsets.fromLTRB(8, 8, 8, 12), child: PlaybackControl()),
            ]),
          ),
        ]),
      ),
      SizedBox(
        width: 1,
        child: Container(color: Colors.grey),
      ),
      SizedBox(
          width: 400,
          child: Column(
            children: [
              Expanded(
                child: Row(children: const [
                  Expanded(
                    child: MorphControlPanel(MorphCategory.eye),
                  ),
                  Expanded(
                    child: MorphControlPanel(MorphCategory.mouth),
                  ),
                ]),
              ),
              Expanded(
                  child: Row(children: const [
                Expanded(
                  child: MorphControlPanel(MorphCategory.eyeblow),
                ),
                Expanded(
                  child: MorphControlPanel(MorphCategory.other),
                ),
              ])),
            ],
          )),
    ]);
  }
}
