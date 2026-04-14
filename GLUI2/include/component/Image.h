#pragma once
#include "component/Component.h"
#include <string_view>
namespace TM {

	class Image: TM_public Component
	{
	TM_protected:
		unsigned int texture;
		float width;
		float height;
		glm::vec2 position;

	TM_public:
		Image(
			Window& _window, int x, int y, std::string_view u8Path, int _width = DEFAULT_WIDTH, int _height = DEFAULT_HEIGHT,
			std::unique_ptr<Component> _child = nullptr, const std::shared_ptr<Shader>& _shader = nullptr, bool loadData = true
		);
		virtual ~Image();

		void setWidth(int w) { width = static_cast<float>(w); }
		void setHeight(int h) { height = static_cast<float>(h); }

		// static 
	TM_public:
		static std::shared_ptr<Shader> defaultShader(bool destruct = false);
		static void staticInit();

		// override
	TM_protected:
		void initVerticesIndices() override;
		void initIndicesPos() override;

	TM_public:
		unsigned int getGlDrawMode() const override { return GL_TRIANGLES; }
		glm::vec2 getRelPos() override { return position; }
		void update() override;
		int getTexture() const { return texture; }
	};

}