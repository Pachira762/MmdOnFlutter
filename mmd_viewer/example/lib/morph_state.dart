import 'package:flutter_riverpod/flutter_riverpod.dart';

enum MorphCategory { eyeblow, eye, mouth, other }

String getMorphCategoryName(MorphCategory category) {
  switch (category) {
    case MorphCategory.eyeblow:
      return "まゆ";
    case MorphCategory.eye:
      return "目";
    case MorphCategory.mouth:
      return "リップ";
    case MorphCategory.other:
      return "その他";
  }
}

class MorphState {
  final List<String> names;
  late final List<int> eyeblowMorphIndices;
  late final List<int> eyeMorphIndices;
  late final List<int> mouthMorphIndices;
  late final List<int> otherMorphIndices;

  MorphState(this.names, List<int> categories) {
    if (names.length != categories.length) {
      return;
    }

    eyeblowMorphIndices = <int>[];
    eyeMorphIndices = <int>[];
    mouthMorphIndices = <int>[];
    otherMorphIndices = <int>[];

    for (int i = 0; i < categories.length; ++i) {
      switch (categories[i]) {
        case 1:
          eyeblowMorphIndices.add(i);
          break;
        case 2:
          eyeMorphIndices.add(i);
          break;
        case 3:
          mouthMorphIndices.add(i);
          break;
        case 4:
          otherMorphIndices.add(i);
          break;
        //
      }
    }
  }

  List<int> getIndices(MorphCategory category) {
    switch (category) {
      case MorphCategory.eyeblow:
        return eyeblowMorphIndices;
      case MorphCategory.eye:
        return eyeMorphIndices;
      case MorphCategory.mouth:
        return mouthMorphIndices;
      case MorphCategory.other:
        return otherMorphIndices;
    }
  }

  int getMorphCount(MorphCategory category) {
    return getIndices(category).length;
  }

  List<String> getNames(MorphCategory category) {
    return <String>[for (final index in getIndices(category)) names[index]];
  }

  String getName(int index) {
    return names[index];
  }

  int get totalCount {
    return names.length;
  }
}

final morphStateProvider = StateProvider<MorphState>((ref) => MorphState([], []));
