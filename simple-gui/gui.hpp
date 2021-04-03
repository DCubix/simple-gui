#ifndef GUI_HPP
#define GUI_HPP

#include <Windows.h>

#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include <iostream>

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

	struct Rect { int x, y, width, height; };

	using WID = size_t;
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
		Rect bounds{};

		int flex{ 1 };

		WID parent{ 0 };

		const WID& id() const { return m_id; }

		bool m_alreadyInitialized{ false };
	protected:
		Rect m_actualBounds;
		WID m_id{ 0 };
	};

	using WidgetMap = std::unordered_map<size_t, std::unique_ptr<Widget>>;

	enum class Alignment {
		Left = 0,
		Center,
		Right
	};

	class Text : public Widget {
	public:
		Text(const std::wstring& text = L"") : text(text) { }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				text.c_str(),
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
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
		std::wstring text{ L"Text" };

		std::wstring fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };
	private:
		HFONT m_font{ nullptr };
	};

	class Button : public Widget {
	public:
		Button(const std::wstring& text = L"") : text(text) {}

		void create() {
			handle = CreateWindow(
				L"BUTTON",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON,
				bounds.x, bounds.y, bounds.width, bounds.height,
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

		std::wstring text{ L"Button" };
		std::function<void()> onPressed;

		std::wstring fontFamily{ L"Segoe UI" };
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
				bounds.x, bounds.y, bounds.width, bounds.height,
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
				bounds.x, bounds.y, bounds.width, bounds.height,
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
									tb->text = std::wstring(buf);
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
		std::wstring text{ L"" };
		
		std::wstring fontFamily{ L"Segoe UI" };
		int fontSize{ 16 };

	private:
		HFONT m_font{ nullptr };
		HWND m_wrapper;
	};

	class Container : public Widget {
	public:
		Container() = default;
		Container(Rect b, Flow flow = Flow::Horizontal) { bounds = b; this->flow = flow; }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
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
					widget->parent = parent;
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
				if (widget->parent) {
					widget->parentHandle = get(widget->parent)->handle;
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
			if (wid->parent == 0) {
				wid->m_actualBounds = wid->bounds;
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
				int size = cont->flow == Flow::Horizontal ? c->bounds.width : c->bounds.height;
				if (c->flex == 0) {
					compSize -= size;
				}
			}
			const int size = (cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height) - cont->border * 2;

			int pbx = parentBounds.x,
				pby = parentBounds.y;
			for (auto& e : cont->children) {
				Widget* c = get(e);

				Rect ret = c->bounds;
				if (cont->parent) {
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
	static ThreadPool TPool{ 20 };

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

			std::wstring cls = std::wstring(L"WAPIDSA_") + std::to_wstring(WindowID++);

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

			ShowWindow(m_hwnd, SW_SHOW);

			m_manager->create<Container>(0, Rect{ 0, 0, 1, 1 });
			onCreate(*m_manager.get());
			m_manager->createWidgets(m_hwnd);
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
						cont->bounds.width = width;
						cont->bounds.height = height;
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

		void setTitle(const std::wstring& title) {
			SetWindowText(m_hwnd, title.c_str());
		}

	private:
		HWND m_hwnd;
		std::unique_ptr<Manager> m_manager;
	};

}

#endif // GUI_HPP