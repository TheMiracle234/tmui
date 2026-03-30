#include <unordered_map>
#include <glm/vec2.hpp>

#include <iostream>
#include "debug/debug.h"

struct A {
	A() {
		TM_println("A construct");
	}
	~A() {
		TM_println("A desstruct");
	}
};

struct B {
	B() {
		TM_println("B construct");
	}
	~B() {
		TM_println("B desstruct");
	}
};


int main() {
	//TM_println(m[32]);
}