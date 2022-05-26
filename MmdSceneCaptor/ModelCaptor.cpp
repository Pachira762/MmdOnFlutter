#include "pch.h"
#include "ModelCaptor.h"

ModelCaptor::ModelCaptor(int model) :
	model_index_(model),
	num_bones_(ExpGetPmdBoneNum(model)),
	num_morphs_(ExpGetPmdMorphNum(model)) {
}

void ModelCaptor::Capture(int frame_no) {
	auto& frame = AcquireFrame(frame_no);

	for (int i = 0; i < num_bones_; ++i) {
		frame.transforms[i] = ExpGetPmdBoneWorldMat(model_index_, i);
	}
}

void ModelCaptor::Save(FileWriter& writer) {
	const char* name = ExpGetPmdFilename(model_index_);
	if (!name) {
		int32_t dummy = 0;
		writer.Write(dummy); // name len
		writer.Write(dummy); // num bones
		writer.Write(dummy); // num morphs
		writer.Write(dummy); // num frames
		return;
	}

	SaveInfo(writer);
	SaveBones(writer);
	SaveMorphs(writer);
}

void ModelCaptor::SaveInfo(FileWriter& writer) {
	writer.WriteString(ExpGetPmdFilename(model_index_));
}

void ModelCaptor::SaveBones(FileWriter& writer) {
	writer.Write(num_bones_);

	for (int bi = 0; bi < num_bones_; ++bi) {
		writer.WriteString(ExpGetPmdBoneName(model_index_, bi));

		auto num_frames = static_cast<int32_t>(frames_.size());
		writer.Write(num_frames);

		for (int fi = 0; fi < num_frames; ++fi) {
			writer.Write(frames_[fi].transforms[bi]);
		}
	}
}

void ModelCaptor::SaveMorphs(FileWriter& writer) {
	using Key = std::pair<int, float>;
	static_assert(sizeof(Key) == 8);

	const auto data = mmp::getMMDMainData()->model_data[model_index_];
	const auto num_morphs = static_cast<int32_t>(data->morph_count);
	const auto keyframes = data->morph_keyframe;

	writer.Write(num_morphs);

	for (int mi = 0; mi < num_morphs; ++mi) {
		writer.WriteString(ExpGetPmdMorphName(model_index_, mi));

		std::vector<Key> keys{};
		for (auto key = &keyframes[mi];;) {
			keys.emplace_back(key->frame_number, key->value);
			
			if (key->next_index == 0) {
				break;
			}

			key = &keyframes[key->next_index];
		}

		auto num_keys = static_cast<int32_t>(keys.size());
		writer.Write(num_keys);
		writer.WriteArray(keys.data(), num_keys);
	}
}

ModelCaptor::Frame& ModelCaptor::AcquireFrame(int frame_no) {
	auto num_frames = frames_.size();
	auto num_request = frame_no + 1;

	if (num_frames < num_request) {
		frames_.resize(num_request);

		for (auto i = num_frames; i < num_request; ++i) {
			frames_[i].transforms.resize(num_bones_);
		}
	}

	return frames_[frame_no];
}
