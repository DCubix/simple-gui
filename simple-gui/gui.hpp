#ifndef GUI_HPP
#define GUI_HPP

#include <Windows.h>
#include <windowsx.h>

#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include <iostream>
#include <cstdint>

#include <gl/GL.h>
#pragma comment (lib, "opengl32.lib")

// thanks MaGetzUb
#pragma comment(linker,"/manifestdependency:\"type='win32' "\
                       "name='Microsoft.Windows.Common-Controls' "\
                       "version='6.0.0.0' "\
                       "processorArchitecture='*' "\
                       "publicKeyToken='6595b64144ccf1df' "\
                       "language='*' "\
                       "\"")

// thanks MaGetzUb
#include "utils.hpp"

namespace gui {

	enum class Flow {
		Horizontal = 0,
		Vertical
	};

	struct Size { int width, height; };
	struct Rect { int x, y, width, height; };

	using WID = size_t;
	using String = std::basic_string<TCHAR>;

	constexpr WID BaseWidgetID = 1000;
	
	class Container;
	class Manager;
	class Widget {
		friend class Manager;
	public:
		Widget() = default;
		~Widget() {
			DestroyWindow(handle);
		}

		virtual void create() = 0;
		virtual void updateAttributes() {};

		void update() {
			SetWindowPos(handle, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);
			updateAttributes();
		}

		HWND parentHandle{ nullptr };
		HWND handle{ nullptr };

		Size size{};
		int flex{ 1 };

		const WID& id() const { return m_id; }
		const WID& parent() const { return m_parent; }
		const Rect& actualBounds() const { return m_actualBounds; }

	protected:
		Rect m_actualBounds;
		WID m_id{ 0 }, m_parent{ 0 };
	};

	using WidgetMap = std::unordered_map<size_t, std::unique_ptr<Widget>>;

	enum class Alignment {
		Left = 0,
		Center,
		Right
	};

	class Label : public Widget {
	public:
		Label(const String& text = L"") : text(text) { }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				text.c_str(),
				WS_VISIBLE | WS_CHILD,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE) GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		void updateAttributes() {
			LONG_PTR align = SS_LEFT;
			switch (alignment) {
				case Alignment::Left: break;
				case Alignment::Center: align = SS_CENTER; break;
				case Alignment::Right: align = SS_RIGHT; break;
			}

			LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
			SetWindowLongPtr(handle, GWL_STYLE, style | align);

			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		Alignment alignment{ Alignment::Center };
		String text{ L"Text" };

		String fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };
	private:
		HFONT m_font{ nullptr };
	};

	class Button : public Widget {
	public:
		Button(const String& text = L"") : text(text) {}

		void create() {
			handle = CreateWindow(
				L"BUTTON",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		void updateAttributes() {
			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		String text{ L"Button" };
		std::function<void()> onPressed;

		String fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };
	private:
		HFONT m_font{ nullptr };
	};

	class TextBox : public Widget {
	public:
		~TextBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create() {
			m_wrapper = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)-1,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(m_wrapper, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(m_wrapper, TextBox::WndProc, 0, 0);

			handle = CreateWindow(
				L"EDIT",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				0, 0, size.width, size.height,
				m_wrapper,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					if (HIWORD(wParam) > 1) {
						auto id = LOWORD(wParam);
						auto cmd = HIWORD(wParam);

						if (id < BaseWidgetID) break;

						switch (cmd) {
							case EN_CHANGE: {
								HWND widgetHnd = (HWND)lParam;

								gui::Widget* widget = (gui::Widget*)GetWindowLongPtr(widgetHnd, GWLP_USERDATA);
								gui::TextBox* tb = static_cast<gui::TextBox*>(widget);
								if (id == tb->id()) {
									wchar_t buf[1024];
									GetWindowText(widgetHnd, buf, 1024);
									tb->text = String(buf);
								}
							} break;
						}
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		void updateAttributes() {
			SetWindowPos(m_wrapper, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);

			LONG_PTR align = ES_LEFT;
			switch (alignment) {
				case Alignment::Left: break;
				case Alignment::Center: align = ES_CENTER; break;
				case Alignment::Right: align = ES_RIGHT; break;
			}

			LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);

			if (multiLine) style |= ES_MULTILINE;
			if (password) style |= ES_PASSWORD;
			if (readOnly) {
				style |= ES_READONLY;
				SendMessage(handle, EM_SETREADONLY, TRUE, 0);
			}
			style |= align;

			SetWindowLongPtr(handle, GWL_STYLE, style);
			SetWindowText(handle, text.c_str());

			m_font = CreateFont(
				fontSize, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				fontFamily.c_str()
			);
			SendMessage(handle, WM_SETFONT, WPARAM(m_font), TRUE);
		}

		bool multiLine{ false }, password{ false }, readOnly{ false };
		Alignment alignment{ Alignment::Left };
		String text{ L"" };
		
		String fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };

	private:
		HFONT m_font{ nullptr };
		HWND m_wrapper;
	};

	class Spacer : public Widget {
	public:
		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_CHILD,
				0, 0, 1, 1,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
		}
	};

	class ListItem {
	public:
		virtual String toString() { return TEXT("gui::ListItem"); };
	};

	class SimpleListItem : public ListItem {
	public:
		SimpleListItem(String d) : data(d) {}
		String toString() override { return data; };

		String data;
	};

	class ListBox : public Widget {
	public:
		~ListBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create() {
			m_wrapper = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)-1,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(m_wrapper, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(m_wrapper, ListBox::WndProc, 0, 0);

			handle = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				L"LISTBOX",
				nullptr,
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY,
				0, 0, size.width, size.height,
				m_wrapper,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);

			for (int i = 0; i < m_items.size(); i++) {
				winapiAdd(i);
			}
			SendMessage(handle, LB_SETCURSEL, m_tempSelected, 0);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					HWND lbox = (HWND)lParam;
					auto id = LOWORD(wParam);
					auto cmd = HIWORD(wParam);

					if (id < BaseWidgetID) break;

					switch (cmd) {
						case LBN_SELCHANGE: {
							ListBox* lst = (ListBox*)GetWindowLongPtr(lbox, GWLP_USERDATA);
							int idx = SendMessage(lbox, LB_GETCURSEL, 0, 0);
							if (lst->onSelected) lst->onSelected(idx, lst->get(idx));
						} break;
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		void updateAttributes() {
			SetWindowPos(m_wrapper, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);
		}

		void add(ListItem* item) {
			m_items.push_back(std::unique_ptr<ListItem>(item));
			if (handle) winapiAdd(m_items.size() - 1);
		}

		void remove(int idx) {
			m_items.erase(m_items.begin() + idx);
			if (handle) {
				SendMessage(handle, LB_DELETESTRING, WPARAM(idx), 0);
			}
		}

		std::vector<int> selected() {
			std::vector<int> sels;
			if (!handle) return sels;
			
			LRESULT len = SendMessage(handle, LB_GETSELCOUNT, 0, 0);
			sels.resize(len);

			SendMessage(handle, LB_GETSELITEMS, len, LPARAM(sels.data()));
			return sels;
		}

		const std::vector<std::unique_ptr<ListItem>>& items() {
			return m_items;
		}

		ListItem* get(int id) {
			if (id < 0) return nullptr;
			if (!handle) return m_items[id].get();
			int dataID = SendMessage(handle, LB_GETITEMDATA, WPARAM(id), 0);
			return m_items[dataID].get();
		}

		void select(int id) {
			if (!handle) m_tempSelected = id;
			else SendMessage(handle, LB_SETCURSEL, id, 0);
		}

		std::function<void(int, ListItem*)> onSelected;

	private:
		HWND m_wrapper;
		std::vector<std::unique_ptr<ListItem>> m_items;
		int m_tempSelected{ -1 };

		void winapiAdd(int item) {
			ListItem* it = m_items[item].get();
			int pos = SendMessage(handle, LB_ADDSTRING, 0, (LPARAM)it->toString().c_str());
			SendMessage(handle, LB_SETITEMDATA, pos, LPARAM(item));
		}

	};

	class ComboBox : public Widget {
	public:
		~ComboBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create() {
			m_wrapper = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)-1,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(m_wrapper, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(m_wrapper, ComboBox::WndProc, 0, 0);

			handle = CreateWindow(
				WC_COMBOBOX,
				L"",
				WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST,
				0, 0, size.width, size.height,
				m_wrapper,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);

			for (int i = 0; i < m_items.size(); i++) {
				winapiAdd(i);
			}
			SendMessage(handle, CB_SETCURSEL, m_tempSelected, 0);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					HWND lbox = (HWND)lParam;
					auto id = LOWORD(wParam);
					auto cmd = HIWORD(wParam);

					if (id < BaseWidgetID) break;

					switch (cmd) {
						case CBN_SELCHANGE: {
							ComboBox* lst = (ComboBox*)GetWindowLongPtr(lbox, GWLP_USERDATA);
							int idx = SendMessage(lbox, CB_GETCURSEL, 0, 0);
							if (lst->onSelected) lst->onSelected(idx, lst->get(idx));
						} break;
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		void updateAttributes() {
			SetWindowPos(m_wrapper, nullptr, m_actualBounds.x, m_actualBounds.y, m_actualBounds.width, m_actualBounds.height, 0);
		}

		void add(ListItem* item) {
			m_items.push_back(std::unique_ptr<ListItem>(item));
			if (handle) winapiAdd(m_items.size() - 1);
		}

		void remove(int idx) {
			m_items.erase(m_items.begin() + idx);
			if (handle) {
				SendMessage(handle, CB_DELETESTRING, WPARAM(idx), 0);
			}
		}

		int selected() {
			if (!handle) return -1;
			return SendMessage(handle, CB_GETCURSEL, 0, 0);
		}

		const std::vector<std::unique_ptr<ListItem>>& items() {
			return m_items;
		}

		ListItem* get(int id) {
			if (id < 0) return nullptr;
			if (!handle) return m_items[id].get();
			int dataID = SendMessage(handle, CB_GETITEMDATA, WPARAM(id), 0);
			return m_items[dataID].get();
		}

		void select(int id) {
			if (!handle) m_tempSelected = id;
			else SendMessage(handle, CB_SETCURSEL, id, 0);
		}

		std::function<void(int, ListItem*)> onSelected;

	private:
		HWND m_wrapper;
		std::vector<std::unique_ptr<ListItem>> m_items;
		int m_tempSelected{ -1 };

		void winapiAdd(int item) {
			ListItem* it = m_items[item].get();
			int pos = ComboBox_AddString(handle, it->toString().c_str());
			ComboBox_SetItemData(handle, pos, item);
		}

	};

#pragma region OpenGL

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

	namespace gl {
		typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext, const int* attribList);
		wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

		typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int* piAttribIList,
			const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);
		wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;

		static void error(const std::wstring& text) {
			MessageBox(nullptr, text.c_str(), L"Error", MB_OK | MB_ICONERROR);
		}

		static void initializeGL() {
			if (wglCreateContextAttribsARB) return;

			WNDCLASSEX cls = {};
			cls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			cls.lpfnWndProc = DefWindowProcA;
			cls.hInstance = GetModuleHandle(0);
			cls.lpszClassName = L"Dummy_WGL";
			cls.cbSize = sizeof(WNDCLASSEX);

			if (!RegisterClassEx(&cls)) {
				error(L"Failed to register OpenGL window.");
				return;
			}

			HWND dummy_window = CreateWindowEx(
				0,
				cls.lpszClassName,
				L"Dummy OpenGL",
				0,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				cls.hInstance,
				0
			);

			if (!dummy_window) {
				error(L"Failed to create OpenGL window.");
			}

			HDC dummy_dc = GetDC(dummy_window);

			PIXELFORMATDESCRIPTOR pfd = {};
			pfd.nSize = sizeof(pfd);
			pfd.nVersion = 1;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.cColorBits = 32;
			pfd.cAlphaBits = 8;
			pfd.iLayerType = PFD_MAIN_PLANE;
			pfd.cDepthBits = 24;
			pfd.cStencilBits = 8;

			int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
			if (!pixel_format) {
				error(L"Failed to find a suitable pixel format.");
			}
			if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
				error(L"Failed to set the pixel format.");
			}

			HGLRC dummy_context = wglCreateContext(dummy_dc);
			if (!dummy_context) {
				error(L"Failed to create a dummy OpenGL rendering context.");
			}

			if (!wglMakeCurrent(dummy_dc, dummy_context)) {
				error(L"Failed to activate dummy OpenGL rendering context.");
			}

			wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
				"wglCreateContextAttribsARB");
			wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
				"wglChoosePixelFormatARB");

			wglMakeCurrent(dummy_dc, 0);
			wglDeleteContext(dummy_context);
			ReleaseDC(dummy_window, dummy_dc);
			DestroyWindow(dummy_window);
		}
	};

	struct OpenGLContextConfig {
		uint8_t depthBits{ 24 },
			stencilBits{ 8 },
			redBits{ 8 },
			greenBits{ 8 },
			blueBits{ 8 },
			alphaBits{ 8 },
			majorVersion{ 1 },
			minorVersion{ 1 };
		enum Profile {
			ProfileCore = WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			ProfileCompat = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
		} profile{ ProfileCompat };
	};

	class GLView : public Widget {
	public:
		~GLView() {
			if (m_context) wglDeleteContext(m_context);
			Widget::~Widget();
		}

		void create() {
			gl::initializeGL();

			WNDCLASS wc = { 0 };
			wc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = GLView::WndProc;
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.lpszClassName = TEXT("GLVIEW");
			RegisterClass(&wc);

			handle = CreateWindow(
				TEXT("GLVIEW"),
				nullptr,
				WS_VISIBLE | WS_CHILD | WS_BORDER | SS_OWNERDRAW,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);

			int pixel_format_attribs[] = {
				WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
				WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
				WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB,         32,
				WGL_DEPTH_BITS_ARB,         m_config.depthBits,
				WGL_STENCIL_BITS_ARB,       m_config.stencilBits,
				0
			};

			m_dc = GetDC(handle);
			
			int pixel_format;
			UINT num_formats;
			if (!gl::wglChoosePixelFormatARB(m_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats)) {
				MessageBox(nullptr, L"Failed to choose pixel format.", L"Error", MB_OK);
				return;
			}

			PIXELFORMATDESCRIPTOR pfd = {};
			DescribePixelFormat(m_dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

			pfd.cRedBits = m_config.redBits;
			pfd.cGreenBits = m_config.greenBits;
			pfd.cBlueBits = m_config.blueBits;
			pfd.cAlphaBits = m_config.alphaBits;

			if (!SetPixelFormat(m_dc, pixel_format, &pfd)) {
				MessageBox(nullptr, L"Failed to set pixel format.", L"Error", MB_OK);
				return;
			}
			
			int gl_attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, m_config.majorVersion,
				WGL_CONTEXT_MINOR_VERSION_ARB, m_config.minorVersion,
				WGL_CONTEXT_PROFILE_MASK_ARB,  m_config.profile,
				0,
			};
			
			m_context = gl::wglCreateContextAttribsARB(m_dc, 0, gl_attribs);
			if (!wglMakeCurrent(m_dc, m_context)) {
				MessageBox(nullptr, L"Failed to make OpenGL context as current.", L"Error", MB_OK);
				return;
			}
		}

		virtual void onPaint() {}

		void swapBuffers() {
			SwapBuffers(m_dc);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			GLView* glv = (GLView*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			switch (uMsg) {
				case WM_PAINT:
					glViewport(0, 0, glv->actualBounds().width, glv->actualBounds().height);
					glv->onPaint();
					glv->swapBuffers();
					return 0;
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	private:
		HDC m_dc;
		OpenGLContextConfig m_config;
		HGLRC m_context;
	};
#pragma endregion

	class Container : public Widget {
	public:
		Container() = default;
		Container(Size b, Flow flow = Flow::Horizontal) { size = b; this->flow = flow; }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				0, 0, size.width, size.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parentHandle, GWLP_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);
			SetWindowSubclass(handle, Container::WndProc, 0, 0);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					auto id = LOWORD(wParam);

					if (id < BaseWidgetID) return DefSubclassProc(hwnd, uMsg, wParam, lParam);
					HWND widgetHnd = GetDlgItem(hwnd, id);
					if (!widgetHnd) return DefSubclassProc(hwnd, uMsg, wParam, lParam);

					Widget* widget = (Widget*)GetWindowLongPtr(widgetHnd, GWLP_USERDATA);
					// TODO: Make this not hacky
					Button* btn = dynamic_cast<Button*>(widget);
					if (btn && btn->onPressed && id == btn->id()) {
						btn->onPressed();
					}

				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}

		int border{ 4 }, spacing{ 4 };
		Flow flow{ Flow::Horizontal };

		std::vector<WID> children{};
	};

	class Manager {
	public:
		Manager() = default;
		~Manager() = default;

		template <class W, typename... Args>
		W& create(WID parent, Args&&... args) {
			size_t id = m_genID++;
			auto widget = std::make_unique<W>(std::forward<Args>(args)...);
			widget->m_id = id;

			if (parent) {
				Container* cont = dynamic_cast<Container*>(get(parent));
				if (cont) {
					cont->children.push_back(id);
					widget->m_parent = parent;
				}
			}

			m_widgets[id] = std::move(widget);
			return *((W*)m_widgets[id].get());
		}

		Widget* get(WID id) {
			return m_widgets[id].get();
		}

		void createWidgets(HWND parentWindow) {
			for (auto& [id, widget] : m_widgets) {
				widget->parentHandle = parentWindow;
				widget->create();
			}

			// Rearrange parents
			for (auto& [id, widget] : m_widgets) {
				if (widget->parent()) {
					widget->parentHandle = get(widget->parent())->handle;
					SetParent(widget->handle, widget->parentHandle);
				}
			}

			updateWidgets();
		}

		void updateWidgets() {
			performContainerLayout(BaseWidgetID);
			for (auto& [id, widget] : m_widgets) {
				widget->update();
			}
		}

		std::vector<WID> widgets() {
			std::vector<WID> ids;
			for (auto& [id, widget] : m_widgets) {
				ids.push_back(id);
			}
			return ids;
		}

	private:
		size_t m_genID{ BaseWidgetID };
		WidgetMap m_widgets;

		void performContainerLayout(WID id) {
			if (m_widgets.find(id) == m_widgets.end()) return;

			Widget* wid = get(id);
			if (wid->parent() == 0) {
				wid->m_actualBounds = Rect{ 0, 0, wid->size.width, wid->size.height };
			}

			int totalProportions = 0;
			Container* cont = dynamic_cast<Container*>(wid);
			if (cont == nullptr) return;

			if (cont->children.empty()) return;

			for (auto& e : cont->children) {
				totalProportions += get(e)->flex;
			}
			totalProportions = totalProportions <= 0 ? 1 : totalProportions;

			Rect parentBounds = cont->m_actualBounds;
			int& compSize = cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height;
			int& comp = cont->flow == Flow::Horizontal ? parentBounds.x : parentBounds.y;

			for (auto& e : cont->children) {
				Widget* c = get(e);
				int size = cont->flow == Flow::Horizontal ? c->size.width : c->size.height;
				if (c->flex == 0) {
					compSize -= size;
				}
			}
			const int size = (cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height) - cont->border * 2;

			int pbx = parentBounds.x,
				pby = parentBounds.y;
			for (auto& e : cont->children) {
				Widget* c = get(e);

				Rect ret = Rect{ 0, 0, c->size.width, c->size.height };
				if (cont->parent()) {
					ret.x -= pbx;
					ret.y -= pby;
				}
				ret.x += parentBounds.x + cont->border;
				ret.y += parentBounds.y + cont->border;

				switch (cont->flow) {
					case Flow::Horizontal: {
						const int sizeSlice = (size - (c->flex <= 0 ? ret.width : 0)) / totalProportions;
						if (c->flex >= 1) {
							ret.width = sizeSlice * c->flex;
						}
						ret.height = parentBounds.height - cont->border * 2;
						parentBounds.x += ret.width;
						parentBounds.width -= ret.width;
						if (e != cont->children.back()) ret.width -= cont->spacing;
					} break;
					case Flow::Vertical: {
						const int sizeSlice = (size - (c->flex <= 0 ? ret.height : 0)) / totalProportions;
						if (c->flex >= 1) {
							ret.height = sizeSlice * c->flex;
						}
						ret.width = parentBounds.width - cont->border * 2;
						parentBounds.y += ret.height;
						parentBounds.height -= ret.height;
						if (e != cont->children.back()) ret.height -= cont->spacing;
					} break;
				}

				c->m_actualBounds = ret;
				performContainerLayout(e);
			}
		}
	};

	static size_t WindowID = 0;

	constexpr int DefaultWidth = 320;
	constexpr int DefaultHeight = 240;

	class Window {
	public:
		Window() = default;
		~Window() {
			DestroyWindow(m_hwnd);
		}

		virtual void onCreate(Manager&) {}

		void show() {
			WNDCLASSEX cex = {};

			HINSTANCE instance = GetModuleHandle(nullptr);

			String cls = String(L"WAPIDSA_") + std::to_wstring(WindowID++);

			cex.cbSize = sizeof(WNDCLASSEX);
			cex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			cex.hCursor = LoadCursor(instance, IDC_ARROW);
			cex.hIcon = LoadIcon(instance, IDI_WINLOGO);
			cex.hInstance = instance;
			cex.lpfnWndProc = Window::WndProc;
			cex.lpszClassName = cls.c_str();
			cex.style = CS_HREDRAW | CS_VREDRAW;
			cex.cbWndExtra = 0;
			cex.cbClsExtra = 0;
			cex.lpszMenuName = NULL;

			if (!RegisterClassEx(&cex)) {
				MessageBox(NULL, L"Failed to initialize window.", L"Error", MB_OK);
				return;
			}

			RECT wect;
			wect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - DefaultWidth / 2;
			wect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - DefaultHeight / 2;
			wect.right = wect.left + DefaultWidth;
			wect.bottom = wect.top + DefaultHeight;

			AdjustWindowRectEx(&wect, WS_OVERLAPPEDWINDOW, 0, 0);

			m_hwnd = CreateWindowEx(
				0,
				cls.c_str(),
				L"Window",
				WS_OVERLAPPEDWINDOW,
				wect.left, wect.top, wect.right - wect.left, wect.bottom - wect.top,
				NULL, NULL,
				instance,
				NULL
			);

			m_manager = std::make_unique<Manager>();
			SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)m_manager.get());

			m_manager->create<Container>(0, Size{ 1, 1 });
			onCreate(*m_manager.get());

			ShowWindow(m_hwnd, SW_SHOW);
		}

		Container& root() { return *((Container*) m_manager->get(BaseWidgetID)); }

		static void mainLoop() {
			MSG msg = {};
			while (GetMessage(&msg, nullptr, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg) {
				case WM_CLOSE: DestroyWindow(hwnd); break;
				case WM_SHOWWINDOW: {
					Manager* man = (Manager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
					man->createWidgets(hwnd);

					HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
					SendMessage(hwnd, WM_SETFONT, WPARAM(font), TRUE);

					for (WID id : man->widgets()) {
						Widget* wid = man->get(id);
						SendMessage(wid->handle, WM_SETFONT, WPARAM(font), TRUE);
					}
				} break;
				case WM_DESTROY: PostQuitMessage(0); break;
				case WM_SIZE: {
					Manager* man = (Manager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
					UINT width = LOWORD(lParam);
					UINT height = HIWORD(lParam);
					Widget* cont = man->get(BaseWidgetID);
					if (cont) {
						cont->size.width = width;
						cont->size.height = height;
						man->updateWidgets();

						InvalidateRect(hwnd, nullptr, true);
						UpdateWindow(hwnd);
					}
				} break;
				case WM_PAINT: {
					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(hwnd, &ps);
					FillRect(hdc, &ps.rcPaint, (HBRUSH)COLOR_WINDOW);
					EndPaint(hwnd, &ps);
				} break;
			}
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		void resize(int width, int height) {	
			RECT wect;
			GetWindowRect(m_hwnd, &wect);

			SetWindowPos(m_hwnd, nullptr, wect.left, wect.top, width, height, 0);

			RECT nect;
			nect.left = wect.left;
			nect.top = wect.top;
			nect.right = wect.left + width;
			nect.bottom = wect.top + height;

			AdjustWindowRectEx(&nect, WS_OVERLAPPEDWINDOW, 0, 0);
		}

		void setTitle(const String& title) {
			SetWindowText(m_hwnd, title.c_str());
		}

	private:
		HWND m_hwnd;
		std::unique_ptr<Manager> m_manager;
	};

}

#endif // GUI_HPP