#pragma once

#include "component/Rectangle.h"

namespace TM
{
	class Container : TM_public Rectangle {
	TM_public:
		Container(
			Window& window, int x, int y, int width, int height, uint32_t RGBA = 0x4477FF11, 
			std::unique_ptr<Component> child = nullptr, const std::shared_ptr<Shader>& shader = nullptr
		);
		virtual ~Container() = default;

	// override
	TM_public:
		std::optional<Viewport> getViewport() { return Viewport{ (int)position.x, (int)position.y, (int)width, (int)height }; }

	TM_protected:
		void initVerticesIndices() override;
		void initIndicesPos() override;
	};
}