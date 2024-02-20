//////////////////////////////////////////////////////////////////
//
// hop library
//
//  Copyright Tobias Loew 2019. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see https://github.com/tobias-loew/hop
//

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt> 
#include <vector>
#include <list>
#include <set>
#include <map>
#include "hop.hpp"


//auto&& os = std::ofstream("hop_out.txt");
#include <ios>
auto&& os = std::cout;

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



    template<class... Ts>
    void operator()(Ts&& ... ts) const {
        os << "args:" << std::endl;
        ((os << typeid(ts).name() << ": " << make_printable(std::forward<Ts>(ts)) << std::endl), ...);
        os << std::endl;
    }
};

output_args_ output_args;



namespace ns_test_9 {
    // forwarding reference
    template<typename T>
    concept PlainType = true;

    // forwarding reference with SFINAE condition
    using namespace boost::mp11;

    template<typename T>
    struct concept_check : std::bool_constant<PlainType<T>>{};

    using overloads_t = hop::ol_list <
        hop::ol<hop::fwd_if_q<mp_bind<concept_check, _1>>>     // an int (no conversion to int)
    >;

    template<typename... Ts, hop::enable_t<overloads_t, Ts...>* = nullptr >
    void foo(Ts&& ... ts) {
        using OL = hop::enable_t<overloads_t, Ts...>;

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo(1);
        //foo(1L);    // error: no matching overloaded function found
        //foo(42.0);    // error: no matching overloaded function found
    }
}

namespace ns_test_22 {
    // using match_tag to select (kudos to Quuxplusone)

    struct tag_ints {};
    struct tag_doubles {};

    using overloads = hop::ol_list <
        hop::tagged_ol<tag_ints, std::string, hop::non_empty_pack<int>>,
        hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
    >;

    template<typename... Ts,
        hop::match_tag_t<overloads, tag_ints, Ts...> = 0
    >
        void foo(Ts&& ... ts) {
        os << "ints overload called: " << std::endl;
    }

    template<typename... Ts,
        hop::match_tag_t<overloads, tag_doubles, Ts...> = 0
    >
        void foo(Ts&& ... ts) {
        os << "doubles overload called: " << std::endl;
    }

    void test() {
        foo(1, 2);
        foo(1.0, 2.0);
        foo(1.0f, 2.0);
        foo("",1.0, 2);
        //foo(1, 2.0);
        //foo(1, 2.0, 3.0);
        // foo(1, 2.0, 3); // error ambigous
    }
}

namespace ns_test_23 {
    // using match_tag to select (kudos to Quuxplusone)
    using namespace hop;
    struct tag_ints {};
//        tagged_ol<tag_ints, n_times<int, 2>>

    using overloads = ol_list <
        tagged_ol<tag_ints, 

// ambigous, when called with one argument
seq<general_defaulted_param<int>, general_defaulted_param<int>>
        >
    >;

    template<typename... Ts,
        match_tag_t<overloads, tag_ints, Ts...> = 0
    >
        void foo(Ts&& ... ts) {
        os << "ints overload called: " << std::endl;
    }


    void test() {
      //  foo(1);
        foo(1, 2);
        foo(1.0, 2.0);
        foo(1.0f, 2.0);
        //foo("", 1.0, 2);
        //foo(1, 2.0);
        //foo(1, 2.0, 3.0);
        // foo(1, 2.0, 3); // error ambigous
    }
}

#if 0
namespace ns_test_23 {
    // WRONG USAGE !!!
    // 
    // using match_tag to select (kudos to Quuxplusone)

    struct tag_ints {};
    struct tag_doubles {};

    using overloads_int = hop::ol_list <
        hop::tagged_ol<tag_ints, hop::non_empty_pack<int>>
    >;
    using overloads_doubles = hop::ol_list <
        hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
    >;

    template<typename... Ts, hop::enable_t<overloads_int, Ts...>* = nullptr >
        void foo(Ts&& ... ts) {
        os << "ints overload called: " << std::endl;
    }

    template<typename... Ts, hop::enable_t<overloads_doubles, Ts...>* = nullptr >
        void foo(Ts&& ... ts) {
        os << "doubles overload called: " << std::endl;
    }

    void test() {
        foo(1, 2);
        foo(1.0, 2.0);
        foo(1.0f, 2.0);
        //foo(1.0, 2);
        //foo(1, 2.0);
        //foo(1, 2.0, 3.0);
        // foo(1, 2.0, 3); // error ambigous
    }
}
#endif


//template <class... Ts,
//    enable_if_t<
//    (sizeof...(Ts) > 0) &&                  // at least one argument
//    (std::is_convertible_v<Ts, A> && ...)   // all arguments convertible to A
//    >* = nullptr >
//    R foo(Ts&&... ts) {
//    /*...*/
//}

int main() {


    //std::integral_constant<size_t, Index>

#define CALL_TEST(n)    \
    os << std::endl << "START TEST " #n << std::endl << std::endl;\
    ns_test_##n::test();

    CALL_TEST(9);
    CALL_TEST(22);
    CALL_TEST(23);

}

