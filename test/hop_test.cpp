#include <iostream>
#include <string>
#include <vector>
#include "hop.hpp"

template<class T0, class T1, class... Ts>
struct t1_is_arithmetic : std::is_arithmetic<T1> {};


template<class T>
struct is_not_double : std::negation<std::is_same<T, double>> {};

template<class T>
struct is_double : std::is_same<T, double> {};


namespace mp11 = boost::mp11;

struct init_hallo {
	std::string operator()() const { return "hallo"; }
};
struct init_42 {
	double operator()() const { return 42; }
};

struct tag_for_double;

using overloads_t = hop::ol_list <
	  hop::ol<int*, hop::default_value<std::string, init_hallo >, hop::default_value<double >>
	, hop::ol<long, long>
	, hop::ol<double, double>
	, hop::ol<int, hop::pack<std::string>>
	, hop::ol<double*, hop::pack<std::string>, long, hop::default_value<double, init_42 > >		// pack with trailing(!) possible defaulted types
	//, hop::ol<double, float>
	//, hop::ol<double, hop::fwd_if<is_not_double>>
	, hop::ol<double, hop::fwd_if_q<mp11::mp_bind<std::is_same, mp11::_1, float>>>
	, hop::ol<char*>
	, hop::tagged_ol<tag_for_double, hop::fwd_if_not<is_double>>
, hop::ol<void, std::pair<int, int>>
, hop::ol<void>
	, hop::ol<>
>;


//template<typename... Ts, decltype((hop::overload_set<overloads_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
template<typename... Ts, decltype((hop::enabler<overloads_t, Ts...>()), 0) = 0 >
void func(Ts&& ... ts) {

	using T = decltype(hop::enabler<overloads_t, Ts...>());

	if constexpr (hop::index<T>::value == 0) 
	{
		int i = 42;
		if constexpr (hop::defaults_specified<T>::value == 0) {
			int i = 42;
			auto value0 = hop::get_default_value<overloads_t, T, 0>();
			auto value1 = hop::get_default_value<overloads_t, T, 1>();
		}
		else if constexpr (hop::defaults_specified<T>::value == 1) {
			auto value1 = hop::get_default_value<overloads_t, T, 1>();
		}
		else if constexpr (hop::defaults_specified<T>::value == 2) {
		}
		else {
			static_assert(hop::dependent_false<T>::value, "invalid"); // ok
		}
	}
	else if constexpr (hop::index<T>::value == 5)
	{
		int n = 5;
	}
	//else if constexpr (hop::has_tag<T, tag_for_double>::value)
	//{
	//	int n = 5;
	//}

}


using overloads2_t = hop::ol_list <
	hop::ol<std::vector<double>>
>;


//template<typename... Ts, decltype((hop::overload_set<overloads2_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
template<typename... Ts, decltype((hop::enabler<overloads2_t, Ts...>()), 0) = 0 >
void func(Ts&& ... ts) {

	using T = decltype(hop::enabler<overloads2_t, Ts...>());

	if constexpr (hop::has_tag<T, tag_for_double>::value)
	{
		int n = 5;
	}

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct tag_ints {};
struct tag_doubles {};

using overloads = hop::ol_list <
	hop::tagged_ol<tag_ints, hop::non_empty_pack<int>>,
	hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
>;

class ccc {
public:




	template<typename Out, typename T>
	void output_as(T&& t) {
		std::cout << (Out)t << std::endl;
	}

	template<typename... Ts, decltype((hop::enabler<overloads, Ts...>()), 0) = 0 >
	void foo(Ts&& ... ts) {
		using _OL = decltype(hop::enabler<overloads, Ts...>());

		//if constexpr (hop::index<_OL>::value == 0) {
		//	std::cout << "got a bunch of ints\n";
		//	(output_as<int>(ts), ...);
		//	std::cout << std::endl;
		//}
		//else
		//	if constexpr (hop::index<_OL>::value == 1) {
		//		std::cout << "got a bunch of doubles\n";
		//		(output_as<double>(ts), ...);
		//		std::cout << std::endl;
		//	}

		if constexpr (hop::has_tag<_OL, tag_ints>::value) {
			std::cout << "got a bunch of ints\n";
			(output_as<int>(ts), ...);
			std::cout << std::endl;
		}
		else
			if constexpr (hop::has_tag<_OL, tag_doubles>::value) {
				std::cout << "got a bunch of doubles\n";
				(output_as<double>(ts), ...);
				std::cout << std::endl;
			}
	}
};

struct tag_strings {};

class ddd :public ccc {
public:


	using ext_overloads = hop::ol_extend <
		overloads,
		hop::tagged_ol<tag_strings, hop::non_empty_pack<std::string>>
	>;


	template<typename Out, typename T>
	void output_as(T&& t) {
		std::cout << (Out)t << std::endl;
	}

	template<typename... Ts, decltype((hop::enabler<ext_overloads, Ts...>()), 0) = 0 >
	void foo(Ts&& ... ts) {
		using _OL = decltype(hop::enabler<ext_overloads, Ts...>());

		if constexpr (hop::is_from_base_v<_OL>) {
			ccc::foo(std::forward<Ts>(ts)...);
		} else
		if constexpr (hop::has_tag_v<_OL, tag_strings>) {
			std::cout << "got a bunch of strings\n";
			(output_as<std::string>(ts), ...);
			std::cout << std::endl;
		}


		//if constexpr (hop::has_tag<_OL, tag_ints>::value) {
		//	std::cout << "got a bunch of ints\n";
		//	(output_as<int>(ts), ...);
		//	std::cout << std::endl;
		//}
		//else
		//	if constexpr (hop::has_tag<_OL, tag_doubles>::value) {
		//		std::cout << "got a bunch of doubles\n";
		//		(output_as<double>(ts), ...);
		//		std::cout << std::endl;
		//	}
	}
};

int main()
{
	//ccc _ccc;
	//_ccc.foo(42, 17);
	//_ccc.foo(1.5, -0.4, 12.0);
	////foo(42, 0.5);

	ddd _ddd;
	_ddd.foo(42, 17);
	_ddd.foo(1.5, -0.4, 12.0);
	_ddd.foo("hallo", std::string{});

//	func(long{}, long{});
//	func(double{}, double{});
//	func(float{}, float{});
//	func(double{}, float{});
////	func(long{}, float{});  // expected error ambigous
	int i;
//	func(&i);
	func(&i, std::string{});
	func(&i, std::string{}, 0);
	func(double{}, double{});
	func(int{}, float{});
	//func(long{}, char{});
	func(int{}, std::string{});
	func(int{}, std::string{}, std::string{});
	func(int{}, std::string{}, std::string{}, std::string{});
	func();
	func(std::vector<int>{});
	//double d;
	//func(&d, std::string{}, std::string{}, std::string{}, 0);
//	func(&d, std::string{}, std::string{}, std::string{}, 0, 43.1);
}
