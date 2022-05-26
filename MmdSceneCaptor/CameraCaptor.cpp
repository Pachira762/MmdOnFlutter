#include "pch.h"
#include "CameraCaptor.h"

void CameraCaptor::Save(FileWriter& writer) {
	auto data = mmp::getMMDMainData();
	if (!data) {
		return;
	}

	uint32_t num_frames = 0;
	for (int frame_no = 0;;) {
		const auto& frame = data->camera_key_frame[frame_no];

		++num_frames;

		frame_no = frame.next_index;
		if (frame_no == 0) {
			break;
		}
	}
	writer.Write(num_frames);

	for (int frame_no = 0; true;) {
		auto& frame = data->camera_key_frame[frame_no];

		writer.Write(static_cast<uint32_t>(frame.frame_no));
		writer.Write(frame.length);
		writer.Write(frame.xyz);
		writer.Write(frame.rxyz);
		writer.Write(frame.hokan1_x[0]); writer.Write(frame.hokan2_x[0]); writer.Write(frame.hokan1_y[0]); writer.Write(frame.hokan2_y[0]);
		writer.Write(frame.hokan1_x[1]); writer.Write(frame.hokan2_x[1]); writer.Write(frame.hokan1_y[1]); writer.Write(frame.hokan2_y[1]);
		writer.Write(frame.hokan1_x[2]); writer.Write(frame.hokan2_x[2]); writer.Write(frame.hokan1_y[2]); writer.Write(frame.hokan2_y[2]);
		writer.Write(frame.hokan1_x[3]); writer.Write(frame.hokan2_x[3]); writer.Write(frame.hokan1_y[3]); writer.Write(frame.hokan2_y[3]);
		writer.Write(frame.hokan1_x[4]); writer.Write(frame.hokan2_x[4]); writer.Write(frame.hokan1_y[4]); writer.Write(frame.hokan2_y[4]);
		writer.Write(frame.hokan1_x[5]); writer.Write(frame.hokan2_x[5]); writer.Write(frame.hokan1_y[5]); writer.Write(frame.hokan2_y[5]);
		writer.Write(static_cast<int32_t>(frame.view_angle));
		writer.Write(static_cast<uint8_t>(!frame.is_perspective));

		frame_no = frame.next_index;
		if (frame_no == 0) {
			break;
		}
	}
}

/*
Vmd Camera Format

struct Interpolation {
	char x1;
	char x2;
	char y1;
	char y2;
};

struct CameraKey {
	uint32_t		frame;
	float			distance;
	Vec3			position;
	Vec3			rotation;
	Interpolation	ix;
	Interpolation	iy;
	Interpolation	iz;
	Interpolation	ir;
	Interpolation	id;
	Interpolation	iv;
	int32_t			view_angle;
	bool			parallel_projection;
};
*/