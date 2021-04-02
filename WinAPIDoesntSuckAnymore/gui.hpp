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

#pragma comment(linker,"/manifestdependency:\"type='win32' "\
					   "name='Microsoft.Windows.Common-Controls' "\
					   "version='6.0.0.0' "\
					   "processorArchitecture='x86' "\
					   "publicKeyToken='6595b64144ccf1df' "\
					   "language='*' "\
					   "\"")

namespace gui {

	enum class Flow {
		Horizontal = 0,
		Vertical
	};

	struct Rect { int x, y, width, height; };

	using WID = size_t;
	
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
		Left = SS_LEFT,
		Center = SS_CENTER,
		Right = SS_RIGHT
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
				(HINSTANCE) GetWindowLong(parentHandle, GWL_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)this);
		}

		void updateAttributes() {
			LONG style = GetWindowLongPtr(handle, GWL_STYLE);
			SetWindowLongPtr(handle, GWL_STYLE, style | LONG(alignment));

			SetWindowText(handle, text.c_str());
		}

		Alignment alignment{ Alignment::Center };
		std::wstring text{ L"Text" };
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
				(HINSTANCE)GetWindowLong(parentHandle, GWL_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)this);
		}

		void updateAttributes() {
			LONG style = GetWindowLongPtr(handle, GWL_STYLE);
			SetWindowLongPtr(handle, GWL_STYLE, style | LONG(alignment));

			SetWindowText(handle, text.c_str());
		}

		Alignment alignment{ Alignment::Center };
		std::wstring text{ L"Button" };
		std::function<void()> onPressed;
	};

	class TextBox : public Widget {
	public:
		void create() {
			handle = CreateWindow(
				L"EDIT",
				text.c_str(),
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLong(parentHandle, GWL_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)this);
		}

		void updateAttributes() {
			LONG style = GetWindowLongPtr(handle, GWL_STYLE);
			SetWindowLongPtr(handle, GWL_STYLE, style | LONG(alignment));
			SetWindowText(handle, text.c_str());
		}

		Alignment alignment{ Alignment::Center };
		std::wstring text{ L"" };

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					auto id = LOWORD(wParam);
					auto cmd = HIWORD(wParam);

					if (id < 1000) return DefSubclassProc(hwnd, uMsg, wParam, lParam);
					
					switch (cmd) {
						case EN_CHANGE: {
							Widget* widget = (Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
							TextBox* tb = static_cast<TextBox*>(widget);
							if (id == tb->id()) {
								wchar_t buf[1024];
								GetWindowText(hwnd, buf, 1024);
								tb->text = std::wstring(buf);
							}
						} break;
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
	};

	class Container : public Widget {
	public:
		Container() = default;
		Container(Rect b) {	bounds = b; }

		void create() {
			handle = CreateWindow(
				L"STATIC",
				nullptr,
				WS_VISIBLE | WS_CHILD,
				bounds.x, bounds.y, bounds.width, bounds.height,
				parentHandle,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLong(parentHandle, GWL_HINSTANCE),
				nullptr
			);
			SetWindowLongPtr(handle, GWLP_USERDATA, (LONG)this);
			SetWindowSubclass(handle, Container::WndProc, 0, 0);
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				default: std::cout << uMsg << std::endl; break;
				case WM_COMMAND: {
					auto id = LOWORD(wParam);

					if (id < 1000) return DefSubclassProc(hwnd, uMsg, wParam, lParam);
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
		W& create(Args&&... args) {
			size_t id = m_genID++;
			auto widget = std::make_unique<W>(std::forward<Args>(args)...);
			widget->m_id = id;
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
			performContainerLayout(1000);
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
		size_t m_genID{ 1000 };
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

			for (auto& e : cont->children) {
				Widget* c = get(e);

				Rect ret = c->bounds;
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

}

#endif // GUI_HPP