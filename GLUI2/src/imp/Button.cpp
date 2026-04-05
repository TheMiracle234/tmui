#include "component/Button.h"
#include "util/util.h"

namespace TM{

	Button::Button(
		Window& window, int x, int y, int w, int h, std::string_view _u8str, std::string_view fontPath, 
		uint32_t colorBtNone, uint32_t colorBtOn, uint32_t colorBtDown, uint32_t colorFtNone, uint32_t colorFtOn, uint32_t colorFtDown
	):
		Text(window, x, y, h, _u8str, colorFtNone, colorBtNone, fontPath, btnDefaultShader(), w)
	{
		setColorOf(TM::colorOf(colorBtOn), State::ON, Layer::BACK);
		setColorOf(TM::colorOf(colorBtDown), State::DOWN, Layer::BACK);
		setColorOf(TM::colorOf(colorFtOn), State::ON, Layer::FRONT);
		setColorOf(TM::colorOf(colorFtDown), State::DOWN, Layer::FRONT);
		setColorOf(TM::colorOf(colorFtNone), State::NONE, Layer::FRONT);
	}

	std::shared_ptr<Shader> Button::btnDefaultShader(bool destruct)
	{
		static std::shared_ptr<Shader> shader = nullptr;
		if (destruct && shader && shader.use_count() == 2) {
			shader.reset();
			return nullptr;
		}
		if (!shader) {
			const Data _data = Data(
				VertexStructure(
					{ 0 },
					{ 2 },
					2 * sizeof(float),
					{ 0 }
				)
			);

			const std::string_view _vsrc = R"(
				#version 330 core
				layout (location = 0) in vec2 aPos;
				out vec2 vPos;
				uniform mat4 projection;
				void main(){
					vPos = aPos;
					gl_Position = projection * vec4(aPos.x, aPos.y, 0, 1.0f);
				}
			)";

			const std::string_view _fsrc = R"(
				#version 330 core
				uniform vec4 uColor;
				uniform vec2 pos1;
				uniform vec2 pos4;
				in vec2 vPos;
				out vec4 FragColor;
				void main(){
					float a = 0.0f;
					const float radius = 24.0f;
					if(vPos.x >= pos1.x + radius && vPos.x <= pos4.x - radius || vPos.y >= pos1.y + radius && vPos.y <= pos4.y - radius){
						a = 1.0f;
					}
					else if(
						distance(vPos, vec2(pos1.x + radius, pos1.y + radius)) < radius || 
						distance(vPos, vec2(pos4.x - radius, pos1.y + radius)) < radius || 
						distance(vPos, vec2(pos1.x + radius, pos4.y - radius)) < radius || 
						distance(vPos, vec2(pos4.x - radius, pos4.y - radius)) < radius
					){
						a = 1.0f;
					}
					FragColor = vec4(uColor.rgb, uColor.a*a);
				}
			)";

			shader = std::make_shared<Shader>(_data, _vsrc, _fsrc, GL_STATIC_DRAW);
		}
		return shader;
	}

	void Button::update() {

		lastState = state;
		
		bool mouse_on = mouseOn();

		if (ok && mouseDown()) { state = State::DOWN; }
		else if(mouse_on) { state = State::ON; }
		else { state = State::NONE; }

		if (Event::mouseMsg.action == Action::PRESS && (!mouse_on || !ok)) {
			ok = false;
		}
		else {
			ok = true;
		}

		auto vp = getViewport();
		glm::mat4 projection = glm::ortho(0.0f, (float)vp.w, (float)vp.h, 0.0f);
		shader->setUniform("uColor", colorOf(state, Layer::BACK));
		shader->setUniform("projection", projection);
		shader->setUniform("pos1", position);
		shader->setUniform("pos4", { position.x + width, position.y + height});
		renewFrontColor(colorOf(state, Layer::FRONT));
	}

	Button::~Button()
	{
		btnDefaultShader(true);
	}

}
