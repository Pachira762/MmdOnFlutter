#pragma once
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#include <shellapi.h>

namespace winapp {
	inline POINT GetPosition(const RECT& rc) {
		return { rc.left, rc.top };
	}

	inline SIZE GetSize(const RECT& rc) {
		return { rc.right - rc.left,rc.bottom - rc.top };
	}

	inline RECT GetWindowRect(HWND hwnd) {
		RECT rc{};
		::GetWindowRect(hwnd, &rc);
		return rc;
	}

	inline SIZE GetClientSize(HWND hwnd) {
		RECT rc{};
		::GetClientRect(hwnd, &rc);
		return { rc.right - rc.left, rc.bottom - rc.top };
	}

	inline POINT GetCursorPos() {
		POINT pos{};
		::GetCursorPos(&pos);
		return pos;
	}

	class WinApp {
	public:
		virtual ~WinApp() = default;

		int Run();

	protected:
		virtual bool	OnInit() { return true; }
		virtual bool	OnCreate(HWND hwnd) { return true; }
		virtual void	OnClose() {}
		virtual void	OnDestroy() {}
		virtual void	OnSize(int width, int height) {}
		virtual void	OnPaint() {}
		virtual LRESULT	OnNotify(WPARAM id, NMHDR* nmh) { return 0; }
		virtual void	OnCommand(WORD id, WORD code, HWND handle) {}
		virtual void	OnHScroll(WORD code, WORD pos, HWND hscroll) {}
		virtual void	OnVScroll(WORD code, WORD pos, HWND hscroll) {}
		virtual void	OnTimer(int timer) {}
		virtual HBRUSH	OnControlColor(HDC hdc, HWND hcontrol) { return QueryBgBrush(); }
		virtual void	OnKeyDown(int key) {}
		virtual void	OnKeyHold(int key) {}
		virtual void	OnKeyUp(int key) {}
		virtual void	OnLClick() {}
		virtual void	OnRClick() {}
		virtual void	OnWClick() {}
		virtual void	OnLDoubleClick() {}
		virtual void	OnRDoubleClick() {}
		virtual void	OnWDoubleClick() {}
		virtual void	OnLRelease() {}
		virtual void	OnRRelease() {}
		virtual void	OnWRelease() {}
		virtual void	OnMouseMove(int x, int y) {}
		virtual void	OnMouseDrag(int mx, int my, int dx, int dy) {};
		virtual void	OnMouseWheel(int delta) {}
		virtual void	OnFileDrop(const wchar_t* path) {}
		virtual void	OnUpdate() {}

		virtual LPCTSTR	QueryWindowTitle() {
			return L"App";
		}

		virtual LPCTSTR QueryWindowClassName() {
			return L"Win32App";
		}

		virtual UINT	QueryWindowClassStyle() {
			return CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		}

		virtual DWORD	QueryWindowStyle() {
			return WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
		}

		virtual DWORD	QueryWindowStyleEx() {
			return WS_EX_ACCEPTFILES;
		}

		virtual POINT	QueryWindowPos() {
			return { CW_USEDEFAULT, CW_USEDEFAULT };
		}

		virtual SIZE	QueryWindowSize() {
			return { CW_USEDEFAULT, CW_USEDEFAULT };
		}
		
		virtual void	QueryMinWindowSize(POINT* size) {
			size->x = 200;
			size->y = 150;
		}

		virtual HMENU	QueryMenuHandle() {
			return NULL;
		}

		virtual HBRUSH	QueryBgBrush() {
			return (HBRUSH)GetStockObject(WHITE_BRUSH);
		}

		virtual bool QueryMessagePooling() {
			return false;
		}

	private:
		HWND	hwnd_ = NULL;

	protected:
		HWND	GetHWND() const {
			return hwnd_;
		}

		bool ToggleFullscreen() {
			static bool fullscreen = false;
			static DWORD style = 0;
			static POINT pos{};
			static SIZE size{};

			if (!fullscreen) {
				auto rc = GetWindowRect(hwnd_);
				pos = GetPosition(rc);
				size = GetSize(rc);
				style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));

				SetWindowLongPtr(hwnd_, GWL_STYLE, (style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP);
				SetWindowPos(hwnd_, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0);
				fullscreen = true;
			}
			else {
				SetWindowLongPtr(hwnd_, GWL_STYLE, style);
				SetWindowPos(hwnd_, NULL, pos.x, pos.y, size.cx, size.cy, SWP_NOZORDER);
				fullscreen = false;
			}

			return fullscreen;
		}

	private:
		int OnCreateInternal(HWND hwnd) {
			if (hwnd_) {
				return 0;
			}

			hwnd_ = hwnd;
			return OnCreate(hwnd) ? 0 : -1;
		}

		LRESULT OnNotifyInternal(WPARAM id, NMHDR* nmh) {
			return OnNotify(id, nmh);
		}

		void OnCommandInternal(WORD id, WORD code, HWND handle) {
			OnCommand(id, code, handle);
		}

		void OnHScrollInternal(WORD code, WORD pos, HWND hscroll) {
			OnHScroll(code, pos, hscroll);
		}

		void OnVScrollInternal(WORD code, WORD pos, HWND hscroll) {
			OnVScroll(code, pos, hscroll);
		}

		void OnDropFilesInternal(HDROP hdrop) {
			TCHAR path[MAX_PATH + 1]{};
			if (DragQueryFile(hdrop, 0, path, MAX_PATH) > 0) {
				OnFileDrop(path);
			}
			DragFinish(hdrop);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
			enum class Mouse {
				Left,
				Right,
				Wheel,
				Num
			};

			static WinApp* self = nullptr;
			static int mx = 0, my = 0;
			static int dmx = 0, dmy = 0;
			static bool	mbutton[(int)Mouse::Num]{};

			switch (msg) {
			case WM_CREATE:
				self = reinterpret_cast<WinApp*>(reinterpret_cast<CREATESTRUCT*>(lp)->lpCreateParams);
				return self ? self->OnCreateInternal(hwnd) : -1;

			case WM_CLOSE:
				self->OnClose();
				DestroyWindow(hwnd);
				return 0;

			case WM_DESTROY:
				if (self) {
					self->OnDestroy();
				}
				PostQuitMessage(0);
				return 0;

			case WM_SIZE:
				self->OnSize(LOWORD(lp), HIWORD(lp));
				return 0;

			case WM_PAINT:
				self->OnPaint();
				ValidateRect(hwnd, nullptr);
				return 0;

			case WM_NOTIFY:
				return self->OnNotifyInternal(wp, (NMHDR*)lp);

			case WM_COMMAND:
				self->OnCommandInternal(LOWORD(wp), HIWORD(wp), (HWND)lp);
				return 0;

			case WM_HSCROLL:
				self->OnHScrollInternal(LOWORD(wp), HIWORD(wp), (HWND)lp);
				return 0;

			case WM_VSCROLL:
				self->OnVScrollInternal(LOWORD(wp), HIWORD(wp), (HWND)lp);
				return 0;

			case WM_TIMER:
				self->OnTimer(static_cast<int>(wp));
				return 0;

			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLORSTATIC:
				return (LRESULT)self->OnControlColor((HDC)wp, (HWND)lp);

			case WM_KEYDOWN:
				(HIWORD(lp) & KF_REPEAT) ? self->OnKeyHold(static_cast<int>(wp)) : self->OnKeyDown(static_cast<int>(wp));
				return 0;

			case WM_KEYUP:
				self->OnKeyUp(static_cast<int>(wp));
				return 0;

			case WM_LBUTTONDOWN: mbutton[(int)Mouse::Left] = true;	SetCapture(hwnd); self->OnLClick(); return 0;
			case WM_RBUTTONDOWN: mbutton[(int)Mouse::Right] = true;	self->OnRClick(); return 0;
			case WM_MBUTTONDOWN: mbutton[(int)Mouse::Wheel] = true;	self->OnWClick(); return 0;

			case WM_LBUTTONDBLCLK: mbutton[(int)Mouse::Left] = true;	self->OnLDoubleClick(); return 0;
			case WM_RBUTTONDBLCLK: mbutton[(int)Mouse::Right] = true;	self->OnRDoubleClick(); return 0;
			case WM_MBUTTONDBLCLK: mbutton[(int)Mouse::Wheel] = true;	self->OnWDoubleClick(); return 0;

			case WM_LBUTTONUP: mbutton[(int)Mouse::Left] = false;	ReleaseCapture(); self->OnLRelease(); return 0;
			case WM_RBUTTONUP: mbutton[(int)Mouse::Right] = false;	self->OnRRelease(); return 0;
			case WM_MBUTTONUP: mbutton[(int)Mouse::Wheel] = false;	self->OnWRelease(); return 0;

			case WM_MOUSEMOVE:
				dmx = GET_X_LPARAM(lp) - mx; mx = GET_X_LPARAM(lp);
				dmy = GET_Y_LPARAM(lp) - my; my = GET_Y_LPARAM(lp);
				mbutton[(int)Mouse::Left] ? self->OnMouseDrag(mx, my, dmx, dmy) : self->OnMouseMove(mx, my);
				return 0;

			case WM_MOUSEWHEEL:
				self->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wp));
				return 0;

			case WM_DROPFILES:
				self->OnDropFilesInternal((HDROP)wp);
				return 0;

			case WM_GETMINMAXINFO:
				if (self) {
					self->QueryMinWindowSize(&reinterpret_cast<MINMAXINFO*>(lp)->ptMinTrackSize);
				}
				return 0;

			default:
				return DefWindowProc(hwnd, msg, wp, lp);
			}
		}
	};

	inline int WinApp::Run() {
		if (!OnInit()) {
			return -1;
		}

		auto hinstance = GetModuleHandle(NULL);

		WNDCLASSEX wc{};
		wc.cbSize = sizeof(wc);
		wc.style = QueryWindowClassStyle();
		wc.lpfnWndProc = WinApp::WndProc;
		wc.hInstance = hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = QueryBgBrush();
		wc.lpszClassName = QueryWindowClassName();
		if (RegisterClassEx(&wc) == INVALID_ATOM) {
			return -1;
		}

		auto [x, y] = QueryWindowPos();
		auto [w, h] = QueryWindowSize();
		auto ret = CreateWindowEx(QueryWindowStyleEx(), QueryWindowClassName(), QueryWindowTitle(), QueryWindowStyle(),
			x, y, w, h, NULL, QueryMenuHandle(), hinstance, this);
		if (!ret) {
			return -1;
		}

		MSG msg{};
		if (QueryMessagePooling()) {
			while (true) {
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
					if (msg.message == WM_QUIT) {
						break;
					}

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					OnUpdate();
					Sleep(1);
				}
			}
		}
		else {
			while (true) {
				BOOL br = GetMessage(&msg, NULL, 0, 0);
				if (br == 0 || br == -1) {
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return static_cast<int>(msg.wParam);
	}
};
