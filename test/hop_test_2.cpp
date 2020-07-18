// hop library
//
//  Copyright Tobias Loew 2019. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see https://github.com/tobias-loew/hop
//

#include <iostream>
#include <string>
#include <vector>
#include "..\include\hop.hpp"


#if 0
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
struct tag_default_string;
struct tag_pack_of_strings;

using overloads_t = hop::ol_list <
    hop::ol<int*, hop::cpp_defaulted_param<hop::tagged_ty<tag_default_string, std::string>, init_hallo >, hop::cpp_defaulted_param<double >>
    , hop::ol<long, long>
    , hop::ol<double, double>
    , hop::ol<int, hop::pack<hop::tagged_ty<tag_pack_of_strings, std::string>>>
    , hop::ol<double*, hop::pack<std::string>, long, hop::cpp_defaulted_param<double, init_42 > >		// pack with trailing(!) possible defaulted types
    //, hop::ol<double, float>
    //, hop::ol<double, hop::fwd_if<is_not_double>>
    , hop::ol<double, hop::fwd_if_q<mp11::mp_bind<std::is_same, mp11::_1, float>>>
    //	, hop::ol<char*>
    , hop::tagged_ol<tag_for_double, hop::fwd_if<is_double>>
    , hop::ol<void, std::pair<int, int>>
    , hop::ol<void>
    , hop::ol<>
>;


//template<typename... Ts, decltype((hop::overload_set<overloads_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
void func(Ts&& ... ts) {

    using T = decltype(hop::enable<overloads_t, Ts...>());

    if constexpr (hop::index<T>::value == 0) {
        int i = 42;
        if constexpr (hop::defaults_specified<T>::value == 0) {
            int i = 42;
            auto value0 = hop::get_value_or<T, tag_default_string>("wtf0", std::forward<Ts>(ts)...);
            auto value1 = hop::get_cpp_defaulted_param<T, 1>();
        } else if constexpr (hop::defaults_specified<T>::value == 1) {
            auto tup = hop::get_args_if<T, hop::impl::true_t>(std::forward<Ts>(ts)...);
            auto tup2 = hop::get_tagged_args<T, tag_default_string>(std::forward<Ts>(ts)...);
            auto value0 = hop::get_value_or<T, tag_default_string>("wtf1", std::forward<Ts>(ts)...);
            auto value1 = hop::get_cpp_defaulted_param<T, 1>();
        } else if constexpr (hop::defaults_specified<T>::value == 2) {
            auto value0 = hop::get_value_or<T, tag_default_string>("wtf2", std::forward<Ts>(ts)...);
            int n = 5;
        } else {
            static_assert(hop::dependent_false<T>::value, "invalid"); // ok
            int n = 5;
        }
    } else if constexpr (hop::index<T>::value == 3) {
        auto tup = hop::get_args<T>(std::forward<Ts>(ts)...);
        auto tup2 = hop::get_tagged_args<T, tag_pack_of_strings>(std::forward<Ts>(ts)...);
        auto value0 = hop::get_value_or<T, tag_pack_of_strings>("wtf4", std::forward<Ts>(ts)...);
        int n = 5;
    } else if constexpr (hop::index<T>::value == 5) {
        auto tup = hop::get_args_if<T, hop::impl::true_t>(std::forward<Ts>(ts)...);
        auto value0 = hop::get_value_or<T, tag_default_string>("wtf4", std::forward<Ts>(ts)...);
        int n = 5;
    }
    //else if constexpr (hop::has_tag<T, tag_for_double>::value)
    //{
    //	int n = 5;
    //}

}


//using overloads2_t = hop::ol_list <
//    hop::ol<std::vector<double>>
//>;
//
//
////template<typename... Ts, decltype((hop::overload_set<overloads2_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
//template<typename... Ts, decltype((hop::enable<overloads2_t, Ts...>()), 0) = 0 >
//void func(Ts&& ... ts) {
//
//    using T = decltype(hop::enable<overloads2_t, Ts...>());
//
//    if constexpr (hop::has_tag<T, tag_for_double>::value) {
//        int n = 5;
//    }
//
//}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct tag_ints {};
struct tag_doubles {};
struct tag_a_pack {};

using overloads = hop::ol_list <
    hop::tagged_ol<tag_ints, hop::non_empty_pack<hop::tagged_ty<tag_a_pack, int>>>,
    //	hop::tagged_ol<tag_ints, hop::non_empty_pack<int>>,
    hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
>;

class ccc {
public:




    template<typename Out, typename T>
    void output_as(T&& t) {
        std::cout << (Out)t << std::endl;
    }

    template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using _OL = decltype(hop::enable<overloads, Ts...>());

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
        } else
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

    template<typename... Ts, decltype((hop::enable<ext_overloads, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using _OL = decltype(hop::enable<ext_overloads, Ts...>());

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

int test_main() {
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
    func(&i);
    func(&i, std::string{});
    func(&i, std::string{}, 0);
    func(double{}, double{});
    func(int{}, float{});
    //func(long{}, char{});
    func(int{}, std::string{ "00" });
    func(int{}, std::string{ "10" }, std::string{ "11" });
    func(int{}, std::string{ "20" }, std::string{ "21" }, std::string{ "22" });
    func();
    //func(std::vector<int>{});
    //double d;
    //func(&d, std::string{}, std::string{}, std::string{}, 0);
//	func(&d, std::string{}, std::string{}, std::string{}, 0, 43.1);

    return 0;
}

#else


template<class T>
using map_vector1 = std::vector<T>const&;

template<class T, class Alloc>
using map_vector2 = std::vector<T, Alloc>const&;

template<template<class...> class Pattern>
struct deducer {

    template<template<class...> class Pattern, class _Ty>
    static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<>> test(Pattern<>, _Ty&&);

    template<template<class...> class Pattern, class T, class _Ty>
    static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<T>> test(Pattern<T>, _Ty&&);

    template<template<class...> class Pattern, class T1, class T2, class _Ty>
    static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<T1, T2>> test(Pattern<T1, T2>, _Ty&&);

    //template<class... T, class _Ty>
    //static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<T...>> test(Pattern<T...>, _Ty&&);

    struct no_match;
    static boost::mp11::mp_list<no_match> test(...);

    template<class T>
    using fn = std::is_same<T, boost::mp11::mp_first<decltype(test<Pattern>(std::declval<T>(), std::declval<T>()))>>;

    template<class T>
    using deduced = boost::mp11::mp_second<decltype(test(std::declval<T>(), std::declval<T>()))>;
};



void bar() {
    using T = deducer<map_vector1>::fn<std::vector<int>>;

}
void bar(int) {}
void bar(double) {}
void bar(std::string) {}



template<class F, class... Ts>
void meow(F&& f, Ts&&... t) {

}

template<class... Ts>
void grr(Ts&&... ts) {
    meow(bar(std::forward<Ts>(ts)...), std::forward<Ts>(ts)...);
}
void hmpf() {
   // grr("");
//    std::invoke((void (*)(int))bar, 0.1);
}


struct tag_t1;
struct tag_t2;
struct init_42 {
    double operator()() const { return 42; }
};
struct tag_pack_of_strings;


using overloads_t = hop::ol_list <
    //hop::ol<int*, hop::cpp_defaulted_param<hop::tagged_ty<tag_default_string, std::string>, init_hallo >, hop::cpp_defaulted_param<double >>
    //, hop::ol<long, long>
    //, hop::ol<double, double>
    //, hop::ol<int, hop::pack<hop::tagged_ty<tag_pack_of_strings, std::string>>>
    //, hop::ol<double*, hop::pack<std::string>, long, hop::cpp_defaulted_param<double, init_42 > >		// pack with trailing(!) possible defaulted types
    ////, hop::ol<double, float>
    ////, hop::ol<double, hop::fwd_if<is_not_double>>
    //, hop::ol<double, hop::fwd_if_q<mp11::mp_bind<std::is_same, mp11::_1, float>>>
    ////	, hop::ol<char*>
    //, hop::tagged_ol<tag_for_double, hop::fwd_if<is_double>>
    //, hop::ol<void, std::pair<int, int>>
    //, hop::ol<void>
    //hop::ol<hop::general_defaulted_param<double, init_42>, std::string>// , hop::cpp_defaulted_param<std::string >>
//	hop::ol<hop::general_defaulted_param<hop::tagged_ty<tag_t1, double>, init_42>, std::string>// , hop::cpp_defaulted_param<std::string >>
//,
    //hop::ol<hop::cpp_defaulted_param<hop::tagged_ty<tag_t1, int>>, hop::cpp_defaulted_param<hop::tagged_ty<tag_t2, std::string>>>

    hop::ol<int, hop::pack<hop::tagged_ty<tag_pack_of_strings, std::string>>>
>;

void blub(double const& d, std::string const& s) {
    std::cout << "got a double and a string\n" << d << s << std::endl;

}

//template<typename... Ts, decltype((hop::overload_set<overloads_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
    using _OL = decltype(hop::enable<overloads_t, Ts...>());
    _OL* p = nullptr;
    int i = 42;

    //auto&& args = hop::get_args<_OL>(std::forward<Ts>(ts)...);
    //std::apply(blub, std::forward<decltype(args)>(args));
    //auto value0 = hop::get_value_or<_OL, tag_t1>(43, std::forward<Ts>(ts)...);
    ////auto value1 = hop::get_value_or<_OL, tag_t2>("hallo", std::forward<Ts>(ts)...);

    using T = decltype(hop::enable<overloads_t, Ts...>());
    auto tup = hop::get_args<T>(std::forward<Ts>(ts)...);
    auto tup2 = hop::get_tagged_args<T, tag_pack_of_strings>(std::forward<Ts>(ts)...);
   // auto value0 = hop::get_value_or<T, tag_pack_of_strings >("wtf4", std::forward<Ts>(ts)...);

    int ij = 43;
}

int test_main() {
    int i;

    //foo(44, "sdgsdfg");
    //foo("sdgsdfg");
    foo(int{}, std::string{ "10" }, std::string{ "11" });
    //foo(i, "");
    //foo("");
    //foo(i, i, i);
    //foo(i,i, i, i);
    //foo(i, i);
    //foo(i, i, "dsg");
    //func(&i);
    //func(&i, std::string{});
    //func(&i, std::string{}, 0);
    //func(double{}, double{});
    //func(int{}, float{});
    ////func(long{}, char{});
    //func(int{}, std::string{ "00" });
    //func(int{}, std::string{ "10" }, std::string{ "11" });
    //func(int{}, std::string{ "20" }, std::string{ "21" }, std::string{ "22" });
    //func();
    //func(std::vector<int>{});
    //double d;
    //func(&d, std::string{}, std::string{}, std::string{}, 0);
//	func(&d, std::string{}, std::string{}, std::string{}, 0, 43.1);

    return 0;
}

#endif