// ShaderBlobGen.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <vector>
#include <fstream>
#include <iostream>
#include <wrl.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

ComPtr<ID3DBlob> CompileShader(const std::wstring& path, const char* entry, const char* target, bool debug) {
    ComPtr<ID3DBlob> code{}, error{};
    auto hr = D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, target, debug ? D3DCOMPILE_DEBUG : 0, 0, &code, &error);
    if (FAILED(hr)) {
        if (error && error->GetBufferPointer()) {
            printf("%s\n", (const char*)error->GetBufferPointer());
        }
    }

    return code;
}

std::vector<char> GenerateHexText(const unsigned char* bin, std::size_t size) {
    constexpr char hex[16] = { '0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    std::vector<char> text_buf(size * 5 + size / 16);
    char* text = text_buf.data();

    for (std::size_t i = 0; i < size; ++i) {
        text[0] = '0';
        text[1] = 'x';
        text[2] = hex[bin[i] >> 4];
        text[3] = hex[bin[i] & 0x0f];
        text[4] = ',';
        
        if (i % 16 == 15) {
            text[5] = '\n';
            text += 6;
        }
        else {
            text += 5;
        }
    }

    return text_buf;
}

std::wstring GeneratePath(const std::wstring& path, const char* entry) {
    wchar_t buf[256]{};
    swprintf_s(buf, L"_%S.txt", entry);
    return path + buf;
}

bool SaveText(const std::wstring& path, const std::vector<char>& text) {
    auto ofs = std::ofstream(path);
    if (!ofs) {
        return false;
    }

    ofs.write(text.data(), text.size());
    return true;
}

bool GenerateShaderBlob(const std::wstring& path, const char* entry, const char* target, bool debug) {
    ComPtr<ID3DBlob> shader = CompileShader(path, entry, target, debug);
    if (!shader) {
        return false;
    }

    auto text = GenerateHexText((const unsigned char*)shader->GetBufferPointer(), shader->GetBufferSize());

    if (!SaveText(GeneratePath(path, entry), text)) {
        return false;
    }

    if (shader->GetBufferSize() > 64) {
        auto ptr = reinterpret_cast<const unsigned char*>(shader->GetBufferPointer());
        for (int i = 0; i < 64; ++i) {
            printf("0x%02X,", ptr[i]);
        }

        printf("\ntext\n");
        for (int i = 0; i < 64 * 5; ++i) {
            printf("%c", text[i]);
        }
        printf("\n");
    }

    return true;
}

int main(int argc, char* argv[]) {
    GenerateShaderBlob(L"D:\\Projects\\MmdOnFlutter\\HeadlessMmdEngine\\MeshPass.hlsl", "MainVS", "vs_5_0", false);
    GenerateShaderBlob(L"D:\\Projects\\MmdOnFlutter\\HeadlessMmdEngine\\MeshPass.hlsl", "MainPS", "ps_5_0", false);

    return 0;
}
