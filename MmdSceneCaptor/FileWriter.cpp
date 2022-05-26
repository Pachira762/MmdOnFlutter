#include "pch.h"
#include "FileWriter.h"

FileWriter::FileWriter(const wchar_t* path) :ofs_(path, std::ios::binary) {
}
