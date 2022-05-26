#pragma once
#include <fstream>

class FileWriter {
public:
	FileWriter(const wchar_t* path);

	template<typename Ty>
	void Write(const Ty& value);

	template<typename Ty>
	void WriteArray(const Ty* values, int num);

	void WriteString(const char* str);

private:
	std::ofstream ofs_;

public:
	operator bool() {
		return static_cast<bool>(ofs_);
	}
};

template<typename Ty>
inline void FileWriter::Write(const Ty& value) {
	ofs_.write((const char*)&value, sizeof(value));
}

template<>
inline void FileWriter::Write<std::string>(const std::string& value) {
	uint32_t len = static_cast<uint32_t>(value.length());
	ofs_.write((const char*)&len, sizeof(len));
	ofs_.write(value.c_str(), len);
}

template<typename Ty>
inline void FileWriter::WriteArray(const Ty* values, int num) {
	ofs_.write((const char*)values, sizeof(Ty) * num);
}

inline void FileWriter::WriteString(const char* str) {
	uint32_t len = static_cast<uint32_t>(strlen(str));
	ofs_.write((const char*)&len, sizeof(len));
	ofs_.write(str, len);
}
