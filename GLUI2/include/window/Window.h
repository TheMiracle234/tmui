#pragma once

#ifndef CPP_VERSION
#	ifdef _MSC_VER
#		define CPP_VERSION _MSVC_LANG
#	else 
#		define CPP_VERSION __cplusplus
#	endif
#endif

#if CPP_VERSION < 201703L
#	error "cpp version should be at least C++17. if you wanna use module, it should be as least C++20"
#endif

#undef CPP_VERSION

/// @notice: all the TM_public obj in class or struct is "read only"
///	all the "gl init part" and something "must init after glinit but has to partly init before glinit" should be init in window constructer only once
///
/// the places where // by ÖÐÎÄ were done by ai
///
///
///
///

#include <memory>
#include <string_view>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <cstdint>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>

#include "util/util.h"
#include "debug/macros.h"

namespace TM {

	class Shader;
	class Component;
	class Window
	{

	TM_private:
		int minWidth = 200;
		int minHeight = 200;
		std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> window;
		std::chrono::steady_clock::time_point m_lastTimePoint;
		glm::vec4 color; // this is stupied but necessary for now. cuz of refreshCallback things
		std::vector<Component*> components; // just for draw loop callback 
		Shader* lastShader = nullptr; // save state

	TM_public:
		int width;
		int height;
		int lastWidth;
		int lastHeight;
		float deltaTime = 1.0f;

	TM_private:
		void update();
		void _clearWithOutCompClear();
		void _drawWithoutCompClear(Component * const component);

	TM_public:
		explicit Window(int w, int h, std::string_view title, uint32_t bk_RGBA = 0x4477FFFF);
		~Window();
		auto& get() { return window; }

		void draw(Component* const component);

		void bind()	{ glfwMakeContextCurrent(window.get());}
		bool closed() { return glfwWindowShouldClose(window.get()); }
		void close() { glfwSetWindowShouldClose(window.get(), true); }
		void setMinWidth(int w) { minWidth = w == 0 ? minWidth : w; glfwSetWindowSizeLimits(window.get(), minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);}
		void setMinHeight(int h) { minHeight = h == 0 ? minHeight : h; glfwSetWindowSizeLimits(window.get(), minWidth, minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);}
		void clear() { setViewport(0, 0, width, height); _clearWithOutCompClear(); components.clear(); }
		void swapBuffers();
		void setBackColor(uint32_t RGBA) { color = colorOf(RGBA); }
		void setBackColor(const glm::vec4& _color) { color = _color; }
		std::string getFPS(int8_t presition = 2);

	//static
	TM_public:
		static int countWindows;

	TM_private:
		static void setViewport(int x, int y, int w, int h) { glViewport(x, y, w, h); }
		static void setSizeCallback(GLFWwindow* window, int w, int h);
		static void refreshCallback(GLFWwindow* window);
	
	TM_public:
		static bool glewiInited() { return countWindows != 0; }
	};
}
