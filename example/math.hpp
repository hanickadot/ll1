#ifndef EXAMPLE__MATH_HPP
#define EXAMPLE__MATH_HPP

#include <ll1/ll1.hpp>

namespace math {
	
using namespace LL1;

struct E {static void print() noexcept { printf("E "); }};
struct E2 {static void print() noexcept { printf("E'"); }};
struct T {static void print() noexcept { printf("T "); }};
struct T2 {static void print() noexcept { printf("T'"); }};
struct F {static void print() noexcept { printf("F "); }};




template <typename, typename> struct empty_table;

template <typename, typename> struct table;

// E
template <> struct table<E, LL1::term<'('>> { using next = push<T, E2>; };
template <> struct table<E, LL1::term<'n'>> { using next = push<T, E2>; };

// E2
template <> struct table<E2, LL1::term<')'>> { using next = epsilon; };
template <> struct table<E2, LL1::term<'+'>> { using next = push<term<'+'>, T, E2>; };
template <> struct table<E2, LL1::term<'-'>> { using next = push<term<'-'>, T, E2>; };
template <> struct table<E2, epsilon> { using next = epsilon; };

// T
template <> struct table<T, LL1::term<'('>> { using next = push<F, T2>; };
template <> struct table<T, LL1::term<'n'>> { using next = push<F, T2>; };

// T2
template <> struct table<T2, LL1::term<')'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'*'>> { using next = push<term<'*'>, F, T2>; };
template <> struct table<T2, LL1::term<'+'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'-'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'/'>> { using next = push<term<'/'>, F, T2>; };
template <> struct table<T2, epsilon> { using next = epsilon; };

// F
template <> struct table<F, LL1::term<'('>> { using next = push<term<'('>, E, LL1::term<')'>>; };
template <> struct table<F, LL1::term<'n'>> { using next = push<term<'n'>>; };






}


#endif
