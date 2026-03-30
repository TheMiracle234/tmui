//#include "debug/debug.h"
#pragma once

#include <string>
#include <string_view>
#include <cassert>
#include <iostream>

#define TM_println(x) std::cout<<x<<"\n"
#define TM_printv(x) std::cout<<#x<<": "<<x<<"\n"
#define TM_perror(x) std::cerr<<__FILE__<<"\nline "<<__LINE__<<": "<<x<<"\n"
#define TM_assertOr(x, out) if(!(x)) { TM_perror(out); system("pause"); exit(1); }
#define TM_sassert(x) static_assert(x)

namespace TM {

	inline std::string lastError = "no error for now";

#	define TM_setError(str) mySetError(str, __FILE__, __LINE__) 

	static inline void mySetError(std::string_view str, std::string_view file, int line) {
		lastError = std::string(file) + "\nline " + std::to_string(line) + ": " + std::string(str) + "\n";

#		ifdef TM_DEBUG
			TM_perror(lastError);
#		endif
	}
}

int main() {

	return 0;
}