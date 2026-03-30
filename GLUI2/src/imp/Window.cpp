#include "window/Window.h"
#include "debug/debug.h"
#include "component/Component.h"
#include "render/Buffer.h"
#include "render/Shader.h"
#include "component/Event.h"

#include <queue>
#include <iostream>

namespace TM {

	int Window::countWindows = 0;

	void Window::setSizeCallback(GLFWwindow* window, int w, int h) {
		auto tm_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		tm_window->width = w;
		tm_window->height = h;
	}

	void Window::refreshCallback(GLFWwindow* window) {
		auto tm_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		tm_window->_clearWithOutCompClear();
		for (auto& component : tm_window->components) {
			tm_window->_drawWithoutCompClear(component);
		}
		glfwSwapBuffers(tm_window->window.get());
	}

	Window::Window(int w, int h, std::string_view title, uint32_t bk_RGBA):
		window(nullptr, &glfwDestroyWindow), 
		width(w), height(h), lastWidth(w), lastHeight(h), color(colorOf(bk_RGBA)),
		m_lastTimePoint(std::chrono::high_resolution_clock::now())
	{
		TM_println("window construct");
		// glfwinit
		if (countWindows == 0) {
			TM_assertOr(glfwInit(), "GLFW init error");
			TM_println("glfwInit");
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}

		window.reset(glfwCreateWindow(w, h, title.data(), nullptr, nullptr));
		TM_assertOr(window, "GLFW window creation error");

		// glewinit
		if (countWindows == 0) {
			glfwMakeContextCurrent(window.get());
			TM_assertOr(glewInit() == GLEW_OK, "GLEW init error");
			TM_println("glewInit");
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // for font render

			//shader related init
			Shader::staticInit();
		}
		
		// other gl init related to window obj
		glViewport(0, 0, w, h);

		glfwSetWindowUserPointer(window.get(), this);
		glfwSetWindowSizeCallback(window.get(), setSizeCallback);
		glfwSetWindowRefreshCallback(window.get(), refreshCallback);
		glfwSetWindowSizeLimits(window.get(), minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);
		glfwSetKeyCallback(window.get(), Event::keyCallBack);
		glfwSetMouseButtonCallback(window.get(), Event::mouseButtonCallback);
		glfwSetCursorPosCallback(window.get(), Event::cursorPostionCallback);

		countWindows++;
	}

	Window::~Window()
	{
		countWindows--;
		if(countWindows == 0) {
			TM_println("(glew has no destruct for now)\nglfwTerminate");
			glfwTerminate();
		}
		TM_println("window destruction");
	}

	void Window::_clearWithOutCompClear()
	{
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		update();
	}

	void Window::swapBuffers()
	{
		glfwSwapBuffers(window.get()); 
		glfwPollEvents();
		Event::mouseMsg.setOldPos();
	}

	std::string Window::getFPS(int8_t presition)
	{
		const int size = 20;
		char str[size];
		sprintf_s(str, size, "%.*f", presition, 1 / deltaTime);
		return str;
	}

	void Window::_drawWithoutCompClear(Component* const component)
	{
		std::queue<Component*> comps;
		Component* itr;
		comps.push(component);
		
		while (!comps.empty()) {
			itr = comps.front();
			comps.pop();

			// == core part ==
			TM_assertOr(itr != nullptr, "component drawn in the window is nullptr");

			if (itr->isActive()) {
				for (auto& c : itr->children) {
					comps.push(c.get());
				}
			}

			if (!itr->shader) {
				continue;
			}

			Shader* shader = itr->getShader().get();
			if (lastShader != shader) {
				lastShader = shader;
				shader->bind();
			}

			itr->update();

			auto vp = itr->getViewport();
			if (vp) {
				setViewport(vp->x, height - (vp->y + vp->h), vp->w, vp->h);
			}
			shader->draw(itr);
			// == core part ==

		}
	}

	void Window::draw(Component* const component) {
		_drawWithoutCompClear(component);
		components.push_back(component);
	}

	void Window::update()
	{
		lastWidth = width;
		lastHeight = height;

		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - m_lastTimePoint).count();
		m_lastTimePoint = now;
	}

}