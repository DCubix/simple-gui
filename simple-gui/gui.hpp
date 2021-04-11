#ifndef GUI_HPP
#define GUI_HPP

#ifdef _WIN32
#include <Windows.h>
#include <windowsx.h>

#include <gl/GL.h>
#pragma comment (lib, "opengl32.lib")

#include <CommCtrl.h>
#pragma comment(lib, "Comctl32.lib")

#include <Richedit.h>

// thanks MaGetzUb
#pragma comment(linker,"/manifestdependency:\"type='win32' "\
                       "name='Microsoft.Windows.Common-Controls' "\
                       "version='6.0.0.0' "\
                       "processorArchitecture='*' "\
                       "publicKeyToken='6595b64144ccf1df' "\
                       "language='*' "\
                       "\"")
#endif

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <locale.h>
#include <cctype>
#include <regex>
#include <stack>

// thanks MaGetzUb
#include "utils.hpp"

namespace gui {

	class Container;
	class Manager;
	class Widget;

#ifdef _WIN32
	using String = std::basic_string<TCHAR>;
#endif

	enum class Flow {
		Horizontal = 0,
		Vertical
	};

	enum class Alignment {
		Left = 0,
		Center,
		Right
	};

	struct Size { int width, height; };
	struct Rect { int x, y, width, height; };
	struct Font { int size; String family; };
	struct Color { uint8_t r, g, b, a; };

	using WID = size_t;

	constexpr WID BaseWidgetID = 1000;
	
	namespace platform {

		enum class NotificationType {
			SetText = 0,
			SetFont,
			SetTextAlignment,
			SetPositionAndSize,
			SetReadOnly,
			SetMultiline,
			SetPassword
		};

		using Notification = std::variant<String, Font, Alignment, Rect, bool>;

		template <typename HandleType, typename WindowParams>
		class PlatformBase {
		public:
			virtual void notify(WID id, HandleType handle, NotificationType type, Notification notif) = 0;
			virtual HandleType createWindow(int id, Size size, WindowParams params, void* userdata) = 0;

			void notifyCached(const std::function<HandleType(WID)>& getWidgetHandle) {
				for (auto& e : m_cache) {
					HandleType handle = getWidgetHandle(e.id);
					notify(e.id, handle, e.type, e.notif);
				}
			}
		protected:
			struct NotificationCacheEntry {
				WID id;
				NotificationType type;
				Notification notif;
			};

			std::vector<NotificationCacheEntry> m_cache;
		};

#ifdef _WIN32
		struct Win32WindowParams {
			String className{}, text{};
			HWND parent;
			LONG_PTR style;
		};

		static std::unordered_map<String, std::array<LONG_PTR, 3>> AlignmentTranslationMap = {
			{ L"Static", { SS_LEFT, SS_CENTER, SS_RIGHT }},
			{ L"Edit", { ES_LEFT, ES_CENTER, ES_RIGHT }},
			{ L"Button", { BS_LEFT, BS_CENTER, BS_RIGHT }}
		};

		class PlatformWin32 : public PlatformBase<HWND, Win32WindowParams> {
		public:
			void notify(WID id, HWND handle, NotificationType type, Notification notif) {
				if (!handle) {
					m_cache.push_back(NotificationCacheEntry{ id, type, notif });
					return;
				}

				switch (type) {
					case NotificationType::SetText:
						SetWindowText(handle, std::get<String>(notif).c_str());
						break;
					case NotificationType::SetTextAlignment: {
						String buf;
						buf.resize(128);
						GetClassName(handle, buf.data(), buf.size());
						buf.erase(std::find(buf.begin(), buf.end(), '\0'), buf.end());
						
						LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
						auto aligns = AlignmentTranslationMap[buf];

						SetWindowLongPtr(handle, GWL_STYLE, style | aligns[int(std::get<Alignment>(notif))]);
					} break;
					case NotificationType::SetFont: {
						Font fnt = std::get<Font>(notif);
						HFONT font = CreateFont(
							fnt.size, 0, 0, 0, FW_DONTCARE,
							FALSE, FALSE, FALSE,
							ANSI_CHARSET,
							OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY,
							DEFAULT_PITCH | FF_SWISS,
							fnt.family.c_str()
						);
						SendMessage(handle, WM_SETFONT, WPARAM(font), TRUE);
					} break;
					case NotificationType::SetPositionAndSize: {
						Rect bounds = std::get<Rect>(notif);
						SetWindowPos(handle, nullptr, bounds.x, bounds.y, bounds.width, bounds.height, 0);
					} break;
					case NotificationType::SetReadOnly: {
						bool val = std::get<bool>(notif);
						LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
						if (val) style |= ES_READONLY;
						else {
							if ((style & ES_READONLY) > 0)
								style &= ~ES_READONLY;
						}
						SetWindowLongPtr(handle, GWL_STYLE, style);
						SendMessage(handle, EM_SETREADONLY, WPARAM(std::get<bool>(notif)), 0);
					} break;
					case NotificationType::SetMultiline: {
						bool val = std::get<bool>(notif);
						LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
						if (val) style |= ES_MULTILINE;
						else {
							if ((style & ES_MULTILINE) > 0)
								style &= ~ES_MULTILINE;
						}
						SetWindowLongPtr(handle, GWL_STYLE, style);
					} break;
					case NotificationType::SetPassword: {
						bool val = std::get<bool>(notif);
						LONG_PTR style = GetWindowLongPtr(handle, GWL_STYLE);
						if (val) style |= ES_PASSWORD;
						else {
							if ((style & ES_PASSWORD) > 0)
								style &= ~ES_PASSWORD;
						}
						SetWindowLongPtr(handle, GWL_STYLE, style);
					} break;
				}
			}

			HWND createWindow(int id, Size size, Win32WindowParams params, void* userdata) {
				HWND handle = CreateWindow(
					params.className.c_str(),
					params.text.c_str(),
					params.style,
					0, 0, size.width, size.height,
					params.parent,
					(HMENU)id,
					(HINSTANCE)GetWindowLongPtr(params.parent, GWLP_HINSTANCE),
					nullptr
				);
				if (userdata) SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)userdata);
				return handle;
			}
		};

		static PlatformWin32 Platform{};

		using Handle = HWND;
#endif

	};

	using namespace platform;

	class Widget {
		friend class Manager;
	public:
		Widget() = default;
		~Widget() {
			DestroyWindow(handle);
		}

		virtual void create(Handle parent) = 0;

		virtual void update() {
			Platform.notify(m_id, handle, NotificationType::SetPositionAndSize, m_actualBounds);
		}

		Handle handle{ nullptr };

		Size size{};
		int flex{ 1 };

		const WID& id() const { return m_id; }
		const WID& parent() const { return m_parent; }
		const Rect& actualBounds() const { return m_actualBounds; }

		virtual String text() {
			String ret{};
#ifdef _WIN32
			int sz = SendMessage(handle, WM_GETTEXTLENGTH, 0, 0);
			ret.resize(sz);
			SendMessage(handle, WM_GETTEXT, WPARAM(ret.size()+1), LPARAM(ret.data()));
#endif
			return ret;
		}

		virtual void text(const String& tx) {
			Platform.notify(m_id, handle, NotificationType::SetText, tx);
		}

		const Alignment& textAlignment() const { return m_textAlignment; }
		void textAlignment(const Alignment& tx) {
			m_textAlignment = tx;
			Platform.notify(m_id, handle, NotificationType::SetTextAlignment, tx);
		}

		const String& fontFamily() const { return m_fontFamily; }
		void fontFamily(const String& tx) {
			m_fontFamily = tx;
			Platform.notify(m_id, handle, NotificationType::SetFont, Font{ .size = m_fontSize, .family = tx });
		}

		int fontSize() const { return m_fontSize; }
		void fontSize(int tx) {
			m_fontSize = tx;
			Platform.notify(m_id, handle, NotificationType::SetFont, Font{ .size = tx, .family = m_fontFamily });
		}

	protected:
		Rect m_actualBounds;
		WID m_id{ 0 }, m_parent{ 0 };
		std::vector<WID> m_children;

		Alignment m_textAlignment{ Alignment::Center };
		String m_fontFamily{ L"Segoe UI" };
		int m_fontSize{ 16 };
	};

	using WidgetMap = std::unordered_map<size_t, std::unique_ptr<Widget>>;

	class Label : public Widget {
	public:
		Label(const String& tx = L"") { m_tempText = tx; }

		void create(Handle parent) {
#ifdef _WIN32
			Win32WindowParams params = {};
			params.className = L"STATIC";
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD;
			params.text = m_tempText;
#endif

			handle = Platform.createWindow(m_id, size, params, (void*)this);
		}

	private:
		HFONT m_font{ nullptr };
		String m_tempText{};
	};

	class Button : public Widget {
	public:
		Button(const String& tx = L"") { m_tempText = tx; }

		void create(Handle parent) {
#ifdef _WIN32
			Win32WindowParams params = {};
			params.className = L"BUTTON";
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON;
			params.text = m_tempText;
#endif

			handle = Platform.createWindow(m_id, size, params, (void*)this);
		}

		std::function<void()> onPressed;
	private:
		HFONT m_font{ nullptr };
		String m_tempText{};
	};

	class Edit : public Widget {
	public:
		Edit() = default;
		~Edit() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create(Handle parent) {
#ifdef _WIN32
			// Due to a bug in the Windows API, the EDIT control does not update its parent when
			// SetParent is called, so we have to wrap it around a dummy window for it to
			// receive events.
			Win32WindowParams params = {};
			params.className = L"STATIC";
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD;

			m_wrapper = Platform.createWindow(-1, size, params, (void*)this);
			SetWindowSubclass(m_wrapper, Edit::WndProc, 0, 0);
#endif

#ifdef _WIN32
			Win32WindowParams eps = {};
			eps.className = L"EDIT";
			eps.parent = m_wrapper;
			eps.style = WS_VISIBLE | WS_CHILD | WS_BORDER;
#endif
			handle = Platform.createWindow(m_id, size, eps, (void*)this);
		}

#ifdef _WIN32
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

								gui::Widget* widget = (gui::Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
								gui::Edit* tb = static_cast<gui::Edit*>(widget);
								if (id == tb->id()) {
									// Use this for something else...
								}
							} break;
						}
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
#endif

		bool multiLine() const { return m_multiLine; }
		void multiLine(bool v) {
			m_multiLine = v;
			Platform.notify(m_id, handle, NotificationType::SetMultiline, v);
		}

		bool password() const { return m_password; }
		void password(bool v) {
			m_password = v;
			Platform.notify(m_id, handle, NotificationType::SetPassword, v);
		}

		bool readOnly() const { return m_readOnly; }
		void readOnly(bool v) {
			m_readOnly = v;
			Platform.notify(m_id, handle, NotificationType::SetReadOnly, v);
		}

	private:
		bool m_multiLine{ false }, m_password{ false }, m_readOnly{ false };
		HFONT m_font{ nullptr };

#ifdef _WIN32
		HWND m_wrapper;
#endif
	};

	class Spacer : public Widget {
	public:
		Spacer() = default;
		void create(Handle parent) {
#ifdef _WIN32
			Win32WindowParams params = {};
			params.className = L"STATIC";
			params.parent = parent;
			params.style = WS_CHILD;
#endif

			handle = Platform.createWindow(m_id, Size{ 1, 1 }, params, (void*)this);
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
		ListBox() = default;
		~ListBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create(Handle parent) {
#ifdef _WIN32
			// Due to a bug in the Windows API, the EDIT control does not update its parent when
			// SetParent is called, so we have to wrap it around a dummy window for it to
			// receive events.
			Win32WindowParams params = {};
			params.className = L"STATIC";
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD;

			m_wrapper = Platform.createWindow(-1, size, params, (void*)this);
			SetWindowSubclass(m_wrapper, ListBox::WndProc, 0, 0);
#endif

#ifdef _WIN32
			Win32WindowParams eps = {};
			eps.className = L"LISTBOX";
			eps.parent = m_wrapper;
			eps.style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | LBS_NOTIFY;
#endif
			handle = Platform.createWindow(m_id, size, eps, (void*)this);

			for (int i = 0; i < m_items.size(); i++) {
				platformAdd(i);
			}
			select(m_tempSelected);
		}

#ifdef _WIN32
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
							int idx = ListBox_GetCurSel(lbox);
							if (lst->onSelected) lst->onSelected(idx, lst->get(idx));
						} break;
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
#endif

		void update() override {
			Widget::update();
#ifdef _WIN32
			Platform.notify(m_id, m_wrapper, NotificationType::SetPositionAndSize, m_actualBounds);
#endif
		}

		void add(ListItem* item) {
			m_items.push_back(std::unique_ptr<ListItem>(item));
			if (handle) platformAdd(m_items.size() - 1);
		}

		void remove(int idx) {
			m_items.erase(m_items.begin() + idx);
			if (handle) {
#ifdef _WIN32
				ListBox_DeleteString(handle, idx);
#endif
			}
		}

		std::vector<int> selected() {
			std::vector<int> sels;
			if (!handle) return sels;
			
#ifdef _WIN32
			LRESULT len = ListBox_GetSelCount(handle);
			sels.resize(len);

			ListBox_GetSelItems(handle, len, sels.data());
#endif
			return sels;
		}

		const std::vector<std::unique_ptr<ListItem>>& items() {
			return m_items;
		}

		ListItem* get(int id) {
			if (id < 0) return nullptr;
			if (!handle) return m_items[id].get();

			ListItem* ret = nullptr;
#ifdef _WIN32
			int dataID = ListBox_GetItemData(handle, id);
			ret = m_items[dataID].get();
#endif
			return ret;
		}

		void select(int id) {
			if (!handle) m_tempSelected = id;
			else {
#ifdef _WIN32
				ListBox_SetCurSel(handle, id);
#endif
			}
		}

		std::function<void(int, ListItem*)> onSelected;

	private:
		HWND m_wrapper;
		std::vector<std::unique_ptr<ListItem>> m_items;
		int m_tempSelected{ -1 };

		void platformAdd(int item) {
			ListItem* it = m_items[item].get();
#ifdef _WIN32
			int pos = ListBox_AddString(handle, (LPARAM)it->toString().c_str());
			ListBox_SetItemData(handle, pos, LPARAM(item));
#endif
		}

	};

	class ComboBox : public Widget {
	public:
		ComboBox() = default;

		~ComboBox() {
			Widget::~Widget();
			DestroyWindow(m_wrapper);
		}

		void create(Handle parent) {
#ifdef _WIN32
			// Due to a bug in the Windows API, the EDIT control does not update its parent when
			// SetParent is called, so we have to wrap it around a dummy window for it to
			// receive events.
			Win32WindowParams params = {};
			params.className = WC_STATIC;
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD;

			m_wrapper = Platform.createWindow(-1, size, params, (void*)this);
			SetWindowSubclass(m_wrapper, ComboBox::WndProc, 0, 0);
#endif

#ifdef _WIN32
			Win32WindowParams eps = {};
			eps.className = WC_COMBOBOX;
			eps.parent = m_wrapper;
			eps.style = WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_HASSTRINGS | CBS_DROPDOWNLIST;
#endif
			handle = Platform.createWindow(m_id, size, eps, (void*)this);

			for (int i = 0; i < m_items.size(); i++) {
				platformAdd(i);
			}
			select(m_tempSelected);
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
			if (handle) platformAdd(m_items.size() - 1);
		}

		void remove(int idx) {
			m_items.erase(m_items.begin() + idx);
			if (handle) {
#ifdef _WIN32
				ComboBox_DeleteString(handle, idx);
#endif
			}
		}

		int selected() {
			if (!handle) return -1;
			int sel = -1;
#ifdef _WIN32
			sel = ComboBox_GetCurSel(handle);
#endif
			return sel;
		}

		const std::vector<std::unique_ptr<ListItem>>& items() {
			return m_items;
		}

		ListItem* get(int id) {
			if (id < 0) return nullptr;
			if (!handle) return m_items[id].get();

			ListItem* ret = nullptr;
#ifdef _WIN32
			int dataID = ComboBox_GetItemData(handle, id);
			ret = m_items[dataID].get();
#endif
			return ret;
		}

		void select(int id) {
			if (!handle) m_tempSelected = id;
			else {
#ifdef _WIN32
				ComboBox_SetCurSel(handle, id);
#endif
			}
		}

		std::function<void(int, ListItem*)> onSelected;

	private:
		HWND m_wrapper;
		std::vector<std::unique_ptr<ListItem>> m_items;
		int m_tempSelected{ -1 };

		void platformAdd(int item) {
			ListItem* it = m_items[item].get();
#ifdef _WIN32
			int pos = ComboBox_AddString(handle, it->toString().c_str());
			ComboBox_SetItemData(handle, pos, item);
#endif
		}

	};

	enum class UnderlineStyle {
		None = 0,
		Solid,
		DoubleSolid,
		Wavy
	};

	struct RichStyle {
		Color background{ 255, 255, 255, 255 }, foreground{ 0, 0, 0, 255 };
		UnderlineStyle underline{ UnderlineStyle::None };
		bool strikeout{ false }, italic{ false }, bold{ false };
	};

	struct HighlightingRule {
		RichStyle style{};
		String pattern{ L"*" };
	};

	class CodeEdit : public Widget {
	public:
		~CodeEdit() {
			Widget::~Widget();
#ifdef _WIN32
			DestroyWindow(m_wrapper);
#endif
		}

		Color foreground{ 0, 0, 0, 255 }, background{ 255, 255, 255, 255 };

		void create(Handle parent) {
#ifdef _WIN32
			// Due to a bug in the Windows API, the EDIT control does not update its parent when
			// SetParent is called, so we have to wrap it around a dummy window for it to
			// receive events.
			Win32WindowParams wparams = {};
			wparams.className = L"STATIC";
			wparams.parent = parent;
			wparams.style = WS_VISIBLE | WS_CHILD;

			m_wrapper = Platform.createWindow(-1, size, wparams, (void*)this);
			SetWindowSubclass(m_wrapper, CodeEdit::WndProc, 0, 0);
#endif

			// For other platforms maybe use Scintilla?
#ifdef _WIN32
			LoadLibrary(TEXT("Msftedit.dll"));

			Win32WindowParams params = {};
			params.className = MSFTEDIT_CLASS;
			params.parent = m_wrapper;
			params.style =
				ES_MULTILINE | WS_VISIBLE | WS_CHILD |
				WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
#endif

			handle = Platform.createWindow(m_id, size, params, (void*)this);
#ifdef _WIN32
			// Setup a monospaced font ;)
			HFONT font = CreateFont(
				18, 0, 0, 0, FW_DONTCARE,
				FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS,
				TEXT("Consolas")
			);
			SendMessage(handle, WM_SETFONT, WPARAM(font), TRUE);

			SendMessage(handle, EM_SHOWSCROLLBAR, SB_VERT, TRUE);
			SendMessage(handle, EM_SHOWSCROLLBAR, SB_HORZ, TRUE);

			SendMessage(handle, EM_SETEVENTMASK, 0, ENM_CHANGE);
			//SendMessage(handle, EM_SETEVENTMASK, 0, ENM_CLIPFORMAT);
			//SendMessage(handle, EM_SETEVENTMASK, 0, ENM_KEYEVENTS);

			SendMessage(handle, EM_SETBKGNDCOLOR, 0, RGB(background.r, background.g, background.b));
#endif
		}

#ifdef _WIN32
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			//std::cout << GetWMName(uMsg) << std::endl;
			switch (uMsg) {
				/*case WM_NOTIFY: {
					NMHDR* nm = (NMHDR*)lParam;
					if (nm->code == EN_MSGFILTER) {
						MSGFILTER* mf = (MSGFILTER*)lParam;
						if (mf->msg == WM_KEYUP && mf->wParam == VK_RETURN) {
							gui::Widget* widget = (gui::Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
							gui::CodeEdit* tb = static_cast<gui::CodeEdit*>(widget);

							int linesBelow = tb->lineCount() - tb->currentLineIndex();

							SendMessage(hwnd, WM_SETREDRAW, false, 0);
							for (int i = 0; i < linesBelow; i++) {
								tb->highlight(i + tb->currentLineIndex());
							}
							SendMessage(hwnd, WM_SETREDRAW, true, 0);
						}
					}
				} break;*/
				case WM_COMMAND: {
					if (HIWORD(wParam) > 1) {
						auto id = LOWORD(wParam);
						auto cmd = HIWORD(wParam);

						if (id < BaseWidgetID) break;
						switch (cmd) {
							case EN_CHANGE: {
								gui::Widget* widget = (gui::Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
								gui::CodeEdit* tb = static_cast<gui::CodeEdit*>(widget);

								int em = SendMessage(tb->handle, EM_SETEVENTMASK, 0, 0);
								SendMessage(tb->handle, WM_SETREDRAW, false, 0);
								for (int i = 0; i < tb->lineCount(); i++) {
									tb->highlight(i);
								}
								SendMessage(tb->handle, WM_SETREDRAW, true, 0);
								InvalidateRect(tb->handle, nullptr, true);
								SendMessage(tb->handle, EM_SETEVENTMASK, 0, em);
							} break;
						}
					}
				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
#endif

		void highlight(int line = -1) {
			for (auto& hr : highlightingRules) {
				highlightStyle(line, hr.pattern, hr.style);
			}
		}

		void highlightStyle(int line, const String& regex, RichStyle style) {
			if (line >= lineCount()) return;

			line = line < 0 ? currentLineIndex() : line;
			auto [idx, len] = lineMetrics(line);

			String text;
			int offset = 0;
#ifdef _WIN32
			// Get first char index of current line
			offset = SendMessage(handle, EM_LINEINDEX, WPARAM(line), 0);
			size_t sz = SendMessage(handle, EM_LINELENGTH, WPARAM(line), 0);
			sz += offset;

			text.resize(sz);

			Edit_GetLine(handle, line, text.data(), sz);
			
#endif
			if (offset == -1) return;

			auto sel = selection();
			select(offset, text.size());
			resetFormat();

			std::wregex re(regex);
			std::wsmatch sm;
			bool matched = false;
			while (std::regex_search(text, sm, re)) {
				select(sm.position(0) + offset, sm[0].length());
				formatSelection(style);
				offset += sm[0].length();
				text = sm.suffix().str();
			}

			select(std::get<0>(sel), 0);
		}

		std::pair<int, int> currentLineMetrics() {
			return lineMetrics(-1);
		}

		std::pair<int, int> lineMetrics(int line) {
			int idx = 0, len = 0;
#ifdef _WIN32
			int chrPos = SendMessage(handle, EM_LINEINDEX, WPARAM(line), 0);
			idx = SendMessage(handle, EM_LINEFROMCHAR, WPARAM(chrPos), 0);
			len = SendMessage(handle, EM_LINELENGTH, WPARAM(idx), 0);
#endif
			return { idx, len };
		}

		std::pair<int, int> selection() {
			int idx = 0, len = 0;
#ifdef _WIN32
			CHARRANGE rng = {};
			SendMessage(handle, EM_EXGETSEL, 0, LPARAM(&rng));
			idx = rng.cpMin;
			len = rng.cpMax - rng.cpMin;
#endif
			return { idx, len };
		}

		int currentLineIndex() {
			int idx = 0;
#ifdef _WIN32
			idx = SendMessage(handle, EM_LINEFROMCHAR, WPARAM(-1), 0);
#endif
			return idx;
		}

		int lineCount() {
			int cnt = 0;
#ifdef _WIN32
			cnt = SendMessage(handle, EM_GETLINECOUNT, 0, 0);
#endif
			return cnt;
		}

		void select(int index, int length) {
#ifdef _WIN32
			CHARRANGE rng = {};
			rng.cpMin = index;
			rng.cpMax = index + length;
			SendMessage(handle, EM_EXSETSEL, 0, LPARAM(&rng));
#endif
		}

		void selectAll() {
			select(0, -1);
		}

		void deselect() {
			select(0, 0);
		}

		String selected() {
			String ret{};
#ifdef _WIN32
			CHARRANGE rng = {};
			SendMessage(handle, EM_EXGETSEL, 0, LPARAM(&rng));
			if (rng.cpMax - rng.cpMin <= 0) return TEXT("");

			ret.resize(rng.cpMax - rng.cpMin);
			SendMessage(handle, EM_GETSELTEXT, 0, LPARAM(ret.data()));
#endif
			return ret;
		}

		void resetFormat() {
			RichStyle style = {};
			style.background = background;
			style.foreground = foreground;
			style.bold = false;
			style.italic = false;
			style.strikeout = false;
			style.underline = UnderlineStyle::None;
			formatSelection(style);
		}

		void formatSelection(RichStyle style) {
#ifdef _WIN32
			CHARFORMAT2 fmt = {};
			fmt.cbSize = sizeof(CHARFORMAT2);
			fmt.dwMask = CFM_COLOR | CFM_BACKCOLOR;

			fmt.crTextColor = RGB(style.foreground.r, style.foreground.g, style.foreground.b);
			fmt.crBackColor = RGB(style.background.r, style.background.g, style.background.b);

			if (style.strikeout) {
				fmt.dwMask |= CFM_STRIKEOUT;
				fmt.dwEffects |= CFE_STRIKEOUT;
			} else {
				fmt.dwMask |= CFM_STRIKEOUT;
			}

			if (style.italic) {
				fmt.dwMask |= CFM_ITALIC;
				fmt.dwEffects |= CFE_ITALIC;
			} else {
				fmt.dwMask |= CFM_ITALIC;
			}

			if (style.underline != UnderlineStyle::None) {
				fmt.dwMask |= CFM_UNDERLINE;
				fmt.dwEffects |= CFE_UNDERLINE;
			} else {
				fmt.dwMask |= CFM_UNDERLINE;
			}

			if (style.bold) {
				fmt.dwMask |= CFM_BOLD;
				fmt.dwEffects |= CFE_BOLD;
			} else {
				fmt.dwMask |= CFM_BOLD;
			}

			switch (style.underline) {
				default: break;
				case UnderlineStyle::Solid: fmt.bUnderlineType = CFU_UNDERLINE; break;
				case UnderlineStyle::DoubleSolid: fmt.bUnderlineType = CFU_UNDERLINETHICK; break;
				case UnderlineStyle::Wavy: fmt.bUnderlineType = CFU_UNDERLINEWAVE; break;
			}

			SendMessage(handle, EM_SETCHARFORMAT, WPARAM(SCF_SELECTION), LPARAM(&fmt));
#endif
		}

		std::vector<HighlightingRule> highlightingRules;

	private:
#ifdef _WIN32
		HWND m_wrapper;
#endif
	};

#ifdef _WIN32
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

		void create(Handle parent) {
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
				parent,
				(HMENU)m_id,
				(HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
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
#endif

	class Container : public Widget {
	public:
		Container() = default;
		Container(Size b, Flow flow = Flow::Horizontal) { size = b; this->flow = flow; }

		void create(Handle parent) {
#ifdef _WIN32
			Win32WindowParams params = {};
			params.className = L"STATIC";
			params.parent = parent;
			params.style = WS_VISIBLE | WS_CHILD;
#endif

			handle = Platform.createWindow(m_id, size, params, (void*)this);

#ifdef _WIN32
			SetWindowSubclass(handle, Container::WndProc, 0, 0);
#endif
		}

#ifdef _WIN32
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
			switch (uMsg) {
				case WM_COMMAND: {
					auto id = LOWORD(wParam);
					auto hwnd = (HWND)lParam;

					if (id < BaseWidgetID) return DefSubclassProc(hwnd, uMsg, wParam, lParam);
					if (!hwnd) return DefSubclassProc(hwnd, uMsg, wParam, lParam);

					Widget* widget = (Widget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
					// TODO: Make this not hacky
					Button* btn = dynamic_cast<Button*>(widget);
					if (btn && btn->onPressed) {
						btn->onPressed();
					}

				} break;
				case WM_NCDESTROY: RemoveWindowSubclass(hwnd, WndProc, 0); break;
			}
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
		}
#endif

		int border{ 4 }, spacing{ 4 };
		Flow flow{ Flow::Horizontal };
	};

	class Manager {
	public:
		Manager() = default;
		~Manager() = default;

		template <class W, typename... Args>
		W& create(WID parent, Args&&... args) {
			size_t id = m_genID++;
			m_widgets[id] = std::make_unique<W>(std::forward<Args>(args)...);
			m_widgets[id]->m_id = id;

			if (parent) {
				get(parent)->m_children.push_back(id);
				m_widgets[id]->m_parent = parent;
			}

			return *((W*)m_widgets[id].get());
		}

		Widget* get(WID id) {
			return m_widgets[id].get();
		}

		void createWidgets(HWND parentWindow) {
			for (auto& [id, widget] : m_widgets) {
				widget->create(parentWindow);
			}

			// Rearrange parents
			for (auto& [id, widget] : m_widgets) {
				if (widget->parent()) {
					SetParent(widget->handle, get(widget->parent())->handle);
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

			if (cont->m_children.empty()) return;

			for (auto& e : cont->m_children) {
				totalProportions += get(e)->flex;
			}
			totalProportions = totalProportions <= 0 ? 1 : totalProportions;

			Rect parentBounds = cont->m_actualBounds;
			int& compSize = cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height;
			int& comp = cont->flow == Flow::Horizontal ? parentBounds.x : parentBounds.y;

			for (auto& e : cont->m_children) {
				Widget* c = get(e);
				int size = cont->flow == Flow::Horizontal ? c->size.width : c->size.height;
				if (c->flex == 0) {
					compSize -= size;
				}
			}
			const int size = (cont->flow == Flow::Horizontal ? parentBounds.width : parentBounds.height) - cont->border * 2;

			int pbx = parentBounds.x,
				pby = parentBounds.y;
			for (auto& e : cont->m_children) {
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
						if (e != cont->m_children.back()) ret.width -= cont->spacing;
					} break;
					case Flow::Vertical: {
						const int sizeSlice = (size - (c->flex <= 0 ? ret.height : 0)) / totalProportions;
						if (c->flex >= 1) {
							ret.height = sizeSlice * c->flex;
						}
						ret.width = parentBounds.width - cont->border * 2;
						parentBounds.y += ret.height;
						parentBounds.height -= ret.height;
						if (e != cont->m_children.back()) ret.height -= cont->spacing;
					} break;
				}

				c->m_actualBounds = ret;
				performContainerLayout(e);
			}
		}
	};

	namespace markup {
		enum class TokenType {
			Identifier,
			Int,
			Float,
			String,
			LParen,
			RParen,
			LBracket,
			RBracket,
			Comma,
			Equals
		};

		using Value = std::variant<int, float, String>;

		struct Token {
			String lexeme;
			TokenType type;
			Value value;
		};

		class Scanner {
		public:
			Scanner() = default;
			~Scanner() = default;

			Scanner(const String& d) : data(d) {}

			TCHAR peek() {
				if (data.empty()) return '\0';
				return data[0];
			}

			TCHAR scan() {
				if (data.empty()) return '\0';
				TCHAR chr = peek();
				data.erase(data.begin());
				return chr;
			}

			String scanWhile(std::function<bool(TCHAR)> cond) {
				String ret{};
				while (peek() && cond(peek())) {
					ret += scan();
				}
				return ret;
			}

			String data;
		};

		static std::vector<Token> tokenize(const String& str) {
			std::vector<Token> tokens;
			std::unique_ptr<Scanner> sc = std::make_unique<Scanner>(str);

			while (sc->peek()) {
				if (::iswalpha(sc->peek())) { // Identifiers
					auto txt = sc->scanWhile([](TCHAR c) { return ::iswalnum(c); });
					tokens.push_back(Token{ txt, TokenType::Identifier, txt });
				} else if (::iswdigit(sc->peek())) { // Int/Float
					auto txt = sc->scanWhile([](TCHAR c) { return ::iswdigit(c) || c == '.' || c == '-'; });
					if (txt.find_first_of('.') != txt.npos) {
						tokens.push_back(Token{ txt, TokenType::Float, std::stof(txt) });
					} else {
						tokens.push_back(Token{ txt, TokenType::Int, std::stoi(txt) });
					}
				} else if (sc->peek() == '"') { // Strings
					sc->scan(); // eat that " 
					auto txt = sc->scanWhile([](TCHAR c) { return c != '"'; });
					sc->scan(); // eat that " 
					tokens.push_back(Token{ txt, TokenType::String, txt });
				} else if (sc->peek() == '(') {
					sc->scan();
					tokens.push_back(Token{ TEXT("("), TokenType::LParen, 0 });
				} else if (sc->peek() == ')') {
					sc->scan();
					tokens.push_back(Token{ TEXT(")"), TokenType::RParen, 0 });
				} else if (sc->peek() == '{') {
					sc->scan();
					tokens.push_back(Token{ TEXT("{"), TokenType::LBracket, 0 });
				} else if (sc->peek() == '}') {
					sc->scan();
					tokens.push_back(Token{ TEXT("}"), TokenType::RBracket, 0 });
				} else if (sc->peek() == ',') {
					sc->scan();
					tokens.push_back(Token{ TEXT(","), TokenType::Comma, 0 });
				} else if (sc->peek() == '=') {
					sc->scan();
					tokens.push_back(Token{ TEXT("="), TokenType::Equals, 0 });
				} else {
					sc->scan();
				}
			}

			return tokens;
		}

		namespace utils {
			static Alignment parseAlignment(const String& str) {
				if (str == L"left" || str == L"LEFT") return Alignment::Left;
				else if (str == L"center" || str == L"CENTER") return Alignment::Center;
				else if (str == L"right" || str == L"RIGHT") return Alignment::Right;
				return Alignment::Left;
			}

			static Flow parseFlow(const String& str) {
				if (str == L"horizontal" || str == L"HORIZONTAL") return Flow::Horizontal;
				else if (str == L"vertical" || str == L"VERTICAL") return Flow::Vertical;
				return Flow::Horizontal;
			}
		}

		using WidgetPropMap = std::unordered_map<String, Value>;
		using WidgetBuilderMap = std::unordered_map<String, std::function<Widget*(WID, Manager&)>>;
		using WidgetSetupMap = std::unordered_map<String, std::function<void(Widget*, WidgetPropMap)>>;

#define WIDGET_BUILDER(T, name) { name, [](WID p, Manager& man) { auto&& ob = man.create<T>(p); return man.get(ob.id()); } }

		static WidgetBuilderMap WidgetBuilders = {
			WIDGET_BUILDER(Label, L"label"),
			WIDGET_BUILDER(Button, L"button"),
			WIDGET_BUILDER(Edit, L"edit"),
			WIDGET_BUILDER(Spacer, L"spacer"),
			WIDGET_BUILDER(ListBox, L"listbox"),
			WIDGET_BUILDER(ComboBox, L"combobox"),
			WIDGET_BUILDER(Container, L"container")
		};

		static std::function<void(Widget*, WidgetPropMap)> DefaultWidgetSetup = [](Widget* w, WidgetPropMap props) {
			if (props.find(L"flex") != props.end())
				w->flex = std::get<int>(props[L"flex"]);

			if (props.find(L"width") != props.end())
				w->size.width = std::get<int>(props[L"width"]);

			if (props.find(L"height") != props.end())
				w->size.height = std::get<int>(props[L"height"]);

			if (props.find(L"text") != props.end())
				w->text(std::get<String>(props[L"text"]));

			if (props.find(L"alignment") != props.end())
				w->textAlignment(utils::parseAlignment(std::get<String>(props[L"alignment"])));

			if (props.find(L"fontSize") != props.end())
				w->fontSize(std::get<int>(props[L"fontSize"]));

			if (props.find(L"fontFamily") != props.end())
				w->fontFamily(std::get<String>(props[L"fontFamily"]));
		};

		static WidgetSetupMap WidgetSetups = {
			{ L"label", DefaultWidgetSetup },
			{ L"button", DefaultWidgetSetup },
			{ L"edit", DefaultWidgetSetup },
			{ L"spacer", DefaultWidgetSetup },
			{ L"listbox", DefaultWidgetSetup },
			{ L"combobox", DefaultWidgetSetup },
			{ L"container", [](Widget* w, WidgetPropMap props) {
				DefaultWidgetSetup(w, props);

				Container* lbl = static_cast<Container*>(w);
				if (props.find(L"spacing") != props.end())
					lbl->spacing = std::get<int>(props[L"spacing"]);
				
				if (props.find(L"border") != props.end())
					lbl->border = std::get<int>(props[L"border"]);

				if (props.find(L"flow") != props.end())
					lbl->flow = utils::parseFlow(std::get<String>(props[L"flow"]));
			}}
		};

		class Parser {
		public:
			Parser() = default;
			~Parser() = default;

			Parser(const String& input) {
				m_tokens = tokenize(input);
			}

			void parse(Manager& man) { parseWidget(BaseWidgetID, man); }

		private:
			size_t m_position{ 0 };
			std::vector<Token> m_tokens;

			WidgetBuilderMap m_builders = WidgetBuilders;
			WidgetSetupMap m_setups = WidgetSetups;

			const Token& current() {
				return m_tokens[m_position];
			}

			const Token& previous() {
				return m_tokens[m_position-1];
			}

			bool accept(TokenType type) {
				if (current().type == type) {
					m_position++;
					return true;
				}
				return false;
			}

			bool expect(TokenType type) {
				if (!accept(type)) {
					std::cout << "Expected: " << int(type) << std::endl;
					return false;
				}
				return true;
			}

			void parseWidget(WID parent, Manager& man) {
				if (m_position >= m_tokens.size() - 1) return;

				if (accept(TokenType::Identifier)) {
					auto& tok = previous();
					String val = std::get<String>(tok.value);

					if (!expect(TokenType::LParen)) return;

					WidgetPropMap props;
					while (current().type != TokenType::RParen) {
						if (expect(TokenType::Identifier)) {
							auto& id = std::get<String>(previous().value);
							if (expect(TokenType::Equals)) {
								props[id] = current().value;
								m_position++;

								if (accept(TokenType::RParen)) break;
								else if (!expect(TokenType::Comma)) break;
							} else {
								break;
							}
						} else {
							break;
						}
					}
					if (current().type == TokenType::RParen) {
						m_position++;
					}

					if (m_builders.find(val) != m_builders.end()) {
						Widget* wid = m_builders[val](parent, man);
						m_setups[val](wid, props);
						if (accept(TokenType::LBracket)) {
							while (current().type == TokenType::Identifier) {
								parseWidget(wid->id(), man);
							}
							expect(TokenType::RBracket);
						}
					}
				}
			}
		};
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
#ifdef _WIN32
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
#endif
		}

		Container& root() { return *((Container*) m_manager->get(BaseWidgetID)); }

		static void mainLoop() {
#ifdef _WIN32
			MSG msg = {};
			while (GetMessage(&msg, nullptr, 0, 0) > 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
#endif
		}

#ifdef _WIN32
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
						if (dynamic_cast<CodeEdit*>(wid)) continue;

						SendMessage(wid->handle, WM_SETFONT, WPARAM(font), TRUE);
					}

					Platform.notifyCached([&](WID id) {
						return man->get(id)->handle;
					});
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
#endif

		void resize(int width, int height) {
#ifdef _WIN32
			RECT wect;
			GetWindowRect(m_hwnd, &wect);

			SetWindowPos(m_hwnd, nullptr, wect.left, wect.top, width, height, 0);

			RECT nect;
			nect.left = wect.left;
			nect.top = wect.top;
			nect.right = wect.left + width;
			nect.bottom = wect.top + height;

			AdjustWindowRectEx(&nect, WS_OVERLAPPEDWINDOW, 0, 0);
#endif
		}

		void setTitle(const String& title) {
#ifdef _WIN32
			SetWindowText(m_hwnd, title.c_str());
#endif
		}

		void loadMarkup(const String& text) {
			std::unique_ptr<markup::Parser> p = std::make_unique<markup::Parser>(text);
			p->parse(*m_manager.get());
		}

	private:
		Handle m_hwnd;
		std::unique_ptr<Manager> m_manager;
	};

}

#endif // GUI_HPP