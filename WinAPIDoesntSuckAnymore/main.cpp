#include <iostream>

#include "gui.hpp"
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_COMMAND: {
			if (HIWORD(wParam) > 1) {
				auto id = LOWORD(wParam);
				auto cmd = HIWORD(wParam);

				if (id < 1000) break;

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
		case WM_SHOWWINDOW: {
			gui::Manager* man = (gui::Manager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			HFONT font = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hwnd, WM_SETFONT, WPARAM(font), TRUE);

			for (gui::WID id : man->widgets()) {
				gui::Widget* wid = man->get(id);
				SendMessage(wid->handle, WM_SETFONT, WPARAM(font), TRUE);
			}
		} break;
		case WM_DESTROY: PostQuitMessage(0); break;
		case WM_SIZE: {
			gui::Manager* man = (gui::Manager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			gui::Widget* cont = man->get(1000);
			cont->bounds.width = width;
			//cont->bounds.height = height;
			man->updateWidgets();

			InvalidateRect(hwnd, nullptr, true);
			UpdateWindow(hwnd);
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

int main(int argc, char** argv) {
	WNDCLASS wc = {};
	wc.lpszClassName = L"WIN_API_DOESNT_SUCK_ANYMORE";
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		L"WIN_API_DOESNT_SUCK_ANYMORE",
		L"WinAPI Window",
		WS_OVERLAPPEDWINDOW,
		20, 20, 800, 600,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	std::unique_ptr<gui::Manager> man = std::make_unique<gui::Manager>();
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) man.get());

	gui::WID btn1, txt1;
	gui::Container& panel = man->create<gui::Container>(gui::Rect{ 0, 0, 800, 32 });
	panel.spacing = 2;
	{
		gui::TextBox& text = man->create<gui::TextBox>();
		text.bounds.width = 120;
		text.bounds.height = 25;
		text.parent = panel.id();
		text.alignment = gui::Alignment::Left;
		panel.children.push_back(text.id());
		txt1 = text.id();
	}

	{
		gui::Button& btn = man->create<gui::Button>(L"Press Me!");
		btn.bounds.width = 120;
		btn.bounds.height = 25;
		btn.parent = panel.id();
		btn.flex = 0;
		btn.onPressed = [&]() {
			auto text = (gui::TextBox*)man->get(txt1);
			MessageBox(nullptr, text->text.c_str(), L"Message", MB_OK);
		};
		panel.children.push_back(btn.id());
		btn1 = btn.id();
	}

	{
		gui::Button& btn = man->create<gui::Button>(L"Press Me Too!");
		btn.bounds.width = 120;
		btn.bounds.height = 25;
		btn.parent = panel.id();
		btn.flex = 0;
		btn.onPressed = [&]() {
			auto but = (gui::Button*) man->get(btn1);
			but->text = L"NO TOUCHY!";
			but->updateAttributes();
		};
		panel.children.push_back(btn.id());
	}

	man->createWidgets(hwnd);

	//FreeConsole();
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};
	while (GetMessage(&msg, hwnd, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hwnd);

	return 0;
}
