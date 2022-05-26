import 'package:file_selector/file_selector.dart';
import 'package:shared_preferences/shared_preferences.dart';

String? getLastDirectory(SharedPreferences prefs, String key) {
  final path = prefs.getString(key);
  if (path == null) {
    return null;
  }

  final lastIndex = path.lastIndexOf('\\');
  final dir = path.substring(0, lastIndex == -1 ? null : (lastIndex + 1));

  return dir;
}

void setLastPath(SharedPreferences prefs, String key, String path) {
  prefs.setString(key, path);
}

Future<String?> pickFile(String tag, [List<XTypeGroup> acceptedTypeGroups = const <XTypeGroup>[]]) async {
  final prefs = await SharedPreferences.getInstance();
  final dir = getLastDirectory(prefs, tag);

  final file = await openFile(acceptedTypeGroups: acceptedTypeGroups, initialDirectory: dir);

  if (file == null) {
    return null;
  }

  setLastPath(prefs, tag, file.path);
  return file.path;
}

Future<String?> pickSaveFile(String tag, [List<XTypeGroup> acceptedTypeGroups = const <XTypeGroup>[]]) async {
  final prefs = await SharedPreferences.getInstance();
  final dir = getLastDirectory(prefs, tag);

  final path = await getSavePath(acceptedTypeGroups: acceptedTypeGroups, initialDirectory: dir);

  if (path == null) {
    return null;
  }

  setLastPath(prefs, tag, path);
  return path;
}
