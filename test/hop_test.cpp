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
#include "..\include\hop.hpp"

// intentionally NOT "using namespace hop;" to better show, where hop-types are used 


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
std::string to_string(T&& cont){
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
    void operator()(Args&& ... ts) const {
        os << "args:" << std::endl;
        ((os << typeid(ts).name() << ": " << make_printable(std::forward<Args>(ts)) << std::endl), ...);
        os << std::endl;
    }
};

output_args_ output_args;


namespace ns_test_1 {
    // a single overload with a single parameter

    using overloads_t = hop::ol_list <
        hop::ol<std::string>        // one std::string
    >;

    // alias template to simplify repetitions
    template<typename... Args>
    using enabler = hop::enable_t<overloads_t, Args...>;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
    }


    void test() {
        foo("Hello");
        auto hop = "hop";
        foo(hop);
        foo(std::string{ "world!" });
    }
}


namespace ns_test_2 {
    // a homogenous pack

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<int>>     // accept a (possibly empty) list of ints
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        // Attention: 
        // output_args(ts...)
        // will only output the caller provided arguments.
        // to get the parameters with defaulted parameters call
        // hop::get_args<OL>(std::forward<Args>(ts)...)
        // which returns a std::tuple containing the caller provided and defaulted parameters, to output call
        // std::apply(output_args, hop::get_args<OL>(std::forward<Args>(ts)...))

        output_args(std::forward<Args>(ts)...);
        std::apply(output_args, hop::get_args<OL>(std::forward<Args>(ts)...));
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        // Attention: 
        // output_args(ts...)
        // will only output the caller provided arguments.
        // to get the parameters with defaulted parameters call
        // hop::get_args<OL>(std::forward<Args>(ts)...)
        // which returns a std::tuple containing the caller provided and defaulted parameters, to output call
        // std::apply(output_args, hop::get_args<OL>(std::forward<Args>(ts)...))

        output_args(std::forward<Args>(ts)...);
        std::apply(output_args, hop::get_args<OL>(std::forward<Args>(ts)...));
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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

    void f(int) {}
    void f(double) {}
    void foo_plain(int, int, int, int, int) {}
    void foo_plain(int, int, int, int, double) {}
    void foo_plain(int, int, int, double, double) {}
    void foo_plain(int, int, double, double, double) {}
    void foo_plain(int, double, double, double, double) {}
    void foo_plain(double, double, double, double, double) {}

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<hop::tagged_ty<tag_int, int>>, hop::pack<hop::tagged_ty<tag_double, double>>>     // a list of ints followed by a list of doubles
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        auto&& int_args = hop::get_tagged_args<OL, tag_int>(std::forward<Args>(ts)...);
        auto&& double_args = hop::get_tagged_args<OL, tag_double>(std::forward<Args>(ts)...);
        std::apply(output_args, std::forward<decltype(int_args)>(int_args));
        std::apply(output_args, std::forward<decltype(double_args)>(double_args));

        static constexpr size_t int_args_count = hop::get_tagged_count<OL, tag_int>();
        os << "int args count: " << int_args_count << std::endl;
        static constexpr size_t double_args_count = hop::get_tagged_count<OL, tag_double>();
        os << "double args count: " << double_args_count << std::endl;
    }


    void test() {
        f((short)3);
        foo_plain(1, 2, (short)3, 3.4, 1.f);
        foo(1, 2, (short)3, 3.4, 1.f);
        foo(1, 2, 3, 3.4);
        foo(1, 2, (short)3, 3.4, 1.f);
    }
}

namespace ns_test_9 {
    // forwarding reference

    using overloads_t = hop::ol_list <
        hop::ol<hop::pack<hop::fwd>>     // a list of forwarding references
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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
    struct at_least_4_byte : mp_bool<sizeof(std::remove_cv_t<std::remove_reference_t<T1>>) >= 4 && sizeof(std::remove_cv_t<std::remove_reference_t<T2>>) >= 4> {};

    using overloads_t = hop::ol_list <
        hop::ol_if<at_least_4_byte, int, int>     // an int (no conversion to int)
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
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
    // accessing cpp-style default-params (with get_tagged_arg_or or get_indexed_defaulted)

    struct init_hallo {
        std::string operator()() const { return "hallo defaulted world"; }
    };

    struct tag_defaulted_string;
    struct tag_defaulted_double;

    using overloads_t = hop::ol_list <
        hop::ol<int, hop::cpp_defaulted_param<hop::tagged_ty<tag_defaulted_string, std::string>, init_hallo >, hop::cpp_defaulted_param<hop::tagged_ty<tag_defaulted_double, double> >>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        os << "get_tagged_arg_or:" << std::endl;
        auto defaulted_param_0 = hop::get_tagged_arg_or<OL, tag_defaulted_string>(-1, std::forward<Args>(ts)...);
        auto defaulted_param_1 = hop::get_tagged_arg_or<OL, tag_defaulted_double>(-1, std::forward<Args>(ts)...);
        std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));

        os << "get_indexed_defaulted:" << std::endl;
        if constexpr (hop::defaults_specified<OL>::value == 0) {
            auto defaulted_param_0 = hop::get_indexed_defaulted<OL, 0>(std::forward<Args>(ts)...);
            auto defaulted_param_1 = hop::get_indexed_defaulted<OL, 1>(std::forward<Args>(ts)...);
            std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));
        } else if constexpr (hop::defaults_specified<OL>::value == 1) {
            auto defaulted_param_0 = hop::get_indexed_defaulted<OL, 0>(std::forward<Args>(ts)...);
            auto defaulted_param_1 = hop::get_indexed_defaulted<OL, 1>(std::forward<Args>(ts)...);
            std::apply(output_args, std::make_tuple(defaulted_param_0, defaulted_param_1));
        } else if constexpr (hop::defaults_specified<OL>::value == 2) {
            auto defaulted_param_0 = hop::get_indexed_defaulted<OL, 0>(std::forward<Args>(ts)...);
            auto defaulted_param_1 = hop::get_indexed_defaulted<OL, 1>(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        if constexpr (hop::index<OL>::value == 0) {
            os << "overload: (long, long)" << std::endl;
            output_args(std::forward<Args>(ts)...);
        } else if constexpr (hop::index<OL>::value == 1) {
            os << "overload: (double, double)" << std::endl;
            output_args(std::forward<Args>(ts)...);
        } else if constexpr (hop::index<OL>::value == 2) {
            os << "overload: (std::string...)" << std::endl;
            output_args(std::forward<Args>(ts)...);
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

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        if constexpr (hop::has_tag_v<OL, tag_longs>) {
            os << "overload: (long, long)" << std::endl;
            output_args(std::forward<Args>(ts)...);
        } else if constexpr (hop::has_tag_v<OL, tag_doubles>) {
            os << "overload: (double, double)" << std::endl;
            output_args(std::forward<Args>(ts)...);
        } else if constexpr (hop::has_tag_v<OL, tag_strings>) {
            os << "overload: (std::string...)" << std::endl;
            output_args(std::forward<Args>(ts)...);
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
    // get_tagged_arg_or

    struct tag_long_arg;



    using overloads_t = hop::ol_list <
        hop::ol<std::string, hop::tagged_ty<tag_long_arg, long>>
        , hop::ol<hop::general_defaulted_param<hop::tagged_ty<tag_long_arg, long>>, double, double>
        , hop::ol<int, int, std::string>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        std::apply(output_args, hop::get_args<OL>(std::forward<Args>(ts)...));

        // hop::get_tagged_arg_or returns the specified tagged-parameter (or its default-value), if there is no parameter with the specified tag, then the 'or' value is returned
        auto&& long_arg = hop::get_tagged_arg_or<OL, tag_long_arg>(42.0, std::forward<Args>(ts)...);
        output_args(std::forward<decltype(long_arg)>(long_arg));
    }

    class foo_class {
    public:
        template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
        foo_class(Args&& ... ts) {}
    };

    void test() {
        foo("text", 3L);
        foo(5, 1.0, 2.0);
        foo(11.0, 12.0);
        foo(11.0, 12.0, "another text");

        auto fc1 = foo_class("text", 3L);
        auto fc2 = foo_class(5, 1.0, 2.0);
        auto fc3 = foo_class(11.0, 12.0);
        auto fc4 = foo_class(11.0, 12.0, "another text");
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


        template<typename... Args, hop::enable_t<overloads, Args...>* = nullptr >
        void foo(Args&& ... ts) {
            using OL = hop::enable_t<overloads, Args...>;

            if constexpr (hop::has_tag_v<OL, tag_ints>) {
                os << "overload: base::foo(int, ...)" << std::endl;
                output_args(std::forward<Args>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_doubles>) {
                os << "overload: base::foo(double, ...)" << std::endl;
                output_args(std::forward<Args>(ts)...);
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

        template<typename... Args, hop::enable_t<ext_overloads, Args...>* = nullptr >
        void foo(Args&& ... ts) {
            using OL = hop::enable_t<ext_overloads, Args...>;

            if constexpr (hop::is_from_base_v<OL>) {
                base::foo(std::forward<Args>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_strings>) {
                os << "overload: derived::foo(std::string, ...)" << std::endl;
                output_args(std::forward<Args>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_floats>) {
                os << "overload: derived::foo(float, ...)" << std::endl;
                output_args(std::forward<Args>(ts)...);
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


namespace ns_test_17 {
    // invoking different base-class constructors via constructor delegation
    // constructor delegation is needed when base-class constructors with different arities are involved
    // (the same technique can be used to invoke different constructors for class members)


    class base_class {
    public:
        base_class() {}
        base_class(std::string) {}
    };


    class foo_class : public base_class {
    public:
        struct tag_ints {};
        struct tag_string {};

        using overloads_t = hop::ol_list <
            hop::tagged_ol<tag_ints, hop::pack<int>>        // calls base's default ctor
            , hop::tagged_ol<tag_string, std::string, double, double>        // calls base's (std::string) ctor
        >;


        template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
        foo_class(Args&& ... ts)
            // delegate constructor using tag-dispatching
            : foo_class(hop::get_tag_type<hop::enable_t<overloads_t, Args...>>{}, std::forward<Args>(ts)...) {
            using OL = hop::enable_t<overloads_t, Args...>;

            output_args(std::forward<Args>(ts)...);
        }

    private:
        // helper contructor to create base_class without arg
        template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
        foo_class(tag_ints, Args&& ... ts)
            : base_class{} {}

        // helper contructor to create base_class with string arg
        template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
        foo_class(tag_string, Args&& ... ts)
            : base_class{ hop::get_arg_at<0>(std::forward<Args>(ts)...) } {}
    };

    void test() {
        //        foo("Hello");
        auto foo_ints = foo_class(0, 1, 2, 3);
        auto foo_string = foo_class("a string", 0.5, -1.e5);
    }
}


namespace ns_test_18 {
    // global template type argument deduction

    template<class T1, class T2>
    using map_alias = std::map<T1, T2>const&;

    template<class T1, class T2>
    using set_alias = std::set<T2>const&;

    struct tag_map_set;


    using overloads_t = hop::ol_list <
        hop::tagged_ol<tag_map_set, hop::deduce<map_alias>, hop::deduce<set_alias>>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        if constexpr (hop::has_tag_v<OL, tag_map_set>) {
            output_args(std::forward<Args>(ts)...);
        }
    }


    void test() {
        std::map<int, std::string> my_map;
        std::set<std::string> my_set;
        foo(my_map, my_set);

        std::set<double> another_set;
        //foo(my_map, another_set); // error
    }
}

namespace ns_test_19 {
    // local template type argument deduction

    template<class T>
    using vector_alias = std::vector<T>const&;

    template<class T, class Alloc>
    using list_alloc_alias = std::list<T, Alloc>const&;

    struct tag_vector;
    struct tag_list_alloc;


    template <class OL, class T, T... I>
    void print_deduced(std::integer_sequence<T, I...>) {
        ((os << typeid(hop::deduced_local_types<OL, I>).name() << std::endl), ...);
    }


    using overloads_t = hop::ol_list <
        hop::tagged_ol<tag_vector, hop::pack<hop::deduce_local<vector_alias>>>
        , hop::tagged_ol<tag_list_alloc, hop::deduce_local<list_alloc_alias>>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        if constexpr (hop::has_tag_v<OL, tag_vector>) {
            os << "deduced types\n";
            print_deduced<OL>(std::make_index_sequence<sizeof...(ts)>{});

            output_args(std::forward<Args>(ts)...);
        } else if constexpr (hop::has_tag_v<OL, tag_list_alloc>) {
            using T = hop::deduced_local_types<OL, 0>;
            os << typeid(T).name() << std::endl;
            output_args(std::forward<Args>(ts)...);
        }
    }



    void test() {
        foo(std::list<int>{});

        std::vector<int> v1;
        std::vector<double> v2;
        std::vector<std::string> v3;
        foo(v1, v2, v3);
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
        static decltype(qux(std::declval<Args>()...)) forward(Args&&... ts) {
            return qux(std::forward<Args>(ts)...);
        }
    };

    using overloads_t = hop::ol_list <
        hop::adapt<bar>
        , hop::adapted<adapt_qux>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;
        if constexpr (hop::is_adapted_v<OL>) {
            return hop::forward_adapted<OL>(std::forward<Args>(ts)...);
        } else {
            using t = boost::mp11::mp_first<OL>;

        }
    }





    void test() {
        foo(0, "Hello");
        foo(std::vector<std::string>{}, 2, 3);
    }
}



namespace ns_test_21 {
    // alternatives and sequences

    using overloads_t = hop::ol_list <
        hop::ol<hop::alt<hop::repeat<int, 1, 3>, std::string>>
        , hop::ol<hop::pack<hop::seq<std::string, hop::alt<int, bool, double, std::string>>>>
    >;

    template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
    void foo(Args&& ... ts) {
        using OL = hop::enable_t<overloads_t, Args...>;

        output_args(std::forward<Args>(ts)...);
    }





    void test() {
        foo("hello");
        foo(42);
        foo(42, 1, 1);
        foo(42, 1);
        foo(42, 1);
        foo();
        
        foo("a", 1.5, "b", "two", "c", false);
    }
}


namespace ns_test_22 {
    // using match_tag to select (kudos to Quuxplusone)

    struct tag_ints {};
    struct tag_doubles {};

    using overloads = hop::ol_list <
        hop::tagged_ol<tag_ints, int, hop::non_empty_pack<int>>,
        hop::tagged_ol<tag_doubles, int, hop::non_empty_pack<double>>
    >;

    template<typename... Args,
        hop::match_tag_t<overloads, tag_ints, Args...> = 0
    >
        void foo(Args&& ... ts) {
        os << "ints overload called: " << std::endl;
    }

    template<typename... Args,
        hop::match_tag_t<overloads, tag_doubles, Args...> = 0
    >
        void foo(Args&& ... ts) {
        os << "doubles overload called: " << std::endl;
    }

    void test() {
        foo(1, 2);
        foo(1.0, 2);
        foo(1, 2.0);
        foo(1, 2.0, 3.0);
        // foo(1, 2.0, 3); // error ambigous
    }
}


int main() {

#define CALL_TEST(n)    \
    os << std::endl << "START TEST " #n << std::endl << std::endl;\
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
    CALL_TEST(17);
    CALL_TEST(18);
    CALL_TEST(19);
    CALL_TEST(20);
    CALL_TEST(21);
    CALL_TEST(22);

}

