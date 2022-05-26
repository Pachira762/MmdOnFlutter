import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:mmd_viewer_example/playback_state.dart';
import 'morph_state.dart';
import 'morph_animation.dart';

class MorphControlPanel extends ConsumerStatefulWidget {
  final MorphCategory category;

  const MorphControlPanel(this.category, {super.key});

  @override
  MorphControlPanelState createState() => MorphControlPanelState();
}

class MorphControlPanelState extends ConsumerState<MorphControlPanel> {
  var _morphCount = 0;
  var _favorites = <bool>[];
  var _visibilities = <bool>[];
  var _sortedIndices = <int>[];

  void _update(MorphState morphState) {
    final morphCount = morphState.getMorphCount(widget.category);
    if (morphCount == _morphCount) {
      return;
    }

    _morphCount = morphCount;
    _favorites = List<bool>.filled(_morphCount, false);
    _visibilities = List<bool>.filled(_morphCount, false);
    _sortedIndices = List<int>.generate(_morphCount, (index) => index);
  }

  void setFavorite(int index, bool favorite) {
    setState(() {
      _favorites[index] = favorite;
    });
  }

  void setVisiblity(int index, int morphIndex, bool visible) {
    setState(() {
      _visibilities[index] = visible;
      ref.read(editingTrackIndexProvider.notifier).update((state) => state == morphIndex ? -1 : morphIndex);
    });
  }

  @override
  Widget build(BuildContext context) {
    final morphState = ref.watch(morphStateProvider);
    _update(morphState);

    _sortedIndices.sort((a, b) {
      a -= _favorites[a] ? 1000 : 0;
      b -= _favorites[b] ? 1000 : 0;
      return a.compareTo(b);
    });

    return Column(children: [
      Padding(
        padding: const EdgeInsets.fromLTRB(0, 4, 0, 4),
        child: Center(child: Text(getMorphCategoryName(widget.category))),
      ),
      SizedBox(height: 1, child: Container(color: const Color(0x40808080))),
      Expanded(
          child: ListView.builder(
              padding: const EdgeInsets.only(right: 8),
              controller: ScrollController(),
              itemCount: _morphCount,
              itemBuilder: (context, index) {
                final inCategoryIndex = _sortedIndices[index];
                final morphIndex = morphState.getIndices(widget.category)[inCategoryIndex];
                return _MorphControl(this, inCategoryIndex, morphIndex, morphState.getName(morphIndex));
              })),
    ]);
  }
}

class _MorphControl extends StatelessWidget {
  final MorphControlPanelState parent;
  final int inCategoryIndex;
  final int morphIndex;
  final String name;

  const _MorphControl(this.parent, this.inCategoryIndex, this.morphIndex, this.name);

  @override
  Widget build(BuildContext buildContext) {
    final favorite = parent._favorites[inCategoryIndex];
    final visiblity = parent._visibilities[inCategoryIndex];
    final label = Text(name, overflow: TextOverflow.ellipsis);

    return ListTile(
      leading: Row(children: [
        ToggleButton(
          checked: favorite,
          onChanged: (value) => parent.setFavorite(inCategoryIndex, value),
          child: const Icon(FluentIcons.heart),
        ),
        const SizedBox(
          width: 4,
        ),
        ToggleButton(
          checked: visiblity,
          onChanged: (value) => parent.setVisiblity(inCategoryIndex, morphIndex, value),
          child: const Icon(FluentIcons.edit),
        ),
      ]),
      subtitle: visiblity ? label : null,
      title: visiblity
          ? Consumer(
              builder: (context, ref, child) => Slider(
                  max: 1.0,
                  value: ref.watch(morphValueProviders[morphIndex]),
                  onChanged: (value) =>
                      ref.read(morphTrackProviders[morphIndex].notifier).update(ref.read(playingPositionProvider), value)))
          : label,
    );
  }
}
