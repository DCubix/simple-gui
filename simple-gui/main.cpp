#include <iostream>

#include "gui.hpp"

class Calculator : public gui::Window {
public:

	void onCreate(gui::Manager& man) override {
		setTitle(L"Calculator");

		gui::Container& rootPane = root();
		rootPane.flow = gui::Flow::Vertical;

		gui::TextBox& display = man.create<gui::TextBox>(rootPane.id());
		display.size.height = 40;
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

		gui::Container& funcRow = man.create<gui::Container>(rootPane.id(), gui::Size{ 800, 50 }, gui::Flow::Horizontal);
		funcRow.flex = 0;
		funcRow.border = 0;

		{
			gui::Button& btn = man.create<gui::Button>(funcRow.id(), L"C");
			btn.flex = 1;
			btn.fontSize = 28;
			btn.onPressed = [&]() {
				display.text = L"0";
				display.updateAttributes();
			};
		}

		{
			gui::Button& btn = man.create<gui::Button>(funcRow.id(), L"CE");
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
			gui::Container& row = man.create<gui::Container>(rootPane.id(), gui::Size{ 800, 50 }, gui::Flow::Horizontal);
			row.flex = 0;
			row.border = 0;
			for (int j = 0; j < 4; j++) {
				wchar_t cop = keypad[j + i * 4];
				gui::Button& numBtn = man.create<gui::Button>(row.id(), gui::String(1, cop));
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
							} else display.text = L"Division by Zero!";
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
							display.text += gui::String(1, chr);
							display.updateAttributes();
							break;
					}
				};
			}
		}

		man.create<gui::Label>(rootPane.id(), L"Made by Diego");

		resize(300, 355);
	}

	wchar_t op{ '\0' };
	float n1{ 0.0f }, ans{ 0.0f };
	bool solved{ false };
};

class OpenGLTriangle : public gui::GLView {
public:
	void onPaint() override {
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_TRIANGLES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -1.0f, 0.0f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glEnd();
	}
};

class WidgetsTest : public gui::Window {
public:
	void onCreate(gui::Manager& man) override {
		setTitle(L"Widgets");

		gui::Container& rootPane = root();
		rootPane.flow = gui::Flow::Vertical;

		OpenGLTriangle& glv = man.create<OpenGLTriangle>(rootPane.id());

		gui::Container& buttonPane = man.create<gui::Container>(rootPane.id());
		buttonPane.flex = 0;
		buttonPane.size.height = 40;

		gui::Button& btn = man.create<gui::Button>(buttonPane.id(), L"Button");
		btn.size.width = 120;
		btn.flex = 0;
	}
};

class MarkupWindow : public gui::Window {
public:
	void onCreate(gui::Manager& man) override {
		root().flow = gui::Flow::Vertical;

		gui::String ui = LR"(
container(height=40, border=0, flex=0) {
	button(width=120, flex=0, text="Button")
}
)";

		loadMarkup(ui);

		resize(400, 400);
	}
};

int main(int argc, char** argv) {
	//Calculator cal1{};
	//cal1.show();

	//WidgetsTest wt{};
	//wt.show();

	//gui::Window::mainLoop();

	
	MarkupWindow win{};
	win.show();

	gui::Window::mainLoop();
	
	return 0;
}
