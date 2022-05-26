#include "pch.h"
#include "WinAppHelper.h"
#include <commdlg.h>

HMENU AddSubMenu(LPCTSTR text) {
	HMENU hsub = CreatePopupMenu();

	MENUITEMINFO mii = {};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_TYPE | MIIM_SUBMENU;
	mii.fType = MFT_STRING;
	mii.hSubMenu = hsub;
	mii.dwTypeData = const_cast<LPTSTR>(text);
	InsertMenuItem(GetMenu(getHWND()), UINT_MAX, TRUE, &mii);

	return hsub;
}

int AddMenuItem(HMENU hmenu, LPCTSTR text) {
	auto id = createWM_APP_ID();

	MENUITEMINFO mii = {};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_ID | MIIM_TYPE;
	mii.fType = MFT_STRING;
	mii.wID = id;
	mii.dwTypeData = const_cast<LPTSTR>(text);
	InsertMenuItem(hmenu, id, FALSE, &mii);

	return id;
}

std::wstring GetWindowTitle(HWND hwnd) {
	TCHAR buff[1024]{};
	GetWindowText(hwnd, buff, _countof(buff));
	return buff;
}

void SetWindowTitle(HWND hwnd, const std::wstring& title) {
	SetWindowText(hwnd, title.c_str());
}

std::wstring OpenSaveFileDialog(const wchar_t* title, const wchar_t* filter, const wchar_t* extension) {
	TCHAR path[MAX_PATH] = {};
	OPENFILENAME ofn = {};

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = getHWND();
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = path;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = title;
	ofn.lpstrDefExt = extension;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn) == 0) {
		return {};
	}

	return path;
}
