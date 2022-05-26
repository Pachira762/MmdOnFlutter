#include "pch.h"
#include "DxContext.h"
#include "Texture.h"

namespace headless_mmd {

bool DxContext::Init(HWND hwnd) {
	if (!super::Init(hwnd)) {
		return false;
	}

	Texture::CreateDummyTexture(this);

	return true;
}

}