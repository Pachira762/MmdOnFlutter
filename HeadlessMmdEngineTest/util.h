#pragma once
#include <string>
#include <stringapiset.h>
#include <format>

template<typename To = wchar_t, typename From = char>
inline std::basic_string<To> MbsToWcs(const From* mbs, UINT code = CP_ACP) {
	static_assert(sizeof(To) == sizeof(wchar_t) && sizeof(From) == sizeof(char));

	const auto len = MultiByteToWideChar(code, 0, reinterpret_cast<const char*>(mbs), -1, nullptr, 0);
	if (len <= 1) { // failed or null string
		return {};
	}

	std::basic_string<To> wstr(static_cast<std::size_t>(len - 1), 0);
	MultiByteToWideChar(code, 0, reinterpret_cast<const char*>(mbs), -1, reinterpret_cast<wchar_t*>(wstr.data()), len);
	return wstr;
}

template<typename To = char, typename From = wchar_t>
inline std::basic_string<To> WcsToMbs(const From* wcs, UINT code = CP_ACP) {
	static_assert(sizeof(To) == sizeof(char) && sizeof(From) == sizeof(wchar_t));

	const auto len = WideCharToMultiByte(code, 0, reinterpret_cast<const wchar_t*>(wcs), -1, nullptr, 0, nullptr, FALSE);
	if (len <= 1) { // failed or null string
		return {};
	}

	std::basic_string<To> str(static_cast<std::size_t>(len - 1), 0);
	WideCharToMultiByte(code, 0, reinterpret_cast<const wchar_t*>(wcs), -1, reinterpret_cast<char*>(str.data()), len, nullptr, FALSE);
	return str;
}

template<typename To = char> // char or char8_t
inline std::basic_string<To> SjisToUtf8(const char* sjis) {
	auto utf16 = MbsToWcs(sjis, 932);	// SJI CP932
	return WcsToMbs<To>(utf16.c_str(), CP_UTF8);
}

template<typename To = char>
inline std::basic_string<To> SjisToUtf8(const std::string& sjis) {
	return SjisToUtf8<To>(sjis.c_str());
}

inline std::wstring SjisToUtf16(const char* sjis) {
	return MbsToWcs(sjis, 932);
}

inline std::wstring SjisToUtf16(const std::string& sjis) {
	return SjisToUtf16(sjis.c_str());
}

template<typename To = char, typename From = char>
inline std::basic_string<To> Utf8ToSjis(const From* utf8) {
	auto utf16 = MbsToWcs(utf8, CP_UTF8);
	return WcsToMbs<To>(utf16.c_str(), 932);
}

template<typename To = char, typename From = char>
inline std::basic_string<To> Utf8ToSjis(const std::basic_string<From>& utf8) {
	return Utf8ToSjis<To>(utf8.c_str());
}

template<typename From = char>
inline std::wstring Utf8ToUtf16(const From* utf8) {
	return MbsToWcs(utf8, CP_UTF8);
}

template<typename From = char>
inline std::wstring Utf8ToUtf16(const std::basic_string<From>& utf8) {
	return Utf8ToUtf16(utf8.c_str());
}

inline std::string Utf16ToSjis(const wchar_t* utf16) {
	return WcsToMbs(utf16, 932);
}

inline std::string Utf16ToSjis(const std::wstring& utf16) {
	return Utf16ToSjis(utf16.c_str());
}

template<typename To = char>
inline std::basic_string<To> Utf16ToUtf8(const wchar_t* utf16) {
	return WcsToMbs<To>(utf16, CP_UTF8);
}

template<typename To = char>
inline std::basic_string<To> Utf16ToUtf8(const std::wstring& utf16) {
	return Utf16ToUtf8<To>(utf16.c_str());
}

template<> struct std::formatter<const char*, wchar_t> : std::formatter<std::wstring, wchar_t> {
	auto format(const char* str, wformat_context& ctx) {
		return formatter<std::wstring, wchar_t>::format(SjisToUtf16(str), ctx);
	}
};

template<std::size_t N> struct std::formatter<char[N], wchar_t> : std::formatter<std::wstring, wchar_t> {
	auto format(const char(&str)[N], wformat_context& ctx) {
		return formatter<std::wstring, wchar_t>::format(SjisToUtf16(str), ctx);
	}
};

template<> struct std::formatter<std::string, wchar_t> : std::formatter<std::wstring, wchar_t> {
	auto format(const std::string& str, wformat_context& ctx) {
		return formatter<std::wstring, wchar_t>::format(SjisToUtf16(str), ctx);
	}
};
