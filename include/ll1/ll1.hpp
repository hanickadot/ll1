#ifndef LL1_HPP
#define LL1_HPP

#include <utility>

namespace LL1 { 

// special symbols:
//  epsilon ... empty input tape or empty stack or action
//  term<S> ... stack representation of symbols from input tape
//  action<A> ... semantic action

struct epsilon {
	static void print() noexcept {
		printf("\033[94m" "ε" "\033[39m");
	}
};

template <auto v> struct term { 
	static constexpr auto value = v;
	static void print() noexcept { 
		printf("\033[91m" "%c" "\033[39m", value);
	}
};

template <typename A> struct action {
	using type = A;
	using function = A;
	static void print() noexcept {
		printf("\033[94m" "<");
		type::print();
		printf(">" "\033[39m");
	}
};

// available parser actions:
//  accept => parser is accepting input
//  reject => parser is rejecting input (also when there is no available path for parsing)
//  pop => pop symbol from top of the stack
//  push<A,B,C> => push symbols or actions to the stack
//  epsilon => push empty string

struct accept { };
struct reject { };
struct pop { };
template <typename... Subject> struct push { };

// is push

template <typename> struct is_push: std::false_type { };
template <typename... Subject> struct is_push<push<Subject...>>: std::true_type { };

template <typename T> static constexpr bool is_push_v = is_push<T>();

// is action

template <typename> struct is_action: std::false_type { };
template <typename A> struct is_action<action<A>>: std::true_type { };

template <typename T> static constexpr bool is_action_v = is_action<T>();

// TODO move me back

template <typename Head, typename... Tail> struct push<Head, Tail...> {
	template <typename Stack> using insert_into = typename Stack::template push_multi<Head, Tail...>;
	using first = Head;
	using rest = push<Tail...>;
	static constexpr bool empty = false;
	static void print() noexcept {
		Head::print();
		if (sizeof...(Tail) > 0) {
			printf(" ");
			push<Tail...>::print();
		}
	}
};

template <> struct push<> {
	template <typename Stack> using insert_into = typename Stack::template push_multi<>;
	static constexpr bool empty = true;
	static void print() noexcept { }
};

// default table:
// 1) epsilon x epsilon => accept
// 2) term x term => pop
// 3) anything x anything => reject

template <typename Symbol, typename Input> struct default_table { using next = reject; };
template <typename Symbol> struct default_table<Symbol, Symbol> { using next = pop; };
template <> struct default_table<epsilon, epsilon> { using next = accept; };

// other table is overlay over default table

template <template <typename, typename> typename T> struct overlay_helper {
	// SFINAE decider for rewrite rule
	template <typename Symbol, typename Input> static constexpr auto next_helper(std::nullptr_t *) -> typename T<Symbol, Input>::next;
	template <typename Symbol, typename Input> static constexpr auto next_helper(...) -> typename default_table<Symbol, Input>::next;
	
	// SFINAE check for rewrite rule
	template <typename Symbol, typename Input> static constexpr auto has_check(std::nullptr_t *) -> decltype(std::declval<typename T<Symbol, Input>::next>(), std::true_type());
	template <typename, typename> static constexpr auto has_check(...) -> std::false_type;
public:
	// public interface for table overlay rewrite rules
	template <typename Symbol, typename Input> static constexpr bool has = decltype(has_check<Symbol, Input>(nullptr))();
	template <typename Symbol, typename Input> using next = decltype(next_helper<Symbol, Input>(nullptr));
};

template <template <typename, typename> typename T, typename Symbol, typename Input> using overlay_table = typename overlay_helper<T>::template next<Symbol, Input>;

template <template <typename, typename> typename T, typename Symbol, typename Input> static constexpr bool has_overlay = overlay_helper<T>::template has<Symbol, Input>;

// parsing stack
// API:
//  bool empty
//  push_one<Item>
//  pop
//  top

template <typename... Content> struct stack;

using empty_stack = stack<>;

// implementation of stack

template <typename... Subject> struct multi_push;

template <> struct stack<> {
	static constexpr bool empty = true;
	using top = epsilon;
	template <typename Item> using push_one = stack<Item>;
	template <typename... Items> using push_multi = typename multi_push<Items...>::template into<stack<>>;
	using pop = stack<>;
	static void print() noexcept {
		printf("\033[94m" "ε" "\033[39m ");
	}
};

template <typename Top> struct stack<Top> {
	static constexpr bool empty = false;
	using top = Top;
	template <typename Item> using push_one = stack<Item, stack<Top>>;
	template <typename... Items> using push_multi = typename multi_push<Items...>::template into<stack<Top>>;
	using pop = stack<>;
	
	static void print() noexcept {
		Top::print();
	}
};

template <typename Top, typename... Rest> struct stack<Top, stack<Rest...>> {
	static constexpr bool empty = false;
	using top = Top;
	template <typename Item> using push_one = stack<Item, stack<Top, stack<Rest...>>>;
	template <typename... Items> using push_multi = typename multi_push<Items...>::template into<stack<Top,stack<Rest...>>>;
	using pop = stack<Rest...>;
	
	static void print() noexcept {
		Top::print();
		if (sizeof...(Rest) > 0) {
			printf(" ");
			stack<Rest...>::print();
		}
	}
};

// push_last_first

template <typename... Subject> struct type_list;

template <typename Head, typename... Tail> struct type_list<Head, Tail...> {
	using last = typename type_list<Tail...>::last;
};

template <typename Head> struct type_list<Head> {
	using last = Head;
};


// multi_push implementation
// for pushing multiple elements in reverse order into stack

template <typename... Subject> struct multi_push: multi_push<type_list<Subject...>> { };

template <typename Head, typename... Tail> struct multi_push<type_list<Head, Tail...>> {
	template <typename OldStack> using into = typename multi_push<type_list<Tail...>>::template into<OldStack>::template push_one<Head>;
};

template <typename Head> struct multi_push<type_list<Head>> {
	template <typename OldStack> using into = typename OldStack::template push_one<Head>;
};

template <> struct multi_push<type_list<>> {
	template <typename OldStack> using into = OldStack;
};

// tape

template <size_t> struct testNum;

template <bool> struct conditional_tape {
	template <auto &, size_t> using at = epsilon;
};

template <> struct conditional_tape<true> {
	template <auto & input, size_t position> using at = term<input[position]>;
};

template <bool Enable, auto & Input, size_t Position> using conditional_tape_t = typename conditional_tape<Enable>::template at<Input,Position>;

template <auto & Input> struct tape {
	static constexpr size_t size = []() constexpr noexcept {
		size_t i{0};
		while (Input[i] != '\0') ++i;
		return i;
	}();
	template <size_t Position> using at = conditional_tape_t<(Position<size), Input, Position>;
	template <size_t Position> using previous_for = conditional_tape_t<((Position<=size) && (Position > 0)), Input, Position-1>;
};




namespace literals {

// emulate P0424R2 with N3599
template <typename CharT, CharT... String> struct tape_helper {
	static inline constexpr CharT string[sizeof...(String)+1] = {String..., '\0'};
};

template <typename CharT, CharT... String> constexpr auto operator""_tape() -> tape<tape_helper<CharT, String...>::string> {
	return {};
};

using hello_tape = decltype("hello"_tape);

static_assert(std::is_same_v<hello_tape::at<0>,term<'h'>>);
static_assert(std::is_same_v<hello_tape::at<4>,term<'o'>>);
static_assert(std::is_same_v<hello_tape::at<5>,epsilon>);
static_assert(std::is_same_v<hello_tape::at<1024>,epsilon>);

}

// modify subject
// output type = new subject

template <typename Action, typename Input, typename Subject> static constexpr auto apply_action() {
	return;
}

// PARSER

// grammar = table + start symbol
// parse output = grammar + input (+ subject)

template <bool Value, typename Subject = void> struct final_state {
	static constexpr bool value = Value;
	using subject = Subject;
	constexpr operator bool() const noexcept {
		return value;
	}
};

template <template <typename, typename> typename Table, typename StartSymbol> struct grammar {


template <typename Symbol, typename Input> using full_table = overlay_table<Table, Symbol, Input>;

// parse function which takes tape and provides
// last<Subject> = last state of parsing (accept/reject) + output subject
// value<Subject> = boolean value, true for accept
// output_subject<Subject> = output subject

template <typename Tape> struct parse_t {

template <size_t Position, typename Stack, typename Subject = void> struct state {
	using previous_symbol = typename Tape::template previous_for<Position>;
	using input_symbol = typename Tape::template at<Position>;
	static constexpr size_t position = Position;
	using stack = Stack;
	
	using subject = Subject;
	
	struct apply {
		using epsilon = state<Position, typename Stack::pop, Subject>;
		using pop = state<Position+1, typename Stack::pop, Subject>;
		template <typename Next> using push = state<Position, typename Next::template insert_into<typename Stack::pop>, Subject>;
		template <typename Next> using qpush = state<Position+1, typename Next::template insert_into<typename Stack::pop>, Subject>;
		template <typename Action> using action = state<Position, typename Stack::pop, decltype(apply_action<Action, previous_symbol, Subject>())>;
	};
};

template <typename State, size_t step = 0> static constexpr auto evaluate() {
	printf("\033[90m" "%02zu> ",step);
	
	if constexpr (std::is_same_v<typename State::input_symbol, epsilon>) {
		printf("     " "\033[39m");
	} else {
		printf("#%02zu: " "\033[39m", State::position);
	}
	
	State::input_symbol::print();
	
	printf("\033[90m" " [ " "\033[39m");
	State::stack::print();
	printf("\033[90m" "] -> " "\033[39m");
	
	if constexpr (is_action_v<typename State::stack::top>) {
		printf("\033[38;5;214m<");
		State::stack::top::type::print();
		puts(">\033[39m");
		return evaluate<typename State::apply::template action<typename State::stack::top::function>, step+1>();
	} else {
		using next = full_table<typename State::stack::top, typename State::input_symbol>;
		
		if constexpr (std::is_same_v<next, accept>) {
			puts("\033[92m" "accept" "\033[39m");
			return final_state<true, typename State::subject>();
		} else if constexpr (std::is_same_v<next, reject>) {
			puts("\033[91m" "reject" "\033[39m");
			return final_state<false, typename State::subject>();
		} else if constexpr (std::is_same_v<next, epsilon>) {
			puts("\033[90m" "epsilon" "\033[39m");
			return evaluate<typename State::apply::epsilon, step+1>();
		} else if constexpr (std::is_same_v<next, pop>) {
			puts("\033[93m" "pop" "\033[39m");
			return evaluate<typename State::apply::pop, step+1>();
		} else if constexpr (is_push_v<next>) {
			// Q-grammar optimization:
			if constexpr (std::is_same_v<typename State::input_symbol, typename next::first>) {
				printf("\033[96m" "qpush" "\033[39m: ");
				next::print();
				printf("\n");
				return evaluate<typename State::apply::template qpush<typename next::rest>, step+1>();
			} else {
				printf("\033[95m" "push" "\033[39m: ");
				next::print();
				printf("\n");
				return evaluate<typename State::apply::template push<next>, step+1>();
			}
		}
		//return std::false_type();
	}
	
	
	
}

template <typename Subject = void> using first = state<0, stack<StartSymbol>, Subject>;

template <typename Subject = void> static auto debug() {
	return evaluate<first<Subject>>();
}

template <typename Subject = void> using last = decltype(evaluate<first<Subject>>());

template <typename Subject = void> static constexpr bool value = last<Subject>();
template <typename Subject = void> using output_subject = typename last<Subject>::subject;

constexpr parse_t() noexcept {
	
}

constexpr operator bool() const noexcept {
	return this->value<void>;
}

};

template <typename Tape> static constexpr parse_t<Tape> parse;

};

}

#endif
