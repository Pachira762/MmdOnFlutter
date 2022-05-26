#pragma once
#include <vector>
#include "FileWriter.h"

class ModelCaptor {
public:
	ModelCaptor(int model);

	void Capture(int frame_no);
	void Save(FileWriter& writer);

private:
	struct Frame {
		std::vector<D3DMATRIX>	transforms;
	};

	void SaveInfo(FileWriter& writer);
	void SaveBones(FileWriter& writer);
	void SaveMorphs(FileWriter& writer);

	Frame& AcquireFrame(int frame_no);

	int32_t model_index_;
	int32_t	num_bones_;
	int32_t num_morphs_;
	std::vector<Frame> frames_;
};
