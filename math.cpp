#include <cstdio>

#include <ll1/ll1.hpp>
#include "example/math.hpp"




int main() {
	using namespace LL1::literals;
	
	using grammar = LL1::grammar<math::table, math::E>;
	using parser = grammar::parse<decltype("(n+n)*n"_tape)>;
	
	auto out = parser::value<>;
	
	parser::debug();
}