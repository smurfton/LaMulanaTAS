#pragma once
#include <string>
#include <vector>
#include <sal.h>
#include <memory>
#include <exception>
#include <d3d9.h>
#include <atlbase.h>

std::string strprintf(_In_z_ _Printf_format_string_ const char * const fmt, ...);
std::wstring wstrprintf(_In_z_ _Printf_format_string_ const wchar_t * const fmt, ...);

template<typename T> class vararray
{
public:
	T* ptr;
	size_t count;
	vararray(T* ptr, size_t count) : ptr(ptr), count(count) {}
	T* begin() {
		return ptr;
	}
	T* end() {
		return ptr + count;
	}
};

std::string getwinerror();
HRESULT d3d9check(HRESULT hr, const char *file, int line, const char *code);
#define WINCHECKED(x) ((x) ? (void)0 : throw std::runtime_error(strprintf("%s:%d %s\n%s", __FILE__, __LINE__, #x, getwinerror().data())))
#define D3D9CHECKED(x) (d3d9check((x), __FILE__, __LINE__, #x))

using unique_handle = std::unique_ptr < std::remove_pointer<HANDLE>::type, decltype(&::CloseHandle)>;
using shared_handle = std::shared_ptr < std::remove_pointer<HANDLE>::type>;

#define BMFALIGN_LEFT	0
#define BMFALIGN_TOP	0
#define BMFALIGN_RIGHT	1
#define BMFALIGN_BOTTOM	0x10

class BitmapFont
{
public:
	BitmapFont(IDirect3DDevice9* dev, int w, int h, HMODULE mod, int res);
	void Add(int x, int y, int align, D3DCOLOR color, const std::string &text);
	void Draw(D3DCOLOR backcolor = D3DCOLOR_ARGB(0,0,0,0));

private:
	BitmapFont(IDirect3DDevice9* dev, int w, int h);

	static const DWORD fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	struct vertex
	{
		float x, y, z, w;
		D3DCOLOR color;
		float u, v;
	};
	CComPtr<IDirect3DTexture9> tex;
	IDirect3DDevice9 *dev;
	int char_w, char_h;
	float char_texw, char_texh;
	std::vector<vertex> vert;
	std::vector<USHORT> index;
	size_t chars;
};
