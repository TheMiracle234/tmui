module;

#include "window/Window.h"
#include "component/Component.h"
#include "component/Rectangle.h"
#include "component/Triangle.h"
#include "component/Container.h"
#include "component/Text.h"
#include "component/Button.h"
#include "component/Event.h"

export module tmui;

export namespace TM {
	export using TM::DEFAULT_FONT_PATH;
	export using TM::DEFAULT_WIDTH;
	using TM::Window;
	using TM::Component;
	using TM::Rectangle;
	using TM::Triangle;
	using TM::Container;
	using TM::Text;
	using TM::Button;
	using TM::Event;
	using TM::Action;
	using TM::KyMod;
	using TM::Key;
	using TM::KeyMsg;
	using TM::MouseMsg;
}