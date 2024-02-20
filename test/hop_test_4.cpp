#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <boost/assert.hpp>
#include <boost/config.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/detail/math.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt> 
#include <../include/hop.hpp>
#include <../include/hop_utils.hpp>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <type_traits>

auto&& os = std::ofstream("hop_out.txt");

template<class T>
struct tag_t {
    using type = T;
    constexpr tag_t() {}
};


std::string ws_to_s(std::wstring const& ws) {
    // use very simple ws to s converter (it's deprecated, but works for our purposes)
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(ws);
}

enum limiters {
    bracket,
    paren,
    brace,
    angle
};

static constexpr char const* limit_symbol[][2]{
    {"[","]"},
    {"(",")"},
    {"{","}"},
    {"<",">"},
};

template<limiters limit, class T>
std::string to_string(T&&);

template<limiters limit, class T>
std::string tuple_to_string(T&&);

template<class T>
decltype(auto) printer(T) {
    using type = std::remove_cv_t<std::remove_reference_t<typename T::type>>;
    if constexpr (std::is_same<type, std::wstring>::value) {
        // use very simple ws to s converter (it's deprecated, but works for our purposes)
        return [](type const& s) {return ws_to_s(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::vector<int>>::value) {
        return [](type const& s) { return to_string<limiters::bracket>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::array<int,1>>::value) {
        return [](type const& s) { return to_string<limiters::bracket>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::pair<int, int>>::value) {
        return [](type const& s) { return tuple_to_string<limiters::bracket>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::tuple<>>::value) {
        return [](type const& s) { return tuple_to_string<limiters::bracket>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::list<int>>::value) {
        return [](type const& s) { return to_string<limiters::paren>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::set<int>>::value) {
        return [](type const& s) { return to_string<limiters::angle>(s); };
    } else if constexpr (boost::mp11::mp_similar<type, std::map<int, int>>::value) {
        return [](type const& s) { return to_string<limiters::brace>(s); };
    } else {
        return [](auto&& t)->decltype(auto) {
            return std::forward<decltype(t)>(t);
        };
    }
}

template<class T>
decltype(auto) make_printable(T&& arg) {
    return printer(tag_t<T>{})(std::forward<T>(arg));
}

template<limiters limit, class T>
std::string to_string(T&& cont) {
    std::string result = limit_symbol[limit][0];
    bool first = true;
    for (auto&& elem : cont) {
        if (first) {
            first = false;
        } else {
            result += ",";
        }
        std::stringstream ss;
        ss << make_printable(elem);
        result += ss.str();
    }
    result += limit_symbol[limit][1];
    return result;
}


template<limiters limit, class T>
std::string tuple_to_string(T&& t) {
    std::string result = limit_symbol[limit][0];
    bool first = true;

    std::apply([&](auto&&... args) {((

        [&]() {
            if (first) {
                first = false;
            } else {
                result += ",";
            }
            std::stringstream ss;
            ss << make_printable(args);
            result += ss.str();
        }()

            ), ...); }, std::forward<T>(t));

    result += limit_symbol[limit][1];
    return result;
}



struct output_args_ {



    template<class... Args>
    void operator()(Args&& ... args) const {
        os << "args:" << std::endl;
        //        ((os << hop::utils::to_string_annotate_type(std::forward<Args>(args))), ...);
        ((os << typeid(args).name() << ": " << make_printable(std::forward<Args>(args)) << std::endl), ...);
        os << std::endl;
    }
};

output_args_ output_args;



//namespace ns_test_21 {
//    // alternatives and sequences
//
//    using overloads_t = hop::ol_list <
//        hop::ol<hop::alt<hop::repeat<int, 1, 3>, std::string>>
//        , hop::ol<hop::pack<hop::seq<std::string, hop::alt<int, bool, double, std::string>>>>
//    >;
//
//    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr>
//    void foo(Args&& ... args) {
//
//        output_args(std::forward<Args>(args)...);
//    }
//
//
//
//
//
//    void test() {
//        foo("hello");
//        //foo(std::vector<int>{});
//        foo(42, 1, 1);
//        foo(42, 1);
//        foo(42, 1);
//        foo();
//
//        foo("a", 1.5, "b", "two", "c", false);
//    }
//}
//
//
//namespace ns_test_22 {
//    // using match_tag to select (kudos to Quuxplusone)
//
//    struct tag_ints {};
//    struct tag_doubles {};
//
//    using overloads = hop::ol_list <
//        hop::tagged_ol<tag_ints, int, hop::non_empty_pack<int>>,
//        hop::tagged_ol<tag_doubles, int, hop::non_empty_pack<double>>
//    >;
//
////    template<typename... Args, typename active_ol = hop::enable_t<overloads_t, Args...>, active_ol* = nullptr >
//
//    template<typename... Args,
//        typename OL = hop::enable_t<overloads, Args...>,
//        hop::ol_matches_tag_t<OL, tag_ints> = 0
//
////        hop::match_tag_t<overloads, tag_ints, Args...> = 0
//    >
//        void foo(Args&& ... args) {
//        os << "int overload called: " << std::endl;
//        output_args(std::forward<Args>(args)...);
//    }
//
//    template<typename... Args,
//        typename OL = hop::enable_t<overloads, Args...>,
//        hop::ol_matches_tag_t<OL, tag_doubles> = 0
////        hop::match_tag_t<overloads, tag_doubles, Args...> = 0
//    >
//        void foo(Args&& ... args) {
//        os << "double overload called: " << std::endl;
//        output_args(std::forward<Args>(args)...);
//    }
//
//    void test() {
//        foo(1, 2);
//        foo(1.0, 2);
//        foo(1, 2.0);
//        foo(1, 2.0, 3.0);
//        // foo(1, 2.0, 3); // error ambigous
//    }
//}
//
//
//
//namespace ns_test_24 {
//    // global template type argument deduction
//
//    using namespace boost::mp11;
//
//    struct mzalloc :std::allocator<int> {};
//
//    template<class T1, class T2>
//    using vector_alias = std::vector<T1, T2>const&;
//
//
//    struct tag_vector;
//    struct tag_vector_value_type;
//
//
//    using overloads_t = hop::ol_list <
//        hop::tagged_ol<tag_vector, hop::pack<hop::deduce_mixed<mp_list<hop::global_deduction_binding<0, tag_vector_value_type>>, vector_alias>>>
//    >;
//
//
//    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
//    void foo(Args&& ... args) {
//        using OL = hop::enable_t<overloads_t, Args...>;
//
//        if constexpr (hop::has_tag_v<OL, tag_vector>) {
//            output_args(std::forward<Args>(args)...);
//        }
//    }
//
//
//    void test() {
//        std::vector<float> my_map;
//        std::vector<int, mzalloc> my_set;
//        //foo(my_map, my_set);
////        foo(my_map, my_set, my_map);
////        foo(my_set, my_map);
//
//        //foo(my_map, another_set); // error
//    }
//}

namespace ns_test_25 {
    using namespace hop::utils;


    void barrrrahf(int i = 9, std::string s = "default") {
        std::cout << "bar called: " << to_string_annotate_type(s) << std::endl;
    }
    void bar_empty() {
        std::cout << "empty called: " << std::endl;
    }



    struct adapt_bar {
        template<class... Ts>
        static decltype(barrrrahf(std::declval<Ts>()...)) forward(Ts&&... ts) {
            return barrrrahf(std::forward<Ts>(ts)...);
        }
    };

    struct adapt_bar_empty {
        template<class... Ts>
        static decltype(bar_empty(std::declval<Ts>()...)) forward(Ts&&... ts) {
            return bar_empty(std::forward<Ts>(ts)...);
        }
    };

    using overloads_t = hop::ol_list<
        hop::adapted<adapt_bar>
        ,
        hop::adapted<adapt_bar_empty>
        , hop::ol<int, hop::pack<int>>
    >;


    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... args) {

        using ols = hop::enable_t<overloads_t, Args...>;

        constexpr auto selected_type_index = hop::index<ols>::value;
        if constexpr (selected_type_index == 0) {
            barrrrahf(args...);
        }

    }

    void test() {
        //  adapt_bar::template forward();
        int n = 5;
        int const m = 6;
        // foo(7); // ambiguous
        //foo(1, 2);
        //foo('a', 'b');
        //foo(1.7, 2.4f);
        //foo(n, m);
        //foo();
    }
}


namespace ns_test_20 {
    // adapting existing functions


    void bar(int n, std::string s) {
        os << "bar called: " << std::endl;
        output_args(n, s);
    }

    template<class T>
    void qux(T t, double d1, double d2) {
        os << "qux called: " << std::endl;
        output_args(t, d1, d2);
    }

    // adapt overload-set 'qux'
    struct adapt_qux {
        template<class... Args>
        static decltype(qux(std::declval<Args>()...)) forward(Args&&... args) {
            return qux(std::forward<Args>(args)...);
        }
    };


    void bar_empty(bool but_its_ok = false) {
        os << "a very sad function called";
        if (but_its_ok) {
            os << ", but it's ok";
        } else {
            os << ", bar is empty";
        }
        os << std::endl;
    }


    // adapt function with defaulted parameters 'bar_empty'
    struct adapt_bar_empty {
        template<class... Args>
        static decltype(bar_empty(std::declval<Args>()...)) forward(Args&&... args) {
            return bar_empty(std::forward<Args>(args)...);
        }
    };


    using overloads_t = hop::ol_list <
        hop::adapt<bar>
        , hop::adapted<adapt_qux>
        , 
        hop::adapted<adapt_bar_empty>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... args) {
        using OL = hop::enable_t<overloads_t, Args...>;
        if constexpr (hop::is_adapted_v<OL>) {
            return hop::forward_adapted<OL>(std::forward<Args>(args)...);
        } else {
            using t = boost::mp11::mp_first<OL>;

        }
    }





    void test() {
        foo(0, "Hello");
        foo(std::vector<std::string>{}, 2, 3);
        foo();
        foo(false);
        foo(true);
    }
}

using A = int;
template<class ...>
using Result = int;

using namespace std;
//foo(A a1);
//foo(A a1, A a2);
//foo(A a1, A a2, A a3);
//foo(A a1, A a2, A a3, A a4);
//...



template <class... Args>
requires
    (is_convertible_v<Args, int> && ...)    // int args
void foo(Args&&... args) {}

template <class... Args>
requires
    (is_convertible_v<Args, float> && ...)  // float args
void foo(Args&&... args) {}

//foo(1, 2, 3);     // (a)
//foo(0.5f, -2.4f); // (b)
//foo(1.5f, 3);     // (c)

namespace ns_test_19 {
    // local template type argument deduction



    template<class T, int N>
    using array_alias = std::array<T,N>const&;


    struct tag_array;


    template <class OL, class T, T... I>
    void print_deduced(std::integer_sequence<T, I...>) {
        ((os << typeid(hop::deduced_local_types<OL, I>).name() << std::endl), ...);
    }


    using namespace boost::mp11;

    template<template<class, int> class Pattern_>
    struct my_deducer_local {

        template<template<class, int> class Pattern, class T1, int T2>
        static mp_list<std::true_type, mp_list<T1, std::integral_constant<int, T2>>>
            test(Pattern<T1, T2>);


        template<template<class...> class Pattern>
        static mp_list<std::false_type, mp_list<>> test(...);

        template<class T>
        using fn = mp_first<decltype(test<Pattern_>(std::declval<T>()))>;

        template<class T>
        using deduced = mp_second<decltype(test<Pattern_>(std::declval<T>()))>;
    };


    template<template<class, int> class Pattern>
    using my_deduce_local = hop::fwd_if_q<my_deducer_local<Pattern>>;








    using overloads_t = hop::ol_list <
        hop::tagged_ol<tag_array, hop::pack<my_deduce_local<array_alias>>>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... args) {
        using OL = hop::enable_t<overloads_t, Args...>;

        if constexpr (hop::has_tag_v<OL, tag_array>) {
            os << "deduced types\n";
            print_deduced<OL>(std::make_index_sequence<sizeof...(args)>{});

        //    output_args(std::forward<Args>(args)...);
        }
    }



    void test() {
        std::array<int,5> v1;
        foo(v1);
    }
}

int main() {
    //foo3(3);
#define CALL_TEST(n)    \
    os << std::endl << "START TEST " #n << std::endl << std::endl;\
    ns_test_##n::test();

    CALL_TEST(19);
    CALL_TEST(20);
    CALL_TEST(25);

}

