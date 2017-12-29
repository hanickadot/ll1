#ifndef EXAMPLE__MATH_HPP
#define EXAMPLE__MATH_HPP

#include <ll1/ll1.hpp>

namespace qmath {
	
using namespace LL1;

struct E {void print() { printf("E "); }};
struct E2 {void print() { printf("E' "); }};
struct T {void print() { printf("T "); }};
struct T2 {void print() { printf("T' "); }};
struct F {void print() { printf("F "); }};

struct value { 
	static void print() { printf("value"); }
};
struct times { static void print() { printf("times"); }};
struct divide { static void print() { printf("divide"); }};
struct plus { static void print() { printf("plus"); }};
struct minus { static void print() { printf("minus"); }};


template <typename, typename> struct empty_table;

template <typename, typename> struct table;

// E
template <> struct table<E, LL1::term<'('>> { using next = push<term<'('>,E,term<')'>,T2,E2>; };
template <> struct table<E, LL1::term<'n'>> { using next = push<term<'n'>, action<value>, T2,E2>; };

// E2
template <> struct table<E2, LL1::term<')'>> { using next = epsilon; };
template <> struct table<E2, LL1::term<'+'>> { using next = push<term<'+'>, T, action<plus>, E2>; };
template <> struct table<E2, LL1::term<'-'>> { using next = push<term<'-'>, T, action<minus>, E2>; };
template <> struct table<E2, epsilon> { using next = epsilon; };

// T
template <> struct table<T, LL1::term<'('>> { using next = push<term<'('>, E, term<')'>, T2>; };
template <> struct table<T, LL1::term<'n'>> { using next = push<term<'n'>, action<value>, T2>; };

// T2
template <> struct table<T2, LL1::term<')'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'*'>> { using next = push<term<'*'>, F, action<times>, T2>; };
template <> struct table<T2, LL1::term<'+'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'-'>> { using next = epsilon; };
template <> struct table<T2, LL1::term<'/'>> { using next = push<term<'/'>, F, action<divide>, T2>; };
template <> struct table<T2, epsilon> { using next = epsilon; };

// F
template <> struct table<F, LL1::term<'('>> { using next = push<term<'('>, E, LL1::term<')'>>; };
template <> struct table<F, LL1::term<'n'>> { using next = push<term<'n'>, action<value>>; };






}


#endif
