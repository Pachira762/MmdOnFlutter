#include "pch.h"
#include "MmdScene.h"

namespace headless_mmd {

std::shared_ptr<MmdScene> MmdSceneImporter::Import(const std::wstring& path) {
	auto scene = std::make_shared<MmdScene>();
	auto& animation_tracks = scene->animation_tracks_;
	auto& camera_track = scene->camera_track_;

	auto bin = portable_mmd::io::LoadBinary(path);
	if (bin.empty()) {
		return nullptr;
	}
	portable_mmd::io::Reader reader(bin.data(), bin.size());

	auto regulation = reader.Uint32();
	if (regulation != 0b0111) {
		return nullptr;
	}

	auto num_animations = reader.Int32();
	DLOG(L"{} animations", num_animations);

	animation_tracks.resize(num_animations);

	for (int ai = 0; ai < num_animations; ++ai) {
		auto model_name = SjisToUtf16(reader.TextA());
		auto& animation_track = animation_tracks[ai];
		auto& bone_animation = animation_track.bone_animation;
		auto& morph_animation = animation_track.morph_animation;

		auto num_bones = reader.Int32();
		DLOG(L"{} bones", num_bones);

		for (int bi = 0; bi < num_bones; ++bi) {
			auto name = SjisToUtf16(reader.TextA());
			auto& track = bone_animation[name];

			auto num_frames = reader.Int32();
			track.resize(num_frames);

			for (int fi = 0; fi < num_frames; ++fi) {
				track[fi] = reader.As<Matrix>();
			}
		}

		auto num_morphs = reader.Int32();
		DLOG(L"{} morph", num_morphs);

		for (int mi = 0; mi < num_morphs; ++mi) {
			auto name = SjisToUtf16(reader.TextA());
			auto& keys = morph_animation[name].keys;

			auto num_keys = reader.Int32();
			keys.resize(num_keys);

			for (int ki = 0; ki < num_keys; ++ki) {
				keys[ki].frame= reader.Uint32();
				keys[ki].value = reader.Float();
			}
		}
	}

	{
		auto& keys = camera_track.keys;
		auto num_camera_keys = reader.Uint32();
		keys.resize(num_camera_keys);
		for (auto& [frame, key] : keys) {
			frame = reader.Uint32();
			key.frame = frame;
			reader >> key.distance;
			reader.ForVec3() >> key.position;
			reader.ForVec3() >> key.rotation;
			reader >> key.ix >> key.iy >> key.iz;
			reader >> key.ir >> key.id >> key.iv;
			reader >> key.view_angle;
			reader.For<uint8_t>() >> key.parallel;
		}
		std::ranges::sort(keys, {}, &Key<Vmd::CameraKey>::frame);
	}

	return scene;
}

}