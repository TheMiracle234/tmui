#pragma once

#include <glm/vec4.hpp>

#include <iostream>
#include "debug/debug.h"

namespace TM {

	class Flag8 {
		bool _0 : 1;bool _1 : 1;bool _2 : 1;bool _3 : 1;bool _4 : 1;bool _5 : 1;bool _6 : 1;bool _7 : 1;
	public:
		Flag8() :
			_0(0),_1(0),_2(0),_3(0),_4(0),_5(0),_6(0),_7(0) 
		{}
		bool operator[] (int i) const {
			switch (i) {
			case 0: return _0;
			case 1: return _1;
			case 2: return _2;
			case 3: return _3;
			case 4: return _4;
			case 5: return _5;
			case 6: return _6;
			case 7: return _7;
			default: TM_assertOr(false, "flag8 out of index"); return false;
			}
		}
		void set(int i) {
			switch (i) {
			case 0: _0 = true;break;
			case 1: _1 = true;break;
			case 2: _2 = true;break;
			case 3: _3 = true;break;
			case 4: _4 = true;break;
			case 5: _5 = true;break;
			case 6: _6 = true;break;
			case 7: _7 = true;break;
			default: TM_assertOr(false, "flag8 out of index");
			}
		}
		void remove(int i) {
			switch (i) {
			case 0: _0 = false;break;
			case 1: _1 = false;break;
			case 2: _2 = false;break;
			case 3: _3 = false;break;
			case 4: _4 = false;break;
			case 5: _5 = false;break;
			case 6: _6 = false;break;
			case 7: _7 = false;break;
			default: TM_assertOr(false, "flag8 out of index");
			}
		}
	};

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

	inline uint32_t rgbaOf(const glm::vec4& color) {
		uint32_t r = static_cast<uint32_t>(color.r * 0xFF) & 0xFF;
		uint32_t g = static_cast<uint32_t>(color.g * 0xFF) & 0xFF;
		uint32_t b = static_cast<uint32_t>(color.b * 0xFF) & 0xFF;
		uint32_t a = static_cast<uint32_t>(color.a * 0xFF) & 0xFF;
		return (r << 24) | (g << 16) | (b << 8) | a;
	}

	inline uint32_t rgbaOf(const glm::ivec4& color) {
		uint32_t r = static_cast<uint32_t>(color.r) & 0xFF;
		uint32_t g = static_cast<uint32_t>(color.g) & 0xFF;
		uint32_t b = static_cast<uint32_t>(color.b) & 0xFF;
		uint32_t a = static_cast<uint32_t>(color.a) & 0xFF;
		return (r << 24) | (g << 16) | (b << 8) | a;
	}

	template<typename T>
	inline T clamp(T value, T min, T max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	template<typename Ptr>
	inline Ptr cp(Ptr ptr) {
#ifndef NDEBUG
		TM_assertOr(ptr, "nullptr error");
#endif
		return ptr;
	}
}