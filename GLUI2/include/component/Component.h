#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <optional>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "render/Shader.h"
#include "window/Window.h"
#include "debug/macros.h"

namespace TM {

#	define TM_COMP_INIT \
		initVerticesIndices(); \
		initIndicesPos();
		
	struct IndicesPos{
		unsigned int offset;
		int count;
	};

	class Component
	{	
	TM_private:
		bool isDestorying = false;
		std::vector<std::unique_ptr<Component>> children;
		std::priority_queue<int> deletedChildrenIndices;
	
	TM_protected:
		Component* parent;// this ptr should be set null only by its parent
		bool active = true; // render or not
		
	TM_public:
		IndicesPos indicesPos;
		std::shared_ptr<Shader> shader; // if shader == nullptr, it wouldn't be rendered but it's ok.
		Window* window;

	TM_public:
		Component(const std::shared_ptr<Shader>& _shader, Window& _window, std::unique_ptr<Component> _child = nullptr);
		virtual ~Component();

		template<typename T>
		std::enable_if_t<std::is_base_of_v<Component, T>, T*>
		pushChild(std::unique_ptr<T> _child) {
			TM_assertOr(_child != nullptr, "parent to be set is nullptr");
			TM_assertOr(this != _child.get(), "child is itself error");
			TM_assertOr(!this->hasAncestor(_child.get()), "child is already Ancestor of this");
			TM_assertOr(!_child->parent, "child already has parent");
			_child->parent = this;
			this->children.emplace_back(std::move(_child));
			return static_cast<T*>(this->children.back().get());
		}

		bool hasAncestor(Component* const prt);
		const std::vector<std::unique_ptr<Component>>& getChildren();
		void resetWindowTo(Window& _window)					{ window = &_window; }
		Data& getData()										{ return shader->getData(); }
		const IndicesPos& getIndicesPos()					{ return indicesPos; }
		const std::shared_ptr<Shader>& getShader()			{ return shader; }
		void render()				{ window->draw(this); }
		bool isActive()				{ return active; }
		void activate()				{ active = true; }
		void deactivate()			{ active = false; }
		// pos in window
		glm::vec2 getAbsPos() {
			auto vp = getViewport();
			glm::vec2 vpPos = { vp.x, vp.y };
			return getRelPos() + vpPos;
		}

	// virtual
	TM_protected:
		virtual void initVerticesIndices() = 0;
		virtual void initIndicesPos() = 0;

	TM_public:
		virtual unsigned int getGlDrawMode() const = 0;
		virtual const Viewport getViewport()				{ return parent ? parent->getViewport() : Viewport{0,0,window->width, window->height}; }
		virtual glm::vec2 getRelPos() = 0; // pos in viewport
		// specially for text
		virtual glm::vec2 getTextPos()						{ return { 0,0 }; };
		virtual std::optional<std::string_view> getText()	{ return std::nullopt; }
		virtual std::optional<FT_Face> getFace()			{ return std::nullopt; }
		virtual std::optional<glm::vec4> getTextColor()		{ return std::nullopt; }
		virtual int getFontWidth()		{ return 0; }
		virtual int getFontHeight()		{ return 0; }
		//all the update is done after shader->bind() and before shader->draw()
		virtual void update() = 0;
	};

}