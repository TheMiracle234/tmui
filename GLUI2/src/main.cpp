//import tmui;
//import glm;

#include "window/Window.h"
#include "component/Rectangle.h"
#include "component/Triangle.h"
#include "component/Text.h"
#include "component/Button.h"
#include "component/Container.h"

#include <memory>
#include <windows.h>

int main() {
    using glm::vec2;
    using BtnState = TM::Button::State;

    constexpr int WIDTH = 1000;
    constexpr int HEIGHT = 800;

    TM::Window window(WIDTH, HEIGHT, "glwindow");

    auto rect1 = std::make_unique<TM::Rectangle>(0, 0, 500, 400, window, 0xFF0000FF);
    auto rect2 = std::make_unique<TM::Rectangle>(100, 20, 20, 300, window, 0x00FF00FF);

    auto tri1 = std::make_unique<TM::Triangle>(
        vec2(0, 300), vec2(400, 100), vec2(800, 200),
        window, 0xFF00FFFF
    );
    auto tri2 = std::make_unique<TM::Triangle>(
        vec2(500, 300), vec2(900, 100), vec2(900, 200),
        window, 0x00A0FF77
    );

    constexpr int gap = 20;
    auto container1 = std::make_unique<TM::Container>(window, gap, gap, WIDTH / 2 - 2 * gap, HEIGHT - gap * 2);
    auto container2 = std::make_unique<TM::Container>(window, WIDTH / 2, gap, WIDTH / 2 - 2 * gap, HEIGHT / 2 - 2 * gap);
    auto container3 = std::make_unique<TM::Container>(window, WIDTH / 2, HEIGHT / 2, WIDTH / 2 - 2 * gap, HEIGHT / 2 - gap);

    auto rect3 = std::make_unique<TM::Rectangle>(10, 20,  20,  30,  window, 0x0000FFFF);
    auto rect4 = std::make_unique<TM::Rectangle>(10, 70,  20,  30,  window, 0xFF00FFFF);
    auto rect5 = std::make_unique<TM::Rectangle>(10, 120, 25,  30,  window, 0xFFFFFFFF);
	auto rect6 = std::make_unique<TM::Rectangle>(10, 165, 600, 400, window, 0x00FFAA33);

	container1->pushChild(std::move(rect3));
    container1->pushChild(std::move(rect4));
    container1->pushChild(std::move(rect5));
    container1->pushChild(std::move(rect6));

    auto fpsStr = std::make_unique<TM::Text>(window, 50, 50,  50,  u8"", 0x00AAFFFF, 0x00000077);
    auto text2 =  std::make_unique<TM::Text>(window, 50, 150, 150, u8"teŒƒ◊÷2st÷– Œƒ te ≤‚ ‘xt", 0xAA00FF77, 0x00000077);
    auto text3 =  std::make_unique<TM::Text>(window, 70, 300, 75,  u8"≤‚ ‘", 0x32AF8988);
    auto text4 =  std::make_unique<TM::Text>(window, 37, 397, 75,  u8"≤‚ ‘", 0x78AF89DD);
	auto loc =    std::make_unique<TM::Text>(window, 37, 397, 75,  u8"", 0x78AF89DD);

    auto button1 = std::make_unique<TM::Button>(window, 250, 350, TM::DEFAULT_WIDTH, 100, u8"but∞¥≈•ton1");
    auto button2 = std::make_unique<TM::Button>(window, 250, 550, 400, 100, u8"but∞¥≈•ton2");
	auto button3 = std::make_unique<TM::Button>(window, 10,  20,  600, 200, u8"in a container");

    window.bind();
    float deltaTime = 0;
    while (!window.closed()) {
        window.clear();

        deltaTime += window.deltaTime;
        if (deltaTime > 0.5f) {
            fpsStr->reset("FPS: " + window.getFPS());
            deltaTime = 0;
        }

		loc->reset("mouse: " + std::to_string(TM::Event::mouseMsg.x) + ", " + std::to_string(TM::Event::mouseMsg.y));

        rect1->render(); // <=> window.draw(rect1.get());
        tri1->render();
        rect2->render();
        text2->render();
        tri2->render();
        button1->render();
        button2->render();
        text3->render();
        text4->render();
		container1->render();
        fpsStr->render();
		container2->render();
		container3->render();
		loc->render();

        switch (button1->state) {
        case BtnState::DOWN:
            text3->reset("button1 down");
            break;
        case BtnState::ON:
            text3->reset("button1 on");
            break;
        }

        switch (button2->state) {
        case BtnState::DOWN:
            text3->reset("button2 down");
            break;
        case BtnState::ON:
            text3->reset("button2 on");
            break;
        }

        if (button2->state == BtnState::NONE && button1->state == BtnState::NONE) {
            text3->reset("none");
        }

        if (button1->pressed()) {
            text4->reset("button1 pressed");
        }else if (button2->pressed()) {
            text4->reset("button2 pressed");
        }

        window.swapBuffers();
    }

    return 0;
}