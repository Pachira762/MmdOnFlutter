#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>

// portable header-only mmd
namespace portable_mmd {
//
// Common
//
struct Interpolation {
	int8_t	x1;
	int8_t	x2;
	int8_t	y1;
	int8_t	y2;
};

inline constexpr Interpolation DefaultInterpolation{
	20,107,20,107
};

template<int N = 8>
inline constexpr float BezierInterp(float x, float x1, float x2, float y1, float y2) noexcept {
	auto Bezier = [](float t, float p1, float p2) noexcept {
		const float t2 = t * t;
		const float t3 = t2 * t;
		return 3.f * p1 * (t3 - 2.f * t2 + t) + 3.f * p2 * (-t3 + t2) + t3;
	};

	auto Bezier1 = [](float t, float p1, float p2) noexcept {
		const float t2 = t * t;
		return 3.f * p1 * (3.f * t2 - 4.f * t + 1.f) + 3.f * p2 * (-3.f * t2 + 2.f * t) + 3 * t2;
	};

	float t = x;
	for (int i = 0; i < N; ++i) {
		float ft = Bezier(t, x1, x2) - x;
		float ft1 = Bezier1(t, x1, x2);
		t = t - (ft / ft1);
	}

	return Bezier(t, y1, y2);
}

template<int N = 8>
inline constexpr float BezierInterp(float x, const Interpolation& interp) noexcept {
	return BezierInterp<N>(x, static_cast<float>(interp.x1) / 127.f, static_cast<float>(interp.x2) / 127.f, static_cast<float>(interp.y1) / 127.f, static_cast<float>(interp.y2) / 127.f);
}

#pragma warning(push)
#pragma warning( disable : 4324)
//
// Pmx
//
inline constexpr char PmxMagic[4] = { 0x50,0x4d,0x58,0x20 };
inline constexpr float PmxVersion = 2.0f;

enum class PmxEncodeType : uint8_t {
	Utf16 = 0,
	Utf8 = 1,
};

enum class PmxWeightType : uint8_t {
	BDEF1 = 0,
	BDEF2 = 1,
	BDEF4 = 2,
	SDEF = 3,
};

enum class PmxMorphType : uint8_t {
	Group = 0,
	Vertex = 1,
	Bone = 2,
	UV = 3,
	ExUV1 = 4,
	ExUV2 = 5,
	ExUV3 = 6,
	ExUV4 = 7,
	Material = 8,
};

enum class PmxMorphPanel : uint8_t {
	Eyeblow = 1,
	Eye = 2,
	Mouth = 3,
	Other = 4
};

enum class PmxMaterialMorphOp : uint8_t {
	Mult = 0,
	Add = 1,
};

enum class PmxNodeItemType : uint8_t {
	Bone = 0,
	Morph = 1
};

enum class PmxBodyPhysicsType : uint8_t {
	Static = 0,
	Dynamic = 1,
	Combine = 2,
};

enum class PmxJointType : uint8_t {
	Spring6DOF = 0,
};

template<typename Vec2, typename Vec3, typename Vec4>
struct Pmx {
	struct Header {
		float			version;
		uint8_t			num_ex_data;
		PmxEncodeType	encode;
		uint8_t			num_ex_uvs;
		uint8_t			vertex_index_size;
		uint8_t			texture_index_size;
		uint8_t			material_index_size;
		uint8_t			bone_index_size;
		uint8_t			morph_index_size;
		uint8_t			body_index_size;
		std::wstring	name;
		std::wstring	name_en;
		std::wstring	comment;
		std::wstring	comment_en;
	};

	struct Vertex {
		Vec3			position;
		Vec3			normal;
		Vec2			uv;
		Vec4			ex_uvs[4];
		PmxWeightType	weight_type;
		int32_t			bone_indices[4];
		float			bone_weights[4];
		Vec3			sdef_c;
		Vec3			sdef_r0;
		Vec3			sdef_r1;
		float			edge;
	};

	using Texture = std::wstring;

	struct Material {
		std::wstring	name;
		std::wstring	name_en;
		Vec4			diffuse;
		Vec4			specular;
		Vec3			ambient;
		bool			twoside;
		bool			cast_ground_shadow;
		bool			cast_self_shadow;
		bool			receive_self_shadow;
		bool			draw_edge;
		Vec4			edge_color;
		float			edge_size;
		int32_t			base_texture_index;
		int32_t			sphere_texture_index;
		uint8_t			sphere_mode;
		bool			use_shared_toon;
		int32_t			toon_texture_index;
		std::wstring	note;
		int32_t			num_vertices;
	};

	struct Bone {
		struct IkLink {
			int32_t		index;
			bool		angle_limited;
			Vec3		angle_min;
			Vec3		angle_max;
		};

		std::wstring	name;
		std::wstring	name_en;
		Vec3			position;
		int32_t			parent_bone_index;
		int32_t			level;
		bool			has_tip_bone;
		bool			rotatable;
		bool			translatable;
		bool			visible;
		bool			operable;
		bool			is_ik;
		bool			local_driven;
		bool			driven_rotation;
		bool			driven_translation;
		bool			has_fixed_axis;
		bool			has_local_axis;
		bool			post_physics_transform;
		bool			external_transform;
		Vec3			tip_offset;
		int32_t			tip_bone_index;
		int32_t			drive_bone_index;
		float			drive_rate;
		Vec3			fixed_axis;
		Vec3			local_axis_x;
		Vec3			local_axis_z;
		int32_t			external_key;
		int32_t			ik_target_bone_index;
		int32_t			ik_iteration_count;
		float			ik_angle_limit;
		std::vector<IkLink>	ik_links;
	};

	struct GroupMorphData {
		int32_t		index;
		float		rate;
	};

	struct VertexMorphData {
		int32_t		index;
		Vec3		offset;
	};

	struct BoneMorphData {
		int32_t		index;
		Vec3		translation;
		Vec4		rotation;
	};

	struct UvMorphData {
		int32_t		index;
		Vec4		offset;
	};

	struct MaterialMorphData {
		int32_t				index;
		PmxMaterialMorphOp	mode;
		Vec4				diffuse;
		Vec4				specular;
		Vec3				ambient;
		Vec4				edge_color;
		float				edge_size;
		Vec4				base_texture_coef;
		Vec4				sphere_texture_coef;
		Vec4				toon_texture_coef;
	};

	template<typename Data>
	struct Morph {
		std::wstring	name;
		std::wstring	name_en;
		PmxMorphPanel		panel;
		PmxMorphType		type;
		std::vector<Data>	data;
	};
	using GroupMorph = Morph<GroupMorphData>;
	using VertexMorph = Morph<VertexMorphData>;
	using BoneMorph = Morph<BoneMorphData>;
	using UvMorph = Morph<UvMorphData>;
	using MaterialMorph = Morph<MaterialMorphData>;

	struct Node {
		struct Item {
			PmxNodeItemType type;
			int32_t	 index;
		};

		std::wstring		name;
		std::wstring		name_en;
		bool				is_special;
		std::vector<Item>	items;
	};

	struct Body {

		std::wstring	name;
		std::wstring	name_en;
		int32_t			index;
		uint8_t			group;
		uint16_t		non_collision_group;
		uint8_t			shape;
		Vec3			size;
		Vec3			position;
		Vec3			rotation;
		float			mass;
		float			translation_atten;
		float			rotation_atten;
		float			restitution;
		float			friction;
		PmxBodyPhysicsType	physics_type;
	};

	struct Joint {
		std::wstring	name;
		std::wstring	name_en;
		PmxJointType	type;
		int32_t			body_index_a;
		int32_t			body_index_b;
		Vec3			position;
		Vec3			rotation;
		Vec3			translation_min;
		Vec3			translation_max;
		Vec3			rotation_min;
		Vec3			rotation_max;
		Vec3			sprint_const_translation;
		Vec3			sprint_const_rotation;
	};

	Header					header;
	std::vector<Vertex>		vertices;
	std::vector<int32_t>	indices;
	std::vector<Texture>	textures;
	std::vector<Material>	materials;
	std::vector<Bone>		bones;
	std::vector<VertexMorph>	vertex_morphs;
	std::vector<UvMorph>		uv_morphs;
	std::vector<BoneMorph>		bone_morphs;
	std::vector<MaterialMorph>	material_morphs;
	std::vector<GroupMorph>		group_morphs;
	std::vector<Node>	nodes;
	std::vector<Body>	bodies;
	std::vector<Joint>	joints;
};

//
// Vmd
//
inline constexpr char VmdMagic[30] = "Vocaloid Motion Data 0002\0\0\0\0";

template<typename Vec3, typename Vec4>
struct Vmd {
	struct Header {
		std::string		name;
	};

	struct MotionKey {
		uint32_t		frame;
		Vec3			position;
		Vec4			orientation;
		Interpolation	ix;
		Interpolation	iy;
		Interpolation	iz;
		Interpolation	ir;
		char			interpolation[64];
		bool			physics;
	};

	struct MorphKey {
		uint32_t		frame;
		float			value;
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
		bool			parallel;
	};

	struct LightKey {
		uint32_t	frame;
		Vec3		color;
		Vec3		position;
	};

	struct ShadowKey {
		uint32_t	frame;
		int8_t		type;
		float		distance;
	};

	struct IkKey {
		uint32_t	frame;
		bool		enable;
	};

	struct VisibilityKey {
		uint32_t	frame;
		bool		visible;
	};

	Header	header;
	std::map<std::string, std::vector<MotionKey>>	motions;
	std::map<std::string, std::vector<MorphKey>>	morphs;
	std::vector<CameraKey>		cameras;
	std::vector<LightKey>		lights;
	std::vector<ShadowKey>		shadows;
	std::vector<VisibilityKey>	visibilities;
	std::map<std::string, std::vector<IkKey>>	iks;

	static constexpr MotionKey DefaultMotionKey{
		0,
		{},
		{},
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		{
			20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107,
			20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 0,
			20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 0, 0,
			20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 107, 20, 0, 0, 0
		},
		false
	};

	static constexpr CameraKey DefaultCameraKey = {
		0,
		45.f,
		{0.f, 10.f, 0.f},
		{},
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		DefaultInterpolation,
		30,
		false
	};
};
#pragma warning(pop)

namespace io {
//
// Reader
//
struct ReaderBase {
	const char* cur;
	std::size_t remain;
	bool overflow;

	bool IsOverflow(std::size_t size) noexcept {
		if (remain < size) {
			remain = 0;
			overflow = true;
			return true;
		}

		return false;
	}

	void Increment(std::size_t size) noexcept {
		cur += size;
		remain -= size;
	}

	template<typename T, std::size_t N>
	void Copy(T(&out)[N]) noexcept {
		static_assert(std::is_trivially_copyable_v<T>);

		constexpr auto size = sizeof(T) * N;
		if (IsOverflow(size)) {
			return;
		}

		std::memcpy(out, cur, size);
		Increment(size);
	}

	template<typename T>
	void Copy(T* out, std::size_t num) {
		static_assert(std::is_trivially_copyable_v<T>);

		const auto size = sizeof(T) * num;
		if (IsOverflow(size)) {
			return;
		}

		std::memcpy(out, cur, size);
		Increment(size);
	}

	template<typename From, typename To, std::size_t N>
	void Cast(To(&out)[N]) noexcept {
		static_assert(std::is_trivially_copyable_v<From>);

		if (IsOverflow(sizeof(From) * N)) {
			return;
		}

		for (std::size_t i = 0; i < N; ++i) {
			From src{};
			std::memcpy(&src, cur, sizeof(From));
			Increment(sizeof(From));

			out[i] = static_cast<To>(src);
		}
	}

	template<typename From, typename To>
	void Cast(To* out, std::size_t num) {
		static_assert(std::is_trivially_copyable_v<From>);

		if (IsOverflow(sizeof(From) * num)) {
			return;
		}

		for (std::size_t i = 0; i < num; ++i) {
			From src{};
			std::memcpy(&src, cur, sizeof(From));
			Increment(sizeof(From));

			out[i] = static_cast<To>(src);
		}
	}
};
static_assert(std::is_standard_layout_v<ReaderBase>);

template<typename From>
struct CastReader : ReaderBase {
};

template<typename From, typename To>
inline CastReader<From>& operator>>(CastReader<From>& reader, To& out) noexcept {
	reader.Cast<From, To, 1>(reinterpret_cast<To(&)[1]>(out));
	return reader;
}

template<typename From, typename To, std::size_t N>
inline CastReader<From>& operator>>(CastReader<From>& reader, To(&out)[N]) noexcept {
	reader.Cast<From, To, N>(out);
	return reader;
}

template<std::size_t N>
struct VectorReader : ReaderBase {
};

template<typename T, std::size_t N>
inline VectorReader<N>& operator>>(VectorReader<N>& reader, T& out) noexcept {
	static_assert(sizeof(T) >= sizeof(float) * N);

	reader.Copy<float, N>(reinterpret_cast<float(&)[N]>(out));
	return reader;
}

template<std::size_t N>
struct FixedLengthStringReader : ReaderBase {
};

template<typename Char, std::size_t N>
inline FixedLengthStringReader<N>& operator>>(FixedLengthStringReader<N>& reader, std::basic_string<Char>& out) {
	constexpr auto size = sizeof(Char) * N;
	if (reader.IsOverflow(size)) {
		return reader;
	}

	out.assign(reader.cur, N);
	reader.Increment(size);

	return reader;
}

class Reader : private ReaderBase {
public:
	Reader(const void* data, std::size_t size) :
		ReaderBase{ reinterpret_cast<const char*>(data), size, false } {
	}

	template<typename From, typename To = From>
	To As() noexcept {
		To out{};
		if constexpr (std::is_same_v<From, To>) {
			Copy<To, 1>(reinterpret_cast<To(&)[1]>(out));
		}
		else {
			Cast<From, To, 1>(reinterpret_cast<To(&)[1]>(out));
		}
		return out;
	}

	template<typename T, std::size_t N>
	void Array(T(&dest)[N]) noexcept {
		Copy<T, N>(dest);
	}

	template<typename T>
	void Array(T* dest, std::size_t num) {
		Copy<T>(dest, num);
	}

	template<typename To, typename From, std::size_t N>
	void ArrayFor(To(&dest)[N]) noexcept {
		Cast<From, To, N>(dest);
	}

	template<typename From, typename To>
	void ArrayFor(To* dest, std::size_t num) {
		Cast<From, To>(dest, num);
	}

	int8_t Int8() noexcept {
		return As<int8_t>();
	}

	int16_t Int16() noexcept {
		return As<int16_t>();
	}

	int32_t Int32() noexcept {
		return As<int32_t>();
	}

	int32_t Int(const uint8_t size) noexcept {
		switch (size) {
		case 1: return Int8();
		case 2: return Int16();
		case 4: return Int32();
		default: return 0;
		}
	}

	uint8_t Uint8() noexcept {
		return As<uint8_t>();
	}

	uint16_t Uint16() noexcept {
		return As<uint16_t>();
	}

	uint32_t Uint32() noexcept {
		return As<uint32_t>();
	}

	uint32_t Uint(const uint8_t size) noexcept {
		switch (size) {
		case 1: return Uint8();
		case 2: return Uint16();
		case 4: return Uint32();
		default: return 0;
		}
	}

	float Float() noexcept {
		return As<float>();
	}

	std::initializer_list<float> Float2() noexcept {
		return {
			As<float>(),
			As<float>(),
		};
	}

	std::initializer_list<float> Float3() noexcept {
		return {
			As<float>(),
			As<float>(),
			As<float>(),
		};
	}

	std::initializer_list<float> Float4() noexcept {
		return {
			As<float>(),
			As<float>(),
			As<float>(),
			As<float>(),
		};
	}

	std::initializer_list<float> Float4x4() noexcept {
		return {
			As<float>(),As<float>(),As<float>(),As<float>(),
			As<float>(),As<float>(),As<float>(),As<float>(),
			As<float>(),As<float>(),As<float>(),As<float>(),
			As<float>(),As<float>(),As<float>(),As<float>(),
		};
	}

	template<typename Char>
	std::basic_string<Char> Text(uint32_t len) {
		const auto size = sizeof(Char) * len;
		if (IsOverflow(size)) {
			return {};
		}

		auto ptr = reinterpret_cast<const Char*>(cur);
		Increment(size);

		return std::basic_string<Char>(ptr, len);
	}

	template<typename Char>
	std::basic_string<Char> Text() {
		const auto len = As<uint32_t>() / static_cast<uint32_t>(sizeof(Char));
		return Text<Char>(len);
	}

	std::string TextA(uint32_t len) {
		return Text<char>(len);
	}

	std::string TextA() {
		return Text<char>();
	}

	std::wstring TextW(uint32_t len) {
		return Text<wchar_t>(len);
	}

	std::wstring TextW() {
		return Text<wchar_t>();
	}

	template<typename T>
	CastReader<T>& For() noexcept {
		return *reinterpret_cast<CastReader<T>*>(this);
	}

	VectorReader<2>& ForVec2() noexcept {
		return *reinterpret_cast<VectorReader<2>*>(this);
	}

	VectorReader<3>& ForVec3() noexcept {
		return *reinterpret_cast<VectorReader<3>*>(this);
	}

	VectorReader<4>& ForVec4() noexcept {
		return *reinterpret_cast<VectorReader<4>*>(this);
	}

	template<int N>
	FixedLengthStringReader<N>& ForFixed() noexcept {
		return *reinterpret_cast<FixedLengthStringReader<N>*>(this);
	}

	operator bool() const noexcept {
		return !overflow;
	}
};
static_assert(std::is_standard_layout_v<Reader>);

template<typename T>
inline Reader& operator>>(Reader& reader, T& out) noexcept {
	out = reader.As<T>();
	return reader;
}

template<typename T, std::size_t N>
inline Reader& operator>>(Reader& reader, T(&out)[N]) noexcept {
	reader.Array<T, N>(out);
	return reader;
}

template<typename Char>
inline Reader& operator>>(Reader& reader, std::basic_string<Char>& out) {
	out = reader.Text<Char>();
	return reader;
}

//
// Writer
//
struct WriterBase {
	std::vector<char>* buf;
	std::size_t cur;

	WriterBase(std::vector<char>& dest) :
		buf(&dest),
		cur(dest.size()) {
	}

	template<typename Src>
	void Copy(const Src* src, std::size_t num) {
		const auto size = sizeof(Src) * num;
		buf->resize(cur + size);
		std::memcpy(buf->data() + cur, src, size);
		cur += size;
	}

	template<typename Src>
	void Test(std::size_t num) {
		const auto size = sizeof(Src) * num;
		buf->resize(cur + size);
		cur += size;
	}

	void Test2(std::size_t size) {
		buf->resize(cur + size);
		cur += size;
	}
};
static_assert(std::is_standard_layout_v<WriterBase>);

template<int N>
struct VectorWriter : WriterBase {
};

template<typename T, int N>
inline VectorWriter<N>& operator<<(VectorWriter<N>& writer, const T& in) {
	writer.Copy<float>(reinterpret_cast<const float*>(&in), N);
	return writer;
}

template<int N>
struct FixedLengthStringWriter : WriterBase {
};

template<typename Char, int N>
inline FixedLengthStringWriter<N>& operator<<(FixedLengthStringWriter<N>& writer, const std::basic_string<Char>& str) {
	constexpr auto size = sizeof(Char) * N;
	auto& buf = writer.buf;
	auto& cur = writer.cur;
	buf->resize(cur + size);
	std::memcpy(buf->data() + cur, str.c_str(), std::min<std::size_t>(sizeof(Char) * str.length(), size));
	cur += size;
	return writer;
}

struct Writer : WriterBase {
	using WriterBase::WriterBase;

	template<typename T>
	void Write(const T& in) {
		Copy(&in, 1);
	}

	template<typename T, std::size_t N>
	void WriteArray(const T(&in)[N]) {
		Copy(in, N);
	}

	template<typename T>
	void WriteArray(const T* in, std::size_t num) {
		Copy(in, num);
	}

	template<typename Char>
	void WriteText(const std::basic_string<Char>& in, uint32_t len) {
		const auto size = static_cast<uint32_t>(sizeof(Char)) * len;
		buf->resize(cur + size);
		std::memcpy(buf->data() + cur, in.c_str(), std::min<std::size_t>(sizeof(Char) * in.length(), size));
		cur += size;
	}

	template<typename Char>
	void WriteText(const std::basic_string<Char>& in) {
		Write(static_cast<uint32_t>(sizeof(Char) * in.length()));
		WriteText(in, static_cast<uint32_t>(in.length()));
	}

	VectorWriter<2>& ForVec2() {
		return *reinterpret_cast<VectorWriter<2>*>(this);
	}

	VectorWriter<3>& ForVec3() {
		return *reinterpret_cast<VectorWriter<3>*>(this);
	}

	VectorWriter<4>& ForVec4() {
		return *reinterpret_cast<VectorWriter<4>*>(this);
	}

	template<int N>
	FixedLengthStringWriter<N>& ForFixed() {
		return *reinterpret_cast<FixedLengthStringWriter<N>*>(this);
	}
};
static_assert(std::is_standard_layout_v<Writer>);

template<typename T>
inline Writer& operator<<(Writer& writer, const T& in) {
	writer.Write(in);
	return writer;
}

template<typename T, std::size_t N>
inline Writer& operator<<(Writer& writer, const T(&in)[N]) {
	writer.WriteArray<T, N>(in);
	return writer;
}

template<typename Char>
inline Writer& operator<<(Writer& writer, const std::basic_string<Char>& str) {
	writer.WriteText(str);
	return writer;
}

//
// Load Pmx
//
template<typename Vec2, typename Vec3, typename Vec4>
struct PmxLoader {
	using PmxTy = Pmx<Vec2, Vec3, Vec4>;

	PmxTy& pmx;
	Reader reader;

	PmxLoader(PmxTy& pmx, const void* data, std::size_t size) :
		pmx(pmx),
		reader(data, size) {
	}

	bool LoadHeader(typename PmxTy::Header& header) {
		char magic[4]{};
		reader >> magic;
		if (std::memcmp(magic, PmxMagic, 4) != 0) {
			return false;
		}

		reader >> header.version;
		if (header.version != PmxVersion) {
			return false;
		}

		reader >> header.num_ex_data;
		if (header.num_ex_data != 8) {
			return false;
		}

		reader >> header.encode;
		reader >> header.num_ex_uvs;
		reader >> header.vertex_index_size;
		reader >> header.texture_index_size;
		reader >> header.material_index_size;
		reader >> header.bone_index_size;
		reader >> header.morph_index_size;
		reader >> header.body_index_size;

		if (header.encode != PmxEncodeType::Utf16) {
			return false;
		}

		if (header.num_ex_uvs > 4) {
			return false;
		}

		auto IsValidIndexSize = [](uint8_t index_size) {
			switch (index_size) {
			case 1:
			case 2:
			case 4:
				return true;
			default:
				return false;
			}
		};

		bool is_valid_index_size = IsValidIndexSize(header.vertex_index_size) &&
			IsValidIndexSize(header.texture_index_size) &&
			IsValidIndexSize(header.material_index_size) &&
			IsValidIndexSize(header.bone_index_size) &&
			IsValidIndexSize(header.morph_index_size) &&
			IsValidIndexSize(header.body_index_size);
		if (!is_valid_index_size) {
			return false;
		}

		reader >> header.name;
		reader >> header.name_en;
		reader >> header.comment;
		reader >> header.comment_en;

		return static_cast<bool>(reader);
	}

	template<typename BoneIndex>
	bool LoadVertices() {
		const auto num_vertices = reader.Int32();
		if (num_vertices < 0) {
			return false;
		}

		pmx.vertices.resize(num_vertices);
		for (auto& vertex : pmx.vertices) {
			reader.ForVec3() >> vertex.position;
			reader.ForVec3() >> vertex.normal;
			reader.ForVec2() >> vertex.uv;

			for (int i = 0; i < pmx.header.num_ex_uvs; ++i) {
				reader.ForVec4() >> vertex.ex_uvs[i];
			}

			reader.For<PmxWeightType>() >> vertex.weight_type;
			switch (vertex.weight_type) {
			case PmxWeightType::BDEF1:
				reader.For<BoneIndex>() >> vertex.bone_indices[0];
				vertex.bone_indices[1] = -1;
				vertex.bone_indices[2] = -1;
				vertex.bone_indices[3] = -1;

				vertex.bone_weights[0] = 1.f;
				vertex.bone_weights[1] = 0.f;
				vertex.bone_weights[2] = 0.f;
				vertex.bone_weights[3] = 0.f;
				break;

			case PmxWeightType::BDEF2:
				reader.For<BoneIndex>() >> vertex.bone_indices[0];
				reader.For<BoneIndex>() >> vertex.bone_indices[1];
				vertex.bone_indices[2] = -1;
				vertex.bone_indices[3] = -1;

				reader >> vertex.bone_weights[0];
				vertex.bone_weights[1] = 1.f - vertex.bone_weights[0];
				vertex.bone_weights[2] = 0.f;
				vertex.bone_weights[3] = 0.f;
				break;

			case PmxWeightType::BDEF4:
				reader.For<BoneIndex>() >> vertex.bone_indices[0];
				reader.For<BoneIndex>() >> vertex.bone_indices[1];
				reader.For<BoneIndex>() >> vertex.bone_indices[2];
				reader.For<BoneIndex>() >> vertex.bone_indices[3];

				reader >> vertex.bone_weights[0];
				reader >> vertex.bone_weights[1];
				reader >> vertex.bone_weights[2];
				reader >> vertex.bone_weights[3];
				break;

			case PmxWeightType::SDEF:
				reader.For<BoneIndex>() >> vertex.bone_indices[0];
				reader.For<BoneIndex>() >> vertex.bone_indices[1];
				vertex.bone_indices[2] = -1;
				vertex.bone_indices[3] = -1;

				reader >> vertex.bone_weights[0];
				vertex.bone_weights[1] = 0.f;
				vertex.bone_weights[2] = 0.f;
				vertex.bone_weights[3] = 0.f;

				reader.ForVec3() >> vertex.sdef_c;
				reader.ForVec3() >> vertex.sdef_r0;
				reader.ForVec3() >> vertex.sdef_r1;
				break;

			default:
				return false;
			}

			reader >> vertex.edge;
		}

		return static_cast<bool>(reader);
	}

	template<typename VertexIndex>
	bool LoadFaces() {
		const auto num_indices = reader.Int32();
		if (num_indices < 0 || num_indices % 3) {
			return false;
		}

		pmx.indices.resize(num_indices);
		for (auto& index : pmx.indices) {
			reader.For<VertexIndex>() >> index;
		}

		return static_cast<bool>(reader);
	}

	bool LoadTextures() {
		auto num_textures = reader.Int32();
		if (num_textures < 0) {
			return false;
		}

		pmx.textures.resize(num_textures);
		for (auto& texture : pmx.textures) {
			reader >> texture;
		}

		return static_cast<bool>(reader);
	}

	template<typename TextureIndex>
	bool LoadElement(typename PmxTy::Material& material) {
		reader.ForVec4() >> material.diffuse;
		reader.ForVec4() >> material.specular;
		reader.ForVec3() >> material.ambient;

		auto flag = reader.Uint8();
		material.twoside = flag & 0x01;
		material.cast_ground_shadow = flag & 0x02;
		material.cast_self_shadow = flag & 0x04;
		material.receive_self_shadow = flag & 0x08;
		material.draw_edge = flag & 0x10;

		reader.ForVec4() >> material.edge_color;
		reader >> material.edge_size;
		reader.For<TextureIndex>() >> material.base_texture_index;
		reader.For<TextureIndex>() >> material.sphere_texture_index;
		reader >> material.sphere_mode;
		reader >> material.use_shared_toon;
		material.toon_texture_index = (material.use_shared_toon) ? reader.Uint8() : reader.As<TextureIndex>();
		reader >> material.note;
		reader >> material.num_vertices;

		if (material.num_vertices % 3 != 0) {
			return false;
		}

		return true;
	}

	template<typename BoneIndex>
	bool LoadElement(typename PmxTy::Bone& bone) {
		reader.ForVec3() >> bone.position;
		reader.For<BoneIndex>() >> bone.parent_bone_index;
		reader >> bone.level;

		auto flag = reader.Uint16();
		bone.has_tip_bone = flag & 0x0001;
		bone.rotatable = flag & 0x0002;
		bone.translatable = flag & 0x0004;
		bone.visible = flag & 0x0008;
		bone.operable = flag & 0x0010;
		bone.is_ik = flag & 0x0020;
		bone.local_driven = flag & 0x0080;
		bone.driven_rotation = flag & 0x0100;
		bone.driven_translation = flag & 0x0200;
		bone.has_fixed_axis = flag & 0x0400;
		bone.has_local_axis = flag & 0x0800;
		bone.post_physics_transform = flag & 0x1000;
		bone.external_transform = flag & 0x2000;

		if (bone.has_tip_bone) {
			bone.tip_offset = {};
			reader.For<BoneIndex>() >> bone.tip_bone_index;
		}
		else {
			reader.ForVec3() >> bone.tip_offset;
			bone.tip_bone_index = -1;
		}

		if (bone.driven_rotation || bone.driven_translation) {
			reader.For<BoneIndex>() >> bone.drive_bone_index;
			reader >> bone.drive_rate;
		}
		else {
			bone.drive_bone_index = -1;
			bone.drive_rate = 0.f;
		}

		if (bone.has_fixed_axis) {
			reader.ForVec3() >> bone.fixed_axis;
		}
		else {
			bone.fixed_axis = {};
		}

		if (bone.has_local_axis) {
			reader.ForVec3() >> bone.local_axis_x;
			reader.ForVec3() >> bone.local_axis_z;
		}
		else {
			bone.local_axis_x = {};
			bone.local_axis_z = {};
		}

		if (bone.external_transform) {
			reader >> bone.external_key;
		}
		else {
			bone.external_key = 0;
		}

		if (bone.is_ik) {
			reader.For<BoneIndex>() >> bone.ik_target_bone_index;
			reader >> bone.ik_iteration_count;
			reader >> bone.ik_angle_limit;

			auto num_ik_links = reader.Int32();
			if (num_ik_links < 0) {
				return false;
			}

			bone.ik_links.resize(num_ik_links);
			for (auto& ik_link : bone.ik_links) {
				reader.For<BoneIndex>() >> ik_link.index;
				reader >> ik_link.angle_limited;

				if (ik_link.angle_limited) {
					reader.ForVec3() >> ik_link.angle_min;
					reader.ForVec3() >> ik_link.angle_max;
				}
			}
		}
		else {
			bone.ik_target_bone_index = -1;
		}

		return true;
	}

	template<typename MorphIndex>
	bool LoadMorphData(typename PmxTy::GroupMorphData& data) {
		reader.For<MorphIndex>() >> data.index;
		reader >> data.rate;
		return true;
	}

	template<typename VertexIndex>
	bool LoadMorphData(typename PmxTy::VertexMorphData& data) {
		reader.For<VertexIndex>() >> data.index;
		reader.ForVec3() >> data.offset;
		return true;
	}

	template<typename BoneIndex>
	bool LoadMorphData(typename PmxTy::BoneMorphData& data) {
		reader.For<BoneIndex>() >> data.index;
		reader.ForVec3() >> data.translation;
		reader.ForVec4() >> data.rotation;
		return true;
	}

	template<typename VertexIndex>
	bool LoadMorphData(typename PmxTy::UvMorphData& data) {
		reader.For<VertexIndex>() >> data.index;
		reader.ForVec4() >> data.offset;
		return true;
	}

	template<typename MaterialIndex>
	bool LoadMorphData(typename PmxTy::MaterialMorphData& data) {
		reader.For<MaterialIndex>() >> data.index;
		reader.For<PmxMaterialMorphOp>() >> data.mode;
		reader.ForVec4() >> data.diffuse;
		reader.ForVec4() >> data.specular;
		reader.ForVec3() >> data.ambient;
		reader.ForVec4() >> data.edge_color;
		reader >> data.edge_size;
		reader.ForVec4() >> data.base_texture_coef;
		reader.ForVec4() >> data.sphere_texture_coef;
		reader.ForVec4() >> data.toon_texture_coef;

		switch (data.mode) {
		case PmxMaterialMorphOp::Mult:
		case PmxMaterialMorphOp::Add: break;
		default: return false;
		}

		return true;
	}

	template<typename Index, typename Morph>
	bool LoadMorphData(const std::wstring& name, const std::wstring& name_en, typename PmxMorphPanel panel, typename PmxMorphType type, std::vector<Morph>& morphs) {
		auto& morph = morphs.emplace_back(name, name_en, panel, type);

		int32_t num_data = reader.Int32();
		if (num_data < 0) {
			return false;
		}

		morph.data.resize(num_data);
		for (auto& data : morph.data) {
			if (!LoadMorphData<Index>(data)) {
				return false;
			}
		}

		return true;
	}

	bool LoadMorphs() {
		const auto num_morphs = reader.Int32();
		if (num_morphs < 0) {
			return false;
		}

		for (int i = 0; i < num_morphs; ++i) {
			auto name = reader.TextW();
			auto name_en = reader.TextW();
			auto panel = reader.As<PmxMorphPanel>();
			auto type = reader.As<PmxMorphType>();

			bool ret = false;
			switch (type) {
			case PmxMorphType::Group:
				switch (pmx.header.morph_index_size) {
				case 1: ret = LoadMorphData<int8_t>(name, name_en, panel, type, pmx.group_morphs); break;
				case 2: ret = LoadMorphData<int16_t>(name, name_en, panel, type, pmx.group_morphs); break;
				case 4: ret = LoadMorphData<int32_t>(name, name_en, panel, type, pmx.group_morphs); break;
				}
				break;

			case PmxMorphType::Vertex:
				switch (pmx.header.vertex_index_size) {
				case 1: ret = LoadMorphData<int8_t>(name, name_en, panel, type, pmx.vertex_morphs); break;
				case 2: ret = LoadMorphData<int16_t>(name, name_en, panel, type, pmx.vertex_morphs); break;
				case 4: ret = LoadMorphData<int32_t>(name, name_en, panel, type, pmx.vertex_morphs); break;
				}
				break;

			case PmxMorphType::Bone:
				switch (pmx.header.bone_index_size) {
				case 1: ret = LoadMorphData<int8_t>(name, name_en, panel, type, pmx.bone_morphs); break;
				case 2: ret = LoadMorphData<int16_t>(name, name_en, panel, type, pmx.bone_morphs); break;
				case 4: ret = LoadMorphData<int32_t>(name, name_en, panel, type, pmx.bone_morphs); break;
				}
				break;

			case PmxMorphType::UV:
			case PmxMorphType::ExUV1:
			case PmxMorphType::ExUV2:
			case PmxMorphType::ExUV3:
			case PmxMorphType::ExUV4:
				switch (pmx.header.vertex_index_size) {
				case 1: ret = LoadMorphData<int8_t>(name, name_en, panel, type, pmx.uv_morphs); break;
				case 2: ret = LoadMorphData<int16_t>(name, name_en, panel, type, pmx.uv_morphs); break;
				case 4: ret = LoadMorphData<int32_t>(name, name_en, panel, type, pmx.uv_morphs); break;
				}
				break;

			case PmxMorphType::Material:
				switch (pmx.header.material_index_size) {
				case 1: ret = LoadMorphData<int8_t>(name, name_en, panel, type, pmx.material_morphs); break;
				case 2: ret = LoadMorphData<int16_t>(name, name_en, panel, type, pmx.material_morphs); break;
				case 4: ret = LoadMorphData<int32_t>(name, name_en, panel, type, pmx.material_morphs); break;
				}
				break;
			}

			if (!ret) {
				return false;
			}
		}

		return static_cast<bool>(reader);
	}

	template<typename BoneIndex, typename MorphIndex>
	bool LoadNodeItem(typename PmxTy::Node::Item& item) {
		reader.For<PmxNodeItemType>() >> item.type;
		switch (item.type) {
		case PmxNodeItemType::Bone:
			reader.For<BoneIndex>() >> item.index;
			break;

		case PmxNodeItemType::Morph:
			reader.For<MorphIndex>() >> item.index;
			break;

		default:
			return false;
		}
		return true;
	}

	template<typename BoneIndex, typename MorphIndex>
	bool LoadNode(typename PmxTy::Node& node) {
		reader >> node.is_special;

		auto num_items = reader.Int32();
		if (num_items < 0) {
			return false;
		}

		node.items.resize(num_items);
		for (auto& item : node.items) {
			if (!LoadNodeItem<BoneIndex, MorphIndex>(item)) {
				return false;
			}
		}

		return true;
	}

	template<typename BoneIndex, typename MorphIndex>
	bool LoadNodes(std::vector<typename PmxTy::Node>& nodes) {
		auto num_nodes = reader.Int32();
		if (num_nodes < 0) {
			return false;
		}

		nodes.resize(num_nodes);
		for (auto& node : nodes) {
			reader >> node.name;
			reader >> node.name_en;
			if (!LoadNode<BoneIndex, MorphIndex>(node)) {
				return false;
			}
		}

		return static_cast<bool>(reader);
	}

	template<typename BoneIndex>
	bool LoadElement(typename PmxTy::Body& body) {
		reader.For<BoneIndex>() >> body.index;

		reader >> body.group;
		reader >> body.non_collision_group;

		reader >> body.shape;
		reader.ForVec3() >> body.size;

		reader.ForVec3() >> body.position;
		reader.ForVec3() >> body.rotation;

		reader >> body.mass;
		reader >> body.translation_atten;
		reader >> body.rotation_atten;
		reader >> body.restitution;
		reader >> body.friction;

		reader.For<PmxBodyPhysicsType>() >> body.physics_type;
		switch (body.physics_type) {
		case PmxBodyPhysicsType::Static:
		case PmxBodyPhysicsType::Dynamic:
		case PmxBodyPhysicsType::Combine: break;
		default: return false;
		}

		return true;
	}

	template<typename BodyIndex>
	bool LoadElement(typename PmxTy::Joint& joint) {
		reader.For<PmxJointType>() >> joint.type;
		switch (joint.type) {
		case PmxJointType::Spring6DOF:
			reader.For<BodyIndex>() >> joint.body_index_a;
			reader.For<BodyIndex>() >> joint.body_index_b;
			reader.ForVec3() >> joint.position;
			reader.ForVec3() >> joint.rotation;
			reader.ForVec3() >> joint.translation_min;
			reader.ForVec3() >> joint.translation_max;
			reader.ForVec3() >> joint.rotation_min;
			reader.ForVec3() >> joint.rotation_max;
			reader.ForVec3() >> joint.sprint_const_translation;
			reader.ForVec3() >> joint.sprint_const_rotation;
			break;

		default:
			return false;
		}

		return true;
	}

	template<typename Index, typename Elem>
	bool LoadElements(std::vector<Elem>& elems) {
		const auto num_elems = reader.Int32();
		if (num_elems < 0) {
			return false;
		}

		elems.resize(num_elems);
		for (auto& elem : elems) {
			reader >> elem.name;
			reader >> elem.name_en;
			if (!LoadElement<Index>(elem)) {
				return false;
			}
		}

		return static_cast<bool>(reader);
	}

	bool Load() {
		if (!reader) {
			return false;
		}

		// Load Header
		bool ret = LoadHeader(pmx.header);
		if (!ret) {
			return false;
		}

		// Load Vertices
		switch (pmx.header.bone_index_size) {
		case 1: ret = LoadVertices<int8_t>(); break;
		case 2: ret = LoadVertices<int16_t>(); break;
		case 4: ret = LoadVertices<int32_t>(); break;
		}
		if (!ret) {
			return false;
		}

		// Load Faces
		switch (pmx.header.vertex_index_size) {
		case 1: ret = LoadFaces<uint8_t>(); break;
		case 2: ret = LoadFaces<uint16_t>(); break;
		case 4: ret = LoadFaces<int32_t>(); break;
		}
		if (!ret) {
			return false;
		}

		// Load Textures
		ret = LoadTextures();
		if (!ret) {
			return false;
		}

		// Load Materials
		switch (pmx.header.texture_index_size) {
		case 1: ret = LoadElements<int8_t>(pmx.materials); break;
		case 2: ret = LoadElements<int16_t>(pmx.materials); break;
		case 4: ret = LoadElements<int32_t>(pmx.materials); break;
		}
		if (!ret) {
			return false;
		}

		// Load Bones
		switch (pmx.header.bone_index_size) {
		case 1: ret = LoadElements<int8_t>(pmx.bones); break;
		case 2: ret = LoadElements<int16_t>(pmx.bones); break;
		case 4: ret = LoadElements<int32_t>(pmx.bones); break;
		}
		if (!ret) {
			return false;
		}

		// Load Morphs
		ret = LoadMorphs();
		if (!ret) {
			return false;
		}

		// Load Nodes
		switch (pmx.header.bone_index_size) {
		case 1:
			switch (pmx.header.morph_index_size) {
			case 1: ret = LoadNodes<int8_t, int8_t>(pmx.nodes); break;
			case 2: ret = LoadNodes<int8_t, int16_t>(pmx.nodes); break;
			case 4: ret = LoadNodes<int8_t, int32_t>(pmx.nodes); break;
			}
			break;

		case 2:
			switch (pmx.header.morph_index_size) {
			case 1: ret = LoadNodes<int16_t, int8_t>(pmx.nodes); break;
			case 2: ret = LoadNodes<int16_t, int16_t>(pmx.nodes); break;
			case 4: ret = LoadNodes<int16_t, int32_t>(pmx.nodes); break;
			}
			break;

		case 4:
			switch (pmx.header.morph_index_size) {
			case 1: ret = LoadNodes<int32_t, int8_t>(pmx.nodes); break;
			case 2: ret = LoadNodes<int32_t, int16_t>(pmx.nodes); break;
			case 4: ret = LoadNodes<int32_t, int32_t>(pmx.nodes); break;
			}
			break;
		}
		if (!ret) {
			return false;
		}

		// Load Bodies
		switch (pmx.header.bone_index_size) {
		case 1: ret = LoadElements<int8_t>(pmx.bodies); break;
		case 2: ret = LoadElements<int16_t>(pmx.bodies); break;
		case 4: ret = LoadElements<int32_t>(pmx.bodies); break;
		}
		if (!ret) {
			return false;
		}

		// Load Joints
		switch (pmx.header.body_index_size) {
		case 1: ret = LoadElements<int8_t>(pmx.joints); break;
		case 2: ret = LoadElements<int16_t>(pmx.joints); break;
		case 4: ret = LoadElements<int32_t>(pmx.joints); break;
		}
		if (!ret) {
			return false;
		}

		return static_cast<bool>(reader);
	}
};

// 
// Serialize Pmx
//
template<typename Vec2, typename Vec3, typename Vec4>
struct PmxSerializer {
	using PmxTy = Pmx<Vec2, Vec3, Vec4>;

	const PmxTy& pmx;
	Writer writer;

	PmxSerializer(const PmxTy& pmx, std::vector<char>& buf) :
		pmx(pmx),
		writer(buf) {
	}

	void SerializeHeader(const typename PmxTy::Header& header) {
		writer << PmxMagic;
		writer << PmxVersion;

		writer << static_cast<uint8_t>(8);
		writer << static_cast<uint8_t>(header.encode);
		writer << static_cast<uint8_t>(header.num_ex_uvs);
		writer << static_cast<uint8_t>(4);
		writer << static_cast<uint8_t>(4);
		writer << static_cast<uint8_t>(4);
		writer << static_cast<uint8_t>(4);
		writer << static_cast<uint8_t>(4);
		writer << static_cast<uint8_t>(4);

		writer << header.name;
		writer << header.name_en;
		writer << header.comment;
		writer << header.comment_en;
	}

	void SerializeVertices(const std::vector<typename PmxTy::Vertex>& vertices, const typename PmxTy::Header& header) {
		writer << static_cast<uint32_t>(vertices.size());

		for (const auto& vertex : vertices) {
			writer.ForVec3() << vertex.position;
			writer.ForVec3() << vertex.normal;
			writer.ForVec2() << vertex.uv;

			for (int i = 0; i < header.num_ex_uvs; ++i) {
				writer.ForVec4() << vertex.ex_uvs[i];
			}

			writer << vertex.weight_type;
			switch (vertex.weight_type) {
			case PmxWeightType::BDEF1:
				writer << vertex.bone_indices[0];
				break;

			case PmxWeightType::BDEF2:
				writer << vertex.bone_indices[0];
				writer << vertex.bone_indices[1];
				writer << vertex.bone_weights[0];
				break;

			case PmxWeightType::BDEF4:
				writer << vertex.bone_indices;
				writer << vertex.bone_weights;
				break;

			case PmxWeightType::SDEF:
				writer << vertex.bone_indices[0];
				writer << vertex.bone_indices[1];
				writer << vertex.bone_weights[0];
				writer.ForVec3() << vertex.sdef_c;
				writer.ForVec3() << vertex.sdef_r0;
				writer.ForVec3() << vertex.sdef_r1;
				break;
			}

			writer << vertex.edge;
		}
	}

	void SerializeFaces(const std::vector<int32_t>& indices) {
		auto num_indices = static_cast<uint32_t>(indices.size());
		writer << num_indices;
		writer.WriteArray(indices.data(), num_indices);
	}

	void SerializeTextures(const std::vector<std::wstring>& textures) {
		auto num_textures = static_cast<uint32_t>(textures.size());
		writer << num_textures;
		for (const auto& texture : textures) {
			writer << texture;
		}
	}

	void SerializeElement(const typename PmxTy::Material& material) {
		writer.ForVec4() << material.diffuse;
		writer.ForVec4() << material.specular;
		writer.ForVec3() << material.ambient;

		uint8_t flag = 0;
		flag |= material.twoside << 0;
		flag |= material.cast_ground_shadow << 1;
		flag |= material.cast_self_shadow << 2;
		flag |= material.receive_self_shadow << 3;
		flag |= material.draw_edge << 4;
		writer << flag;

		writer.ForVec4() << material.edge_color;
		writer << material.edge_size;
		writer << material.base_texture_index;
		writer << material.sphere_texture_index;
		writer << material.sphere_mode;
		writer << material.use_shared_toon;

		if (material.use_shared_toon == 0) {
			writer << material.toon_texture_index;
		}
		else {
			writer << static_cast<int8_t>(material.toon_texture_index);
		}

		writer << material.note;
		writer << material.num_vertices;
	}

	void SerializeElement(const typename PmxTy::Bone& bone) {
		writer.ForVec3() << bone.position;
		writer << bone.parent_bone_index;
		writer << bone.level;

		uint16_t flag = 0;
		flag |= bone.has_tip_bone << 0;
		flag |= bone.rotatable << 1;
		flag |= bone.translatable << 2;
		flag |= bone.visible << 3;
		flag |= bone.operable << 4;
		flag |= bone.is_ik << 5;
		flag |= bone.local_driven << 7;
		flag |= bone.driven_rotation << 8;
		flag |= bone.driven_translation << 9;
		flag |= bone.has_fixed_axis << 10;
		flag |= bone.has_local_axis << 11;
		flag |= bone.post_physics_transform << 12;
		flag |= bone.external_transform << 13;
		writer << flag;

		if (bone.has_tip_bone) {
			writer << bone.tip_bone_index;
		}
		else {
			writer.ForVec3() << bone.tip_offset;
		}

		if (bone.driven_rotation || bone.driven_translation) {
			writer << bone.drive_bone_index;
			writer << bone.drive_rate;
		}

		if (bone.has_fixed_axis) {
			writer.ForVec3() << bone.fixed_axis;
		}

		if (bone.has_local_axis) {
			writer.ForVec3() << bone.local_axis_x;
			writer.ForVec3() << bone.local_axis_z;
		}

		if (bone.external_transform) {
			writer << bone.external_key;
		}

		if (bone.is_ik) {
			writer << bone.ik_target_bone_index;
			writer << bone.ik_iteration_count;
			writer << bone.ik_angle_limit;

			writer << static_cast<uint32_t>(bone.ik_links.size());
			for (auto& ik : bone.ik_links) {
				writer << ik.index;
				writer << ik.angle_limited;

				if (ik.angle_limited) {
					writer.ForVec3() << ik.angle_min;
					writer.ForVec3() << ik.angle_max;
				}
			}
		}
	}

	void SerializeMorphData(const typename PmxTy::GroupMorphData& data) {
		writer << data.index;
		writer << data.rate;
	}

	void SerializeMorphData(const typename PmxTy::VertexMorphData& data) {
		writer << data.index;
		writer.ForVec3() << data.offset;
	}

	void SerializeMorphData(const typename PmxTy::BoneMorphData& data) {
		writer << data.index;
		writer.ForVec3() << data.translation;
		writer.ForVec4() << data.rotation;
	}

	void SerializeMorphData(const typename PmxTy::UvMorphData& data) {
		writer << data.index;
		writer.ForVec4() << data.offset;
	}

	void SerializeMorphData(const typename PmxTy::MaterialMorphData& data) {
		writer << data.index;
		writer << data.mode;
		writer.ForVec4() << data.diffuse;
		writer.ForVec4() << data.specular;
		writer.ForVec3() << data.ambient;
		writer.ForVec4() << data.edge_color;
		writer << data.edge_size;
		writer.ForVec4() << data.base_texture_coef;
		writer.ForVec4() << data.sphere_texture_coef;
		writer.ForVec4() << data.toon_texture_coef;
	}

	template<typename Morph>
	void SerializeMorphsData(const std::vector<Morph>& morphs) {
		for (auto& morph : morphs) {
			writer << morph.name;
			writer << morph.name_en;
			writer << morph.panel;
			writer << morph.type;

			writer << static_cast<uint32_t>(morph.data.size());
			for (auto& data : morph.data) {
				SerializeMorphData(data);
			}
		}
	}

	void SerializeMorphs() {
		uint32_t total_morphs = static_cast<uint32_t>(pmx.group_morphs.size()) +
			static_cast<uint32_t>(pmx.vertex_morphs.size()) +
			static_cast<uint32_t>(pmx.bone_morphs.size()) +
			static_cast<uint32_t>(pmx.uv_morphs.size()) +
			static_cast<uint32_t>(pmx.material_morphs.size());
		writer << total_morphs;

		SerializeMorphsData(pmx.group_morphs);
		SerializeMorphsData(pmx.vertex_morphs);
		SerializeMorphsData(pmx.bone_morphs);
		SerializeMorphsData(pmx.uv_morphs);
		SerializeMorphsData(pmx.material_morphs);
	}

	void SerializeElement(const typename PmxTy::Node& node) {
		writer << node.is_special;

		writer << static_cast<uint32_t>(node.items.size());
		for (auto& item : node.items) {
			writer << item.type;
			writer << item.index;
		}
	}

	void SerializeElement(const typename PmxTy::Body& body) {
		writer << body.index;
		writer << body.group;
		writer << body.non_collision_group;
		writer << body.shape;
		writer.ForVec3() << body.size;
		writer.ForVec3() << body.position;
		writer.ForVec3() << body.rotation;
		writer << body.mass;
		writer << body.translation_atten;
		writer << body.rotation_atten;
		writer << body.restitution;
		writer << body.friction;
		writer << body.physics_type;
	}

	void SerializeElement(const typename PmxTy::Joint& joint) {
		writer << joint.type;
		switch (joint.type) {
		case PmxJointType::Spring6DOF:
			writer << joint.body_index_a;
			writer << joint.body_index_b;
			writer.ForVec3() << joint.position;
			writer.ForVec3() << joint.rotation;
			writer.ForVec3() << joint.translation_min;
			writer.ForVec3() << joint.translation_max;
			writer.ForVec3() << joint.rotation_min;
			writer.ForVec3() << joint.rotation_max;
			writer.ForVec3() << joint.sprint_const_translation;
			writer.ForVec3() << joint.sprint_const_rotation;
			break;
		}
	}

	template<typename Elem>
	void SerializeElements(const std::vector<Elem>& elems) {
		writer << static_cast<uint32_t>(elems.size());
		for (auto& elem : elems) {
			writer << elem.name;
			writer << elem.name_en;
			SerializeElement(elem);
		}
	}

	bool Serialize() {
		SerializeHeader(pmx.header);
		SerializeVertices(pmx.vertices, pmx.header);
		SerializeFaces(pmx.indices);
		SerializeTextures(pmx.textures);
		SerializeElements(pmx.materials);
		SerializeElements(pmx.bones);
		SerializeMorphs();
		SerializeElements(pmx.nodes);
		SerializeElements(pmx.bodies);
		SerializeElements(pmx.joints);

		return true;
	}
};

//
// Load Vmd
//
template<typename Vec3, typename Vec4>
struct VmdLoader {
	using VmdTy = Vmd<Vec3, Vec4>;

	VmdTy& vmd;
	Reader reader;

	VmdLoader(VmdTy& vmd, const void* data, std::size_t size) :
		vmd(vmd),
		reader(data, size) {
	}

	bool LoadHeader(typename VmdTy::Header& header) {
		char magic[30] = {};
		reader >> magic;
		if (memcmp(magic, VmdMagic, strlen(VmdMagic)) != 0) {
			return false;
		}

		reader.ForFixed<20>() >> header.name;

		return static_cast<bool>(reader);
	}

	void LoadNamedKey(typename VmdTy::MotionKey& key) {
		reader >> key.frame;
		reader.ForVec3() >> key.position;
		reader.ForVec4() >> key.orientation;
		reader >> key.interpolation;
		key.ix.x1 = key.interpolation[0 + 0];	key.ix.y1 = key.interpolation[0 + 4];	key.ix.x2 = key.interpolation[0 + 8];	key.ix.y2 = key.interpolation[0 + 12];
		key.iy.x1 = key.interpolation[16 + 0];	key.iy.y1 = key.interpolation[16 + 4];	key.iy.x2 = key.interpolation[16 + 8];	key.iy.y2 = key.interpolation[16 + 12];
		key.iz.x1 = key.interpolation[32 + 0];	key.iz.y1 = key.interpolation[32 + 4];	key.iz.x2 = key.interpolation[32 + 8];	key.iz.y2 = key.interpolation[32 + 12];
		key.ir.x1 = key.interpolation[48 + 0];	key.ir.y1 = key.interpolation[48 + 4];	key.ir.x2 = key.interpolation[48 + 8];	key.ir.y2 = key.interpolation[48 + 12];
		key.physics = !(key.interpolation[2] == 0x63 && key.interpolation[3] == 0x0f);
	}

	void LoadNamedKey(typename VmdTy::MorphKey& key) {
		reader >> key.frame;
		reader >> key.value;
	}

	template<typename Key>
	bool LoadNamedTracks(std::map<std::string, std::vector<Key>>& trasks) {
		const auto num_keys = reader.Uint32();
		for (uint32_t i = 0; i < num_keys; ++i) {
			auto name = reader.TextA(15);
			auto& key = trasks[name].emplace_back(Key{});
			LoadNamedKey(key);
		}

		return static_cast<bool>(reader);
	}

	void LoadKey(typename VmdTy::CameraKey& key) {
		reader >> key.frame;
		reader >> key.distance;
		reader.ForVec3() >> key.position;
		reader.ForVec3() >> key.rotation;
		reader >> key.ix >> key.iy >> key.iz >> key.ir >> key.id >> key.iv;
		reader >> key.view_angle;
		reader.For<uint8_t>() >> key.parallel;
	}

	void LoadKey(typename VmdTy::LightKey& key) {
		reader >> key.frame;
		reader.ForVec3() >> key.color;
		reader.ForVec3() >> key.position;
	}

	void LoadKey(typename VmdTy::ShadowKey& key) {
		reader >> key.frame;
		reader >> key.type;
		reader >> key.distance;
	}

	template<typename Key>
	bool LoadTrack(std::vector<Key>& keys) {
		const auto num_keys = reader.Uint32();
		keys.resize(num_keys);
		for (auto& key : keys) {
			LoadKey(key);
		}

		return static_cast<bool>(reader);
	}

	void LoadIkKey(uint32_t frame) {
		const auto name = reader.TextA(20);
		const bool enable = reader.Uint8();

		auto& key = vmd.iks[name].emplace_back(typename VmdTy::IkKey{});
		key.frame = frame;
		key.enable = enable;
	}

	void LoadExKey() {
		const auto frame = reader.Uint32();
		const bool visibility = reader.Uint8();

		auto& visibility_key = vmd.visibilities.emplace_back(typename VmdTy::VisibilityKey{});
		visibility_key.frame = frame;
		visibility_key.visible = visibility;

		const auto num_ik_keys = reader.Uint32();
		for (uint32_t i = 0; i < num_ik_keys; ++i) {
			LoadIkKey(frame);
		}
	}

	bool LoadExTrack() {
		const auto num_keys = reader.Uint32();
		for (uint32_t i = 0; i < num_keys; ++i) {
			LoadExKey();
		}

		return static_cast<bool>(reader);
	}

	bool Load() {
		if (!reader) {
			return false;
		}

		if (!LoadHeader(vmd.header)) {
			return false;
		}

		if (!LoadNamedTracks(vmd.motions)) {
			return false;
		}

		if (!LoadNamedTracks(vmd.morphs)) {
			return false;
		}

		if (!LoadTrack(vmd.cameras)) {
			return false;
		}

		if (!LoadTrack(vmd.lights)) {
			return false;
		}

		if (!LoadTrack(vmd.shadows)) {
			return false;
		}

		if (!LoadExTrack()) {
			return false;
		}

		return static_cast<bool>(reader);
	}
};

//
// Serialize Vmd
//
template<typename Vec3, typename Vec4>
struct VmdSerializer {
	using VmdTy = Vmd<Vec3, Vec4>;

	const VmdTy& vmd;
	Writer writer;

	VmdSerializer(const VmdTy& vmd, std::vector<char>& buf) :
		vmd(vmd),
		writer(buf) {
	}

	void SerializeHeader(const typename VmdTy::Header& header) {
		writer << VmdMagic;
		writer.ForFixed<20>() << header.name;
	}

	void SerializeKey(const typename VmdTy::MotionKey& key) {
		writer << key.frame;
		writer.ForVec3() << key.position;
		writer.ForVec4() << key.orientation;

		char interp[64]{};
		memcpy_s(interp, 64, key.interpolation, 64);
		interp[0 + 0] = key.ix.x1;	interp[0 + 4] = key.ix.y1;	interp[0 + 8] = key.ix.x2;	interp[0 + 12] = key.ix.y2;
		interp[16 + 0] = key.iy.x1;	interp[16 + 4] = key.iy.y1;	interp[16 + 8] = key.iy.x2;	interp[16 + 12] = key.iy.y2;
		interp[32 + 0] = key.iz.x1;	interp[32 + 4] = key.iz.y1;	interp[32 + 8] = key.iz.x2;	interp[32 + 12] = key.iz.y2;
		interp[48 + 0] = key.ir.x1;	interp[48 + 4] = key.ir.y1;	interp[48 + 8] = key.ir.x2;	interp[48 + 12] = key.ir.y2;

		if (key.physics) {
			interp[2] = 0x63;
			interp[3] = 0x0f;
		}

		writer << interp;
	}

	void SerializeKey(const typename VmdTy::MorphKey& key) {
		writer << key.frame;
		writer << key.value;
	}

	template<typename Key>
	void SerializeNamedTracks(const std::map<std::string, std::vector<Key>>& tracks) {
		uint32_t num_keys = 0;
		for (auto& [name, keys] : tracks) {
			num_keys += static_cast<uint32_t>(keys.size());
		}
		writer << num_keys;

		for (auto& [name, keys] : tracks) {
			for (auto& key : keys) {
				writer.ForFixed<15>() << name;
				SerializeKey(key);
			}
		}
	}

	void SerializeKey(const typename VmdTy::CameraKey& key) {
		writer << key.frame;
		writer << key.distance;
		writer.ForVec3() << key.position;
		writer.ForVec3() << key.rotation;
		writer << key.ix << key.iy << key.iz << key.ir << key.id << key.iv;
		writer << key.view_angle;
		writer << static_cast<uint8_t>(key.parallel);
	}

	void SerializeKey(const typename VmdTy::LightKey& key) {
		writer << key.frame;
		writer.ForVec3() << key.color;
		writer.ForVec3() << key.position;
	}

	void SerializeKey(const typename VmdTy::ShadowKey& key) {
		writer << key.frame;
		writer << key.type;
		writer << key.distance;
	}

	template<typename Track>
	void SerializeTrack(const Track& keys) {
		writer << static_cast<uint32_t>(keys.size());

		for (auto& key : keys) {
			SerializeKey(key);
		}
	}

	struct IntegratedExKey {
		uint32_t	frame;
		uint8_t		visible;
		std::map<std::string, uint8_t> iks;
	};

	void SerializeExKey(const IntegratedExKey& key) {
		writer << key.frame;
		writer << static_cast<uint8_t>(key.visible);

		writer << static_cast<uint32_t>(key.iks.size());
		for (auto& [name, enable] : key.iks) {
			writer.ForFixed<20>() << name;
			writer << static_cast<uint8_t>(enable);
		}
	}

	void SerializeExTrack() {
		std::map<uint32_t, IntegratedExKey> ex_keys{};

		for (const auto& visibility : vmd.visibilities) {
			auto& ex = ex_keys[visibility.frame];
			ex.frame = visibility.frame;
			ex.visible = visibility.visible;
		}

		for (const auto& [name, keys] : vmd.iks) {
			for (const auto& key : keys) {
				auto& ex = ex_keys[key.frame];
				ex.frame = key.frame;
				ex.iks[name] = key.enable;
			}
		}

		writer << static_cast<uint32_t>(ex_keys.size());
		for (auto& [frame, key] : ex_keys) {
			SerializeExKey(key);
		}
	}

	bool Serialize() {
		SerializeHeader(vmd.header);
		SerializeNamedTracks(vmd.motions);
		SerializeNamedTracks(vmd.morphs);
		SerializeTrack(vmd.cameras);
		SerializeTrack(vmd.lights);
		SerializeTrack(vmd.shadows);
		SerializeExTrack();

		return true;
	}
};

template<typename Path>
inline std::vector<char> LoadBinary(const Path& path) {
	auto ifs = std::ifstream(path, std::ios::binary);
	if (!ifs) {
		return {};
	}

	ifs.seekg(0, std::ios::end);
	auto end = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	auto beg = ifs.tellg();
	const auto size = end - beg;

	std::vector<char> bin(size);
	ifs.read(bin.data(), size);
	return bin;
}

template<typename Path>
inline bool SaveBinary(const Path& path, const std::vector<char>& bin) {
	auto ofs = std::ofstream(path, std::ios::binary);
	if (!ofs) {
		return false;
	}

	ofs.write(bin.data(), bin.size());
	return true;
}

} // namespace io

template<typename Vec2, typename Vec3, typename Vec4, typename Path>
inline bool LoadPmx(const Path& path, Pmx<Vec2, Vec3, Vec4>& pmx) {
	auto bin = io::LoadBinary(path);
	return io::PmxLoader<Vec2, Vec3, Vec4>(pmx, bin.data(), bin.size()).Load();
}

template<typename Vec2, typename Vec3, typename Vec4>
inline bool LoadPmx(const void* data, std::size_t size, Pmx<Vec2, Vec3, Vec4>& pmx) {
	return io::PmxLoader<Vec2, Vec3, Vec4>(pmx, data, size).Load();
}

template<typename Vec2, typename Vec3, typename Vec4, typename Path>
inline bool SavePmx(const Path& path, const Pmx<Vec2, Vec3, Vec4>& pmx) {
	std::vector<char> buf{};
	buf.reserve(41 * pmx.vertices.size() + 4 * pmx.indices.size() + 90 * pmx.materials.size() + 30 * pmx.bones.size());

	if (!io::PmxSerializer<Vec2, Vec3, Vec4>(pmx, buf).Serialize()) {
		return false;
	}

	return io::SaveBinary(path, buf);
}

template<typename Vec2, typename Vec3, typename Vec4>
inline bool SavePmx(std::vector<char>& out, const Pmx<Vec2, Vec3, Vec4>& pmx) {
	out.reserve(41 * pmx.vertices.size() + 4 * pmx.indices.size() + 90 * pmx.materials.size() + 30 * pmx.bones.size());
	return io::PmxSerializer<Vec2, Vec3, Vec4>(pmx, out).Serialize();
}

template<typename Vec3, typename Vec4, typename Path>
inline bool LoadVmd(const Path& path, Vmd<Vec3, Vec4>& vmd) {
	auto bin = io::LoadBinary(path);
	return io::VmdLoader<Vec3, Vec4>(vmd, bin.data(), bin.size()).Load();
}

template<typename Vec3, typename Vec4>
inline bool LoadVmd(const void* data, std::size_t size, Vmd<Vec3, Vec4>& vmd) {
	return io::VmdLoader<Vec3, Vec4>(vmd, data, size).Load();
}

template<typename Vec3, typename Vec4, typename Path>
inline bool SaveVmd(const Path& path, const Vmd<Vec3, Vec4>& vmd) {
	std::vector<char> buf{};
	buf.reserve(111 * vmd.motions.size() + 23 * vmd.morphs.size() + 105 * vmd.cameras.size());

	if (!io::VmdSerializer<Vec3, Vec4>(vmd, buf).Serialize()) {
		return false;
	}

	return io::SaveBinary(path, buf);
}

template<typename Vec3, typename Vec4>
inline bool SaveVmd(std::vector<char>& out, const Vmd<Vec3, Vec4>& vmd) {
	out.reserve(111 * vmd.motions.size() + 23 * vmd.morphs.size() + 105 * vmd.cameras.size());
	return io::VmdSerializer<Vec3, Vec4>(vmd, out).Serialize();
}

} // namspace portable_mmd
