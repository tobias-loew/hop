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

#include <iostream>
#include <string>
#include <locale>
#include <codecvt> 
#include <vector>
#include "..\include\hop.hpp"


template<class T>
struct tag_t { constexpr tag_t() {} };

template<class T>
auto printer(tag_t<T>) {
    return [](auto&& t)->decltype(auto) {
        return std::forward<decltype(t)>(t);
    };
}

std::string ws_to_s(std::wstring const& ws) {
    // use very simple ws to s converter (I know it has it's problems...)
    return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(ws);
}


auto printer(tag_t<std::wstring>) {
    return [](std::wstring const& s) {return ws_to_s(s); };
}

auto printer(tag_t<std::wstring&>) {
    return [](std::wstring const& s) {return ws_to_s(s); };
}

auto printer(tag_t<std::wstring&&>) {
    return [](std::wstring const& s) {return ws_to_s(s); };
}

auto printer(tag_t<std::wstring const &&>) {
    return [](std::wstring const& s) {return ws_to_s(s); };
}

template<class T>
decltype(auto) make_printable(T&& arg) {
    return printer(tag_t<T>{})(std::forward<T>(arg));
}


struct output_args_ {



    template<class... Ts>
    void operator()(Ts&& ... ts) const {
        std::cout << "args:" << std::endl;
        ((std::cout << make_printable(std::forward<Ts>(ts)) << std::endl), ...);
        std::cout << std::endl;
    }
};

output_args_ output_args;


namespace ns_test_1 {
    // a single overload with a single parameter

    using overloads_t = hop::ol_list <
        hop::ol<std::string>        // one std::string
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo("Hello");
        auto hop = "hop";
        foo(hop);
        foo(std::string{"world!"});
    }
}


namespace ns_test_2 {
    // a homogenous pack

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<int>>     // accpet a (possibly empty) list of ints
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo();
        foo(42);
        int n = 5;
        foo(n, 2, -4, 4.6, 'c');
        // foo("hello error");  // error: no matching overloaded function found
    }
}



namespace ns_test_3 {
    // a repeating a type from [min, ... ,max] (or [min, ...) )

    using overloads_t = hop::ol_list <
        hop::ol<hop::repeat<int, 2, 5>>     // 2 - 5 ints
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        //foo();                    // error: no matching overloaded function found
        //foo(1);                   // error: no matching overloaded function found
        foo(1, 2);
        foo(1, 2, 3);
        foo(1, 2, 3, 4);
        foo(1, 2, 3, 4, 5);
        //foo(1, 2, 3, 4, 5, 6);    // error: no matching overloaded function found

    }
}

namespace ns_test_4 {
    // additional argument AFTER a pack

    using overloads_t = hop::ol_list <
        hop::ol<hop::non_empty_pack<int>, std::string>     // a non-empty list of ints followed by a std::string
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        //foo("no ints");           // error: no matching overloaded function found
        //foo(1);                   // error: no matching overloaded function found
        foo(42, "hop");
        foo(1, 2, 3, std::string{});

    }
}

namespace ns_test_5 {
    // trailing defaulted-parameters (C++ style)

    struct init_default {
        std::string operator()() const { return "default initialized"; }
    };


    using overloads_t = hop::ol_list <
        hop::ol<hop::non_empty_pack<int>, hop::cpp_defaulted_param<std::string, init_default>, hop::cpp_defaulted_param<std::wstring>>     // a non-empty list of ints followed by a defaulted std::string and a defaulted std::wstring
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        // Attention: 
        // output_args(ts...)
        // will only output the caller provided arguments.
        // to get the parameters with defaulted parameters call
        // hop::get_args<OL>(std::forward<Ts>(ts)...)
        // which returns a std::tuple containing the caller provided and defaulted parameters, to output call
        // std::apply(output_args, hop::get_args<OL>(std::forward<Ts>(ts)...))

        output_args(std::forward<Ts>(ts)...);
        std::apply(output_args, hop::get_args<OL>(std::forward<Ts>(ts)...));
    }


    void test() {
        //foo("no ints");           // error: no matching overloaded function found
        foo(1);
        foo(42, "hop");
        foo(1, 2, 3, std::string{ "a string" });
        foo(1, 2, 3, std::string{ "a string and a number" }, std::wstring{ L"into the great wide open" });
        // foo(1, 2, 3, L"into the great wide open"); // error: no matching overloaded function found
    }
}

namespace ns_test_6 {
    // general defaulted-parameters

    struct init_default {
        std::string operator()() const { return "default initialized"; }
    };


    using overloads_t = hop::ol_list <
        hop::ol<hop::general_defaulted_param<std::string, init_default>, hop::pack<int>>     // a defaulted std::string followed by a list of ints 
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        // Attention: 
        // output_args(ts...)
        // will only output the caller provided arguments.
        // to get the parameters with defaulted parameters call
        // hop::get_args<OL>(std::forward<Ts>(ts)...)
        // which returns a std::tuple containing the caller provided and defaulted parameters, to output call
        // std::apply(output_args, hop::get_args<OL>(std::forward<Ts>(ts)...))

        output_args(std::forward<Ts>(ts)...);
        std::apply(output_args, hop::get_args<OL>(std::forward<Ts>(ts)...));
    }


    void test() {
        foo("no ints");           // error: no matching overloaded function found
        foo(1, 2, 3);
        foo("hop", 42);
        foo();
    }
}

namespace ns_test_7 {
    // more than one pack...

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<int>, hop::pack<double>>     // a list of ints followed by a list of doubles
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo();
        foo(1, 2.5);
        // foo(1, 2.5, 42);    // that's ambigous
    }
}


namespace ns_test_8 {
    // tagging types to get a associated args
    struct tag_int;
    struct tag_double;

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<hop::tagged_ty<tag_int, int>>, hop::pack<hop::tagged_ty<tag_double, double>>>     // a list of ints followed by a list of doubles
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        auto&& int_args = hop::get_tagged_args<OL, tag_int>(std::forward<Ts>(ts)...);
        auto&& double_args = hop::get_tagged_args<OL, tag_double>(std::forward<Ts>(ts)...);
        std::apply(output_args, std::forward<decltype(int_args)>(int_args));
        std::apply(output_args, std::forward<decltype(double_args)>(double_args));

        static constexpr size_t int_args_count = hop::get_tagged_count<OL, tag_int>();
        std::cout << "int args count: " << int_args_count << std::endl;
        static constexpr size_t double_args_count = hop::get_tagged_count<OL, tag_double>();
        std::cout << "double args count: " << double_args_count << std::endl;
    }


    void test() {
        foo(1, 2, 3L, 3.4, 1.f);
    }
}

namespace ns_test_9 {
    // forwarding reference

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<hop::fwd>>     // a list of forwarding references
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo(1, "a text", std::wstring(L"a wide string"), 2.4);
    }
}

namespace ns_test_10 {
    // forwarding reference with SFINAE condition
    using namespace boost::mp11;

    using overloads_t = hop::ol_list <
        hop::ol<hop::fwd_if_q<mp_bind<std::is_same, _1, int>>>     // an int (no conversion to int)
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo(1);
        //foo(1L);    // error: no matching overloaded function found
        //foo(42.0);    // error: no matching overloaded function found
    }
}

namespace ns_test_11 {
    // SFINAE condition on whole parameter-set
    using namespace boost::mp11;

    template<class T1, class T2>
    struct at_least_4_byte : mp_bool<sizeof(std::remove_cvref_t<T1>) >= 4 && sizeof(std::remove_cvref_t<T2>) >= 4>
    {};

    using overloads_t = hop::ol_list <
        hop::ol_if<at_least_4_byte, int, int>     // an int (no conversion to int)
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        output_args(std::forward<Ts>(ts)...);
    }


    void test() {
        foo(1, 2);
        foo(1LL, 2);
        foo(4.2f, 2.4);
        // foo(1, (short)2);    // error: no matching overloaded function found
        // foo('a', 2);    // error: no matching overloaded function found
    }
}

namespace ns_test_12 {
    // accessing cpp-style deaulft-params

    struct init_hallo {
        std::string operator()() const { return "hallo defaulted world"; }
    };

    struct tag_default_string;

    using overloads_t = hop::ol_list <
        hop::ol<int, hop::cpp_defaulted_param<hop::tagged_ty<tag_default_string, std::string>, init_hallo >, hop::cpp_defaulted_param<double >>
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        if constexpr (hop::defaults_specified<OL>::value == 0) {
            auto defaulted_param_0 = hop::get_value_or_default<OL, 0>(std::forward<Ts>(ts)...);
            auto defaulted_param_1 = hop::get_value_or_default<OL, 1>(std::forward<Ts>(ts)...);
            std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));
        } else if constexpr (hop::defaults_specified<OL>::value == 1) {
            auto defaulted_param_0 = hop::get_value_or_default<OL, 0>(std::forward<Ts>(ts)...);
            auto defaulted_param_1 = hop::get_value_or_default<OL, 1>(std::forward<Ts>(ts)...);
            std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));
        } else if constexpr (hop::defaults_specified<OL>::value == 2) {
            auto defaulted_param_0 = hop::get_value_or_default<OL, 0>(std::forward<Ts>(ts)...);
            auto defaulted_param_1 = hop::get_value_or_default<OL, 1>(std::forward<Ts>(ts)...);
            std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));
        } else {
            static_assert(hop::dependent_false<OL>::value, "Ooops!");
        }
    }

    void test() {
        foo(0);
        foo(1, "specified");
        foo(2, "specified", 42.0);
    }
}


namespace ns_test_13 {
    // more than one overload

    using overloads_t = hop::ol_list <
        hop::ol<long, long>                 // index 0
        , hop::ol<double, double>           // index 1
        , hop::ol<hop::pack<std::string>>   // index 2
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        if constexpr (hop::index<OL>::value == 0) {
            std::cout << "overload: (long, long)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else if constexpr (hop::index<OL>::value == 1) {
            std::cout << "overload: (double, double)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else if constexpr (hop::index<OL>::value == 2) {
            std::cout << "overload: (std::string...)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else {
            static_assert(hop::dependent_false<OL>::value, "Ooops!");
        }
    }


    void test() {
        foo(1, 3L);
        // foo(1, 3LL);     // error: no matching overloaded function found
        foo(1.0, 3.0);
        foo("one", "three");
    }
}



namespace ns_test_14 {
    // tagging overloads

    struct tag_longs;
    struct tag_doubles;
    struct tag_strings;

    using overloads_t = hop::ol_list <
        hop::tagged_ol<tag_longs, long, long>                 // index 0
        , hop::tagged_ol<tag_doubles, double, double>           // index 1
        , hop::tagged_ol<tag_strings, hop::pack<std::string>>   // index 2
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        if constexpr (hop::has_tag_v<OL, tag_longs>) {
            std::cout << "overload: (long, long)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else if constexpr (hop::has_tag_v<OL, tag_doubles>) {
            std::cout << "overload: (double, double)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else if constexpr (hop::has_tag_v<OL, tag_strings>) {
            std::cout << "overload: (std::string...)" << std::endl;
            output_args(std::forward<Ts>(ts)...);
        } else {
            static_assert(hop::dependent_false<OL>::value, "Ooops!");
        }
    }


    void test() {
        foo(1, 3L);
        // foo(1, 3LL);     // error: no matching overloaded function found
        foo(1.0, 3.0);
        foo("one", "three");
    }
}



namespace ns_test_15 {
    // get_value_or

    struct tag_long_arg;



    using overloads_t = hop::ol_list <
        hop::ol<std::string, hop::tagged_ty<tag_long_arg, long>>         
        , hop::ol<hop::general_defaulted_param<hop::tagged_ty<tag_long_arg, long>>, double, double>
        , hop::ol<int, int, std::string>
    >;

    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());

        std::apply(output_args, hop::get_args<OL>(std::forward<Ts>(ts)...));

        // hop::get_value_or returns the specified tagged-parameter (or its default-value), if there is no parameter with the specified tag, then the 'or' value is returned
        auto&& long_arg = hop::get_value_or<OL, tag_long_arg>(42.0, std::forward<Ts>(ts)...);
        output_args(std::forward<decltype(long_arg)>(long_arg));
    }


    void test() {
        foo("text", 3L);
        foo(5, 1.0, 2.0);
        foo(11.0, 12.0);
        foo(11.0, 12.0, "another text");
    }
}



namespace ns_test_16 {
    // extending an overload set


    struct tag_ints {};
    struct tag_doubles {};
    struct tag_a_pack {};

    using overloads = hop::ol_list <
        hop::tagged_ol<tag_ints, hop::non_empty_pack<hop::tagged_ty<tag_a_pack, int>>>,
        hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
    >;

    class base {
    public:


        template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<overloads, Ts...>());

            if constexpr (hop::has_tag_v<OL, tag_ints>) {
                std::cout << "overload: base::foo(int, ...)" << std::endl;
                output_args(std::forward<Ts>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_doubles>) {
                std::cout << "overload: base::foo(double, ...)" << std::endl;
                output_args(std::forward<Ts>(ts)...);
            }
        }
    };

    struct tag_strings {};
    struct tag_floats {};

    class derived : public base {
    public:

        using ext_overloads = hop::ol_extend <
            overloads,
            hop::tagged_ol<tag_strings, hop::non_empty_pack<std::string>>,
            hop::tagged_ol<tag_floats, hop::non_empty_pack<float>>
        >;

        template<typename... Ts, decltype((hop::enable<ext_overloads, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<ext_overloads, Ts...>());

            if constexpr (hop::is_from_base_v<OL>) {
                base::foo(std::forward<Ts>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_strings>) {
                std::cout << "overload: derived::foo(std::string, ...)" << std::endl;
                output_args(std::forward<Ts>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_floats>) {
                std::cout << "overload: derived::foo(float, ...)" << std::endl;
                output_args(std::forward<Ts>(ts)...);
            }
        }
    };



    void test() {
        base _base;
        _base.foo(42, 17);
        _base.foo(-0.4, 12.0);
        _base.foo(1.5f, -0.4f, 12.0f);
        _base.foo(0.1, 2.5f);
        // _base.foo(42, 17.0);     // error: no matching overloaded function found
        
        derived _derived;
        _derived.foo(42, 17);
        _derived.foo(-0.4, 12.0);
        _derived.foo(1.5f, -0.4f, 12.0f);
        // _derived.foo(0.1, 2.5f);  // error: ambigous
        // _base.foo(42, 17.0);     // error: no matching overloaded function found
        _derived.foo("hello", "extended", "world!");
    }
}



int main() {
#define CALL_TEST(n)    \
    std::cout << std::endl << "START TEST " #n << std::endl << std::endl;\
    ns_test_##n::test();

    CALL_TEST(1);
    CALL_TEST(2);
    CALL_TEST(3);
    CALL_TEST(4);
    CALL_TEST(5);
    CALL_TEST(6);
    CALL_TEST(7);
    CALL_TEST(8);
    CALL_TEST(9);
    CALL_TEST(10);
    CALL_TEST(11);
    CALL_TEST(12);
    CALL_TEST(13);
    CALL_TEST(14);
    CALL_TEST(15);
    CALL_TEST(16);

}

