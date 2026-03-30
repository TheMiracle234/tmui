#include "component/Container.h"

namespace TM
{
	Container::Container(
		Window& window, int x, int y, int width, int height, uint32_t RGBA, 
		std::unique_ptr<Component> child, const std::shared_ptr<Shader>& shader
	):
		Rectangle(x, y, width, height, window, RGBA, std::move(child), shader, false)
	{
		TM_COMP_INIT;
	}

	void Container::initVerticesIndices() {
		indicesPos.offset = getData().push(
			{
				 1.0f,	 1.0f,
				-1.0f,	 1.0f,
				 1.0f,	-1.0f,
				-1.0f,	-1.0f,
			},
			{
				0, 1, 2,
				1, 2, 3
			}
		);
	}

	void Container::initIndicesPos() {
		indicesPos.count = 6;
	}

}