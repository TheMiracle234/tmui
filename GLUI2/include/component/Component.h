#pragma once

#include <vector>
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

	struct Viewport{
		int x, y, w, h;
	};


	class Component
	{
		friend class Window;

	TM_protected:
		Component* parent;// this ptr should be set null only by its parent
		std::vector<std::unique_ptr<Component>> children;
		bool active = true; // render or not
		
	TM_public:
		IndicesPos indicesPos;
		std::shared_ptr<Shader> shader; // if shader == nullptr, it wouldn't be rendered but it's ok.
		Window* window;

	TM_public:
		Component(const std::shared_ptr<Shader>& _shader, Window& _window, std::unique_ptr<Component> _child = nullptr);
		virtual ~Component();

		void resetWindowTo(Window& _window) { window = &_window; }
		void pushChild(std::unique_ptr<Component> _child);
		bool hasAncestor(Component* const prt);
		Data& getData() { return shader->getData(); }
		const IndicesPos& getIndicesPos() { return indicesPos; }
		const std::shared_ptr<Shader>& getShader() { return shader; }
		void render() { window->draw(this); }
		bool isActive() { return active; }
		void activate() { active = true; }
		void deactivate() { active = false; }

	// virtual
	TM_protected:
		virtual void initVerticesIndices() = 0;
		virtual void initIndicesPos() = 0;

	TM_public:
		virtual unsigned int getGlDrawMode() const = 0;
		virtual std::optional<Viewport> getViewport() { return parent ? parent->getViewport() : std::nullopt; }
		virtual glm::vec2 getPos() = 0;
		// specially for text
		virtual glm::vec2 getTextPos() { return { 0,0 }; };
		virtual std::optional<std::string_view> getText() { return std::nullopt; }
		virtual std::optional<FT_Face> getFace() { return std::nullopt; }
		virtual std::optional<glm::vec4> getTextColor() { return std::nullopt; }
		virtual int getFontWidth() { return 0; }
		virtual int getFontHeight() { return 0; }
		//all the update is done after shader->bind() and before shader->draw()
		virtual void update() = 0;
	};

}