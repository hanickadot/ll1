#include <cstdio>

#include <ll1/ll1.hpp>
#include "example/qmath.hpp"




int main() {
	using namespace LL1::literals;
	
	auto out = LL1::grammar<qmath::table, qmath::E>::parse<decltype("(n+n)*n"_tape)>;
	
	static_assert(sizeof(out) == 1);
	
	out.debug();
	
	static_assert(out == true);	
	
	if (out) {
		puts("input accepted");
	} else {
		puts("input rejected");
	}
	
}