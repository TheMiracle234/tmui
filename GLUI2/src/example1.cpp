#include "window/Window.h"
#include "component/Rectangle.h"
#include "component/Text.h"
#include "component/Button.h"
#include "component/Container.h"
#include "component/Triangle.h"

#include <memory>
#include <windows.h>

int main() {
    using glm::vec2;
    using BtnState = TM::Button::State;

    constexpr int WIDTH = 1000;
    constexpr int HEIGHT = 800;

    TM::Window window(WIDTH, HEIGHT, "UI Layout Example");

    constexpr int gap = 20; // 间距

    auto tri1 = std::make_unique<TM::Triangle>(
        vec2(0, 300), vec2(400, 100), vec2(800, 200),
        window, 0xFF00FF77
    );

    // 创建容器
    auto container1 = std::make_unique<TM::Container>(window, gap, gap, WIDTH / 2 - 2 * gap, HEIGHT / 2 - 2 * gap);
    auto container2 = std::make_unique<TM::Container>(window, WIDTH / 2 + gap, gap, WIDTH / 2 - 2 * gap, HEIGHT / 2 - 2 * gap);
    auto container3 = std::make_unique<TM::Container>(window, gap, HEIGHT / 2 + gap, WIDTH - 2 * gap, HEIGHT / 2 - 2 * gap);

    // 第一个容器：包含矩形和按钮
    auto rect1 = std::make_unique<TM::Rectangle>(0, 0, 500, 200, window, 0xFF0000FF);
    auto rect2 = std::make_unique<TM::Rectangle>(50, 220, 400, 100, window, 0xAAFF0066);
    auto button2 = std::make_unique<TM::Button>(window, 260, 220, 200, 80, u8"按钮2");

    container1->pushChild(std::move(rect1));
    container1->pushChild(std::move(rect2));
    auto getBtn1 = container1->pushChild(std::make_unique<TM::Button>(window, 50, 225, 200, 80, u8"按钮1", TM::DEFAULT_FONT_PATH, 0xFF00FF77));
    container1->pushChild(std::move(button2));

    // 第二个容器：包含文本组件
    auto text1 = std::make_unique<TM::Text>(window, 10, 10, 150, u8"Hello, 世界!", 0x000000FF, 0xFFFF00FF);
    auto text2 = std::make_unique<TM::Text>(window, 10, 165, 40, u8"文本t测es试t", 0x000000FF, 0xFF00FF77);

    container2->pushChild(std::move(text1));
    container2->pushChild(std::move(text2));

    // 第三个容器：包含矩形和按钮
    auto rect3 = std::make_unique<TM::Rectangle>(10, 10, 300, 100, window, 0xFFFF0033);
    auto button3 = std::make_unique<TM::Button>(window, 10, 120, 300, 100, u8"按钮3");

    container3->pushChild(std::move(rect3));
    container3->pushChild(std::move(button3));

    // FPS文本
    auto fpsStr = std::make_unique<TM::Text>(window, 10, 50, 60, u8"FPS: 0", 0x004400AA);
	auto mousePos = std::make_unique<TM::Text>(window, 10, 110, 40, u8"鼠标: (0, 0)", 0x224477AA);

    window.bind();
    float deltaTime = 0;
    char flag = 1;
    while (!window.closed()) {
        window.clear();

        deltaTime += window.deltaTime;
        if (deltaTime > 0.5f) {
            fpsStr->reset("FPS: " + window.getFPS());
            deltaTime = 0;
        }

        mousePos->reset(u8"鼠标: (" + std::to_string(TM::Event::mouseMsg.x) + ", " + std::to_string(TM::Event::mouseMsg.y) + ")");

        // 渲染各个容器和它们的子组件
        container1->render();
        container2->render();
        container3->render();
        fpsStr->render(); // 显示 FPS
		mousePos->render(); // 显示鼠标位置
        if (flag) {
            tri1->render();
        }

        if (getBtn1->pressed()) {
            flag ^= 1;
        }

        window.swapBuffers();
    }

    return 0;
}