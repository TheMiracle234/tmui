#pragma once

#include <glm/vec4.hpp>

#include <iostream>
#include "debug/debug.h"

namespace TM {

	inline glm::vec4 colorOf(int r, int g, int b, int a) {
		return glm::vec4(
			static_cast<float>(r) / 0xFF,
			static_cast<float>(g) / 0xFF,
			static_cast<float>(b) / 0xFF,
			static_cast<float>(a) / 0xFF
		);
	}

	inline glm::vec4 colorOf(uint32_t RGBA) {
		int r = static_cast<int>((RGBA >> 24) & 0xFF);
		int g = static_cast<int>((RGBA >> 16) & 0xFF);
		int b = static_cast<int>((RGBA >> 8) & 0xFF);
		int a = static_cast<int>(RGBA & 0xFF);
		return colorOf(r, g, b, a);
	}

	template<typename Ptr>
	inline Ptr cp(Ptr ptr) {
#ifndef NDEBUG
		TM_assertOr(ptr, "nullptr error");
#endif
		return ptr;
	}
}