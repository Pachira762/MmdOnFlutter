#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HMENU AddSubMenu(LPCTSTR text);

int AddMenuItem(HMENU hmenu, LPCTSTR text);

std::wstring GetWindowTitle(HWND hwnd);

void SetWindowTitle(HWND hwnd, const std::wstring& title);

std::wstring OpenSaveFileDialog(const wchar_t* title, const wchar_t* filter, const wchar_t* extension);
