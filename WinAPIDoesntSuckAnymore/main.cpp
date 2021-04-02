#include <iostream>

#include "gui.hpp"
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
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
			cont->bounds.height = height;
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
		L"Calculator That Doesn't Suck",
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
		20, 20, 360, 360,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	std::unique_ptr<gui::Manager> man = std::make_unique<gui::Manager>();
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) man.get());

	gui::Container& panel = man->create<gui::Container>(0, gui::Rect{ 0, 0, 800, 40 }, gui::Flow::Vertical);

	gui::TextBox& display = man->create<gui::TextBox>(panel.id());
	display.bounds.height = 40;
	display.flex = 0;
	display.readOnly = true;
	display.alignment = gui::Alignment::Right;
	display.fontSize = 32;

	wchar_t keypad[] = {
		'7', '8', '9', '+',
		'4', '5', '6', '-',
		'1', '2', '3', L'×',
		'.', '0', '=', L'÷'
	};

	static wchar_t op = '\0';
	static float n1 = 0.0f;
	static float ans = 0.0f;
	static bool solved = false;

	gui::Container& funcRow = man->create<gui::Container>(panel.id(), gui::Rect{ 0, 0, 800, 50 }, gui::Flow::Horizontal);
	funcRow.flex = 0;
	funcRow.border = 0;

	{
		gui::Button& btn = man->create<gui::Button>(funcRow.id(), L"C");
		btn.flex = 1;
		btn.fontSize = 28;
		btn.onPressed = [&]() {
			display.text = L"0";
			display.updateAttributes();
		};
	}

	{
		gui::Button& btn = man->create<gui::Button>(funcRow.id(), L"CE");
		btn.flex = 1;
		btn.fontSize = 28;
		btn.onPressed = [&]() {
			display.text = L"0";
			display.updateAttributes();
			op = '\0';
			n1 = ans = 0.0f;
			solved = false;
		};
	}

	for (int i = 0; i < 4; i++) {
		gui::Container& row = man->create<gui::Container>(panel.id(), gui::Rect{ 0, 0, 800, 50 }, gui::Flow::Horizontal);
		row.flex = 0;
		row.border = 0;
		for (int j = 0; j < 4; j++) {
			wchar_t cop = keypad[j + i * 4];
			gui::Button& numBtn = man->create<gui::Button>(row.id(), std::wstring(1, cop));
			numBtn.fontSize = 28;
			numBtn.onPressed = [&]() {
				wchar_t chr = numBtn.text[0];
				switch (chr) {
					case '+':
					case '-':
					case L'×':
					case L'÷':
						if (op == '\0') n1 = std::stof(display.text);
						else {
							if (!solved) ans = std::stof(display.text);
							else {
								n1 = std::stof(display.text);
								solved = false;
							}
						}

						op = chr;
						display.text = L"0";
						display.updateAttributes();
						break;
					case '.':
						display.text += L".";
						display.updateAttributes();
						break;
					case '=': {
						ans = std::stof(display.text);
						bool divisionByZero = false;
						switch (op) {
							case '+': ans += n1; break;
							case '-': ans -= n1; break;
							case L'×': ans *= n1; break;
							case L'÷': if (ans > 0.0f) { ans = n1 / ans; } else { divisionByZero = true; } break;
						}
						if (!divisionByZero) {
							n1 = ans;
							display.text = std::to_wstring(ans);
							solved = true;
						}  else display.text = L"Division by Zero!";
						display.updateAttributes();
					} break;
					default:
						if (solved) {
							display.text = L"0";
							op = '\0';
							solved = false;
						}
						if (display.text == L"0") {
							display.text = L"";
						}
						display.text += std::wstring(1, chr);
						display.updateAttributes();
						break;
				}
			};
		}
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
