#include "window/Window.h"
#include "component/Rectangle.h"
#include "component/Text.h"
#include "component/Button.h"
#include "component/Container.h"
#include "component/Triangle.h"

#include <string>
#include <memory>

//import tmui;
//import glm;

int main() {
    using glm::vec2;

    constexpr int WIDTH = 1000;
    constexpr int HEIGHT = 800;

    TM::Window window(WIDTH, HEIGHT, "UI Layout Example");
    window.bind();

    constexpr int gap = 20; // 间距

    // 创建容器
    auto container1 = std::make_unique<TM::Container>(window, gap, gap, WIDTH / 2 - 2 * gap, HEIGHT / 2 - 2 * gap);
    auto container2 = std::make_unique<TM::Container>(window, WIDTH / 2 + gap, gap, WIDTH / 2 - 2 * gap, HEIGHT / 2 - 2 * gap);
    auto container3 = std::make_unique<TM::Container>(window, gap, HEIGHT / 2 + gap, WIDTH - 2 * gap, HEIGHT / 2 - 2 * gap);

    // 第一个容器：包含矩形和按钮
    container1->setPriority(7);
    auto rect1 = std::make_unique<TM::Rectangle>(0, 0, 500, 200, window, 0xFF0000FF);
    auto rect2 = std::make_unique<TM::Rectangle>(50, 220, 400, 100, window, 0xAAFF0066);
    auto button2 = std::make_unique<TM::Button>(window, 260, 220, 200, 80, (char*)u8"按钮2");
    // cpp 17 直接u8"",但cpp20及以上需要static_cast<char*>(u8"") or (char*)u8""

    auto r1 = container1->pushChild(std::move(rect1));
    container1->pushChild(std::move(rect2));
    auto getBtn1 = container1->pushChild(std::make_unique<TM::Button>(window, 50, 225, 200, 80, (char*)u8"按钮1", TM::DEFAULT_FONT_PATH, 0xFF00FF77));
    container1->pushChild(std::move(button2));

    // 第二个容器：包含文本组件
    auto text1 = std::make_unique<TM::Text>(window, 10, 10, 150, (char*)u8"Hello, 世界!", 0x000000FF, 0xFFFF00FF);
    auto text2 = std::make_unique<TM::Text>(window, 10, 165, 40, (char*)u8"文本t测es试t", 0x000000FF, 0xFF00FF77);

    container2->pushChild(std::move(text1));
    container2->pushChild(std::move(text2));

    // 第三个容器：包含矩形和按钮
    auto rect3 = std::make_unique<TM::Rectangle>(10, 10, 300, 100, window, 0xFFFF0033);
    auto button3 = std::make_unique<TM::Button>(window, 10, 120, 300, 100, (char*)u8"按钮3");
    button3->setColorOf(TM::colorOf(0xFF0000FF), TM::BtnState::ON, TM::BtnLayer::FRONT);
    button3->setColorOf(TM::colorOf(0x00FF00FF), TM::BtnState::DOWN, TM::BtnLayer::FRONT);
    auto lBtn = std::make_unique<TM::LiteBtn>(window, 400, 10, TM::DEFAULT_WIDTH, 100, (char*)u8"轻量按钮");

    container3->pushChild(std::move(rect3));
    container3->pushChild(std::move(button3));
    auto l_btn = container3->pushChild(std::move(lBtn));

    // FPS文本
    auto fpsStr = std::make_unique<TM::Text>(window, 10, 50, 60, (char*)u8"FPS: 0", 0x004400AA);
	auto mousePos = std::make_unique<TM::Text>(window, 10, 110, 40, (char*)u8"鼠标: (0, 0)", 0x224477AA);

    auto tri1 = std::make_unique<TM::Triangle>(
        vec2(0, 300), vec2(400, 100), vec2(800, 200),
        window, 0xFF00FF77
    );

    std::vector<TM::Component*> btns;
    tri1->setPriority(1);

    float deltaTime = 0;
    char flag = 1;
    while (!window.closed<true>()) {
        deltaTime += window.deltaTime;
        if (deltaTime > 0.5f) {
            fpsStr->reset("FPS: " + window.getFPS());
            deltaTime = 0;
        }

        mousePos->reset((char*)u8"鼠标: (" + std::to_string(TM::Event::mouseMsg.x) + ", " + std::to_string(TM::Event::mouseMsg.y) + ")");

        if (l_btn->pressed()) {
            TM_println("press");
            flag ^= 1;
            if (flag) {
                tri1->activate();
                container1.reset();
            }
            else {
                tri1->deactivate();
            }
        }
    }

    return 0;
}