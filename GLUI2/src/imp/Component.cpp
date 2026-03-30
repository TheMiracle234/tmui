#include "component/Component.h"
#include "render/Shader.h"
#include "debug/debug.h"

#include <iostream>
#include <queue>

#include <GL/glew.h>

namespace TM {

	Component::Component(const std::shared_ptr<Shader>& _shader, Window& _window, std::unique_ptr<Component> _child):
		shader(_shader), window(&_window)
	{
		TM_println("Component construct");
		if(_child){
			pushChild(std::move(_child));
		}
	}

	Component::~Component()
	{
		TM_println("Component destruct");
		if (!parent) {
			return;
		}
		for (auto itr = parent->children.begin(); itr != parent->children.end(); ++itr) {
			if (itr->get() == this) {
				parent->children.erase(itr);
				break;
			}
		}
	}

	void Component::pushChild(std::unique_ptr<Component> _child)
	{
		TM_assertOr(_child != nullptr, "parent to be set is nullptr");
		TM_assertOr(this != _child.get(), "child is itself error");
		TM_assertOr(!this->hasAncestor(_child.get()), "child is already Ancestor of this");
		TM_assertOr(!_child->parent, "child already has parent");

		_child->parent = this;
		this->children.emplace_back(std::move(_child));
	}

	bool Component::hasAncestor(Component* const prt)
	{
		if (!prt || !this->parent) {
			return false;
		}

		Component* p = this;
		while (p = p->parent) {
			if (prt == p) {
				return true;
			}
		}

		return false;
	}

}