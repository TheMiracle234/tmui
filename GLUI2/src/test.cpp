#include "window/Window.h"
#include "component/LoadingBar.h"
#include "component/Rectangle.h"
#include "component/Event.h"

using TM::Window, TM::Axis, TM::Rectangle, TM::Event, TM::LoadingBar;

int main() {
	Window window(800, 600, "SizableBar Test");
	window.bind();
	auto loadingBar = std::make_unique<LoadingBar>(10, 10, 600, 30, window, Axis::x, 0xFF00FFFF);
	auto rect = std::make_unique<Rectangle>(10, 50, 600, 30, window, 0xFF0000FF);
	while (!window.closed()) {
		auto pos = Event::mouseMsg.getPos();
		loadingBar->setCurrAbsPos(pos.x);
	}
	return 0;
}