#pragma once

#include "component/Component.h"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace TM {

	class Rectangle : TM_public Component
	{
	TM_protected:
		float width;
		float height;

		const glm::vec2 position;
		glm::vec4 rectColor;

	TM_public:
		Rectangle(
			int x, int y, int _width, int _height, Window& _window, 
			uint32_t RGBA = 0x4477FFFF, std::unique_ptr<Component> _child = nullptr, const std::shared_ptr<Shader>& _shader = nullptr, bool loadData = true
		);
		virtual ~Rectangle();

		void setWidth(int w)	{ width = static_cast<float>(w); }
		void setHeight(int h)	{ height = static_cast<float>(h); }

	// static 
	TM_public:
		static std::shared_ptr<Shader> defaultShader(bool destruct = false);

	// override
	TM_protected:
		void initVerticesIndices() override;
		void initIndicesPos() override;

	TM_public:
		unsigned int getGlDrawMode() const override { return GL_TRIANGLES; }
		glm::vec2 getPos() override { return position; }
		void update() override;
	};

}