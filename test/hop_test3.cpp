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
#include <string>
#include <locale>
#include <codecvt> 
#include <vector>
#include <list>
#include <set>
#include <map>
#include "..\include\hop.hpp"

namespace test {
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

    auto printer(tag_t<std::wstring const&&>) {
        return [](std::wstring const& s) {return ws_to_s(s); };
    }

    template<class T>
    auto printer(tag_t<std::vector<T>>) {
        return [](std::vector<T> const& s) {return "a"; };
    }

    template<class T>
    auto printer(tag_t<std::vector<T>&>) {
        return [](std::vector<T> const& s) {return "a"; };
    }

    template<class T>
    auto printer(tag_t<std::vector<T>&&>) {
        return [](std::vector<T> const& s) {return "a"; };
    }

    template<class T>
    auto printer(tag_t<std::vector<T> const&&>) {
        return [](std::vector<T> const& s) {return "a"; };
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




    namespace ns_test_101 {
        // a single overload with a single parameter








        struct vector_test {

            template<class T, class _Ty>
            static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<T>> test(std::vector<T>, _Ty&&);

            struct no_match;
            static boost::mp11::mp_list<no_match> test(...);

            template<class T>
            using fn = std::is_same<T, boost::mp11::mp_first<decltype(test(std::declval<T>(), std::declval<T>()))>>;

            template<class T>
            using deduced = boost::mp11::mp_second<decltype(test(std::declval<T>(), std::declval<T>()))>;

        };


        //template<template<class...> class Pattern>
        //struct deduce {

        //    template<class T, class _Ty>
        //    static boost::mp11::mp_list<_Ty, boost::mp11::mp_list<T>> test(Pattern<T>, _Ty&&);

        //    struct no_match;
        //    static boost::mp11::mp_list<no_match> test(...);

        //    template<class T>
        //    using fn = std::is_same<T, boost::mp11::mp_first<decltype(test(std::declval<T>(), std::declval<T>()))>>;

        //    template<class T>
        //    using deduced = boost::mp11::mp_second<decltype(test(std::declval<T>(), std::declval<T>()))>;
        //};


        struct tag_vector;
        struct tag_list_alloc;

        template<class T1, class T2>
        using map_alias = std::map<T1, T2>const&;

        template<class T1, class T2>
        using set_alias = std::set<T2>const&;

        template<class T2, class Alloc2>
        using map_list_alloc = std::list<T2, Alloc2>const&;

        //template<class T>
        //using map_vector1 = std::vector<T>const&;


        using overloads_t = hop::ol_list <
            hop::ol<hop::deduce<map_alias>, hop::deduce<set_alias>>
            // one std::string
            //        hop::ol<hop::fwd_if_q<vector_test>>        // one std::string
    //        hop::tagged_ol<tag_vector, std::string, hop::deduce<map_vector>, hop::deduce<map_list_alloc>>        // one std::string
    //        , hop::tagged_ol<tag_vector, std::string, hop::deduce<map_list_alloc>>        // one std::string
    //        , hop::tagged_ol<tag_vector, hop::deduce<map_vector1>>        // one std::string
            , hop::tagged_ol<tag_vector, std::string>        // one std::string
            //,hop::ol<int>        // one std::string
            ,hop::tagged_ol<tag_list_alloc, hop::deduce<map_list_alloc>>        // one std::string
            //hop::ol<hop::fwd_if_q<deduce<std::vector>>>        // one std::string
            //,
            //hop::ol<hop::fwd_if_q<deduce_ref<std::vector>>>        // one std::string
            //        hop::ol<hop::tmpl_q<vector_test>>        // one std::string
        >;

        template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<overloads_t, Ts...>());

            if constexpr (hop::has_tag_v<OL, tag_vector>) {
                //output_args(std::forward<Ts>(ts)...);
            } else if constexpr (hop::has_tag_v<OL, tag_list_alloc>) {
                            using Actual = hop::deduced_types<OL>;

                            //typename hop::debug<Actual> d;
                            //typename hop::debug<boost::mp11::mp_second<Actual>> d;
                            using arg_0_t = boost::mp11::mp_first<Actual>;
                            arg_0_t t;
                            t = 42;
                          //z  typename hop::debug<arg_0_t>::type d;
                            int i = 42;
                            //output_args(std::forward<Ts>(ts)...);
            }
            //output_args(std::forward<Ts>(ts)...);
        }





        void test() {
            foo("Hello");
            foo(std::list<int>{});
             //       foo(std::string{}, std::list<int>{});
            //foo(std::map<int, std::string>{}, std::set<std::string>{});
            //foo(std::vector<int>{}/*, std::list<int>{}*/);
            //foo(std::vector{ "world!" });
            //foo(4);
            //std::vector<int> v;
            //      foo(v);
            //foo(std::vector<int>{}, std::list<float>{});
        }
    }




    namespace ns_test_102 {
        // a single overload with a single parameter





        void bar(int, std::string) {}

        template<class T>
        void qux(T, double, double) {}

        struct adapt_qux {

            template<class... Ts>
            static decltype(qux(std::declval<Ts>()...)) forward(Ts&&... ts) {
                //static decltype(qux(std::declval<Ts>()...)) forward(Ts&&... ts) {
                return qux(std::forward<Ts>(ts)...);
            }

        };

        using overloads_t = hop::ol_list <
            hop::adapt<bar>
            , hop::adapted<adapt_qux>
        >;

        template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<overloads_t, Ts...>());

        }





        void test() {
            foo(0, "Hello");
            foo(std::vector<std::string>{}, 2, 3);
        }
    }



    struct bend_plus_to_max {
        size_t value;
        constexpr bend_plus_to_max(size_t value) : value{ value } {}
    };
    constexpr bend_plus_to_max operator + (bend_plus_to_max lhs, bend_plus_to_max rhs) {
        return { std::max(lhs.value, rhs.value) };
    }

    template<typename... T>
    constexpr size_t max_sizeof() {
        return (bend_plus_to_max(sizeof(T)) + ... + bend_plus_to_max(0)).value;
    }


    static_assert(max_sizeof<int, char, double, short>() == 8);
    static_assert(max_sizeof<char, float>() == sizeof(float));
    static_assert(max_sizeof<int, char>() == 4);






    int main2() {
 
#define CALL_TEST(n)    \
    std::cout << std::endl << "START TEST " #n << std::endl << std::endl;\
    ns_test_##n::test();

        CALL_TEST(101);


        //static_assert(min_size<int, double>::value == 4);
        //static_assert(min_size<char, double, std::string>::value == 1);
        return 0;
    }

}