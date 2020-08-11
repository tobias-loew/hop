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

namespace test4 {
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




//    namespace ns_test_2 {
//        // a homogenous pack
//
//        using overloads_t = hop::ol_list <
////            hop::ol<hop::pack<int>>     // accept a (possibly empty) list of ints
//            hop::ol<hop::repeat<int,2>>     // accept exactly 2 ints
//        >;
//
//        template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
//        void foo(Ts&& ... ts) {
//            using OL = decltype(hop::enable<overloads_t, Ts...>());
//
//            output_args(std::forward<Ts>(ts)...);
//        }
//
//
//        void test() {
//            foo(1,2);
//            //foo(42);
//            //int n = 5;
//            //foo(n, 2, -4, 4.6, 'c');
//            // foo("hello error");  // error: no matching overloaded function found
//        }
//    }


    namespace ns_test_401 {
        // a single overload with a single parameter

        struct gathered_tag;

        using overloads_t = hop::ol_list <
            //hop::ol<std::string>
            //,
//            hop::ol<hop::alt<hop::repeat<int, 1, 3>, std::string>>
//hop::ol<hop::gather<gathered_tag, hop::alt<hop::repeat<int, 1, 3>, std::string>>>
hop::ol<hop::pack<hop::gather<gathered_tag, int>>>
//,hop::ol<hop::pack<hop::seq<std::string, hop::alt<int, bool, double, std::string>>>>
//            hop::ol<hop::repeat<int, 1, 3>>
          //  hop::ol<hop::repeat<int, 0, 2>, hop::repeat<std::string, 0, 2>>
        >;

        template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<overloads_t, Ts...>());

            //if constexpr (hop::has_tag_v<OL, tag_vector>) {
            //    //output_args(std::forward<Ts>(ts)...);
            //} else if constexpr (hop::has_tag_v<OL, tag_list_alloc>) {
            //                using Actual = hop::deduced_types<OL>;

            //                //typename hop::debug<Actual> d;
            //                //typename hop::debug<boost::mp11::mp_second<Actual>> d;
            //                using arg_0_t = boost::mp11::mp_first<Actual>;
            //                arg_0_t t;
            //                t = 42;
            //              //z  typename hop::debug<arg_0_t>::type d;
            //                int i = 42;
            //                //output_args(std::forward<Ts>(ts)...);
            //}
            ////output_args(std::forward<Ts>(ts)...);
        }





        void test() {
          //  foo(42,"");
            //foo("hello");
            //foo(42);
            //foo(42, 1, 1);
            //foo(42, 1);
            foo(42,1);
            //foo();
            //foo("a",1.5, "b", "two", "c", false);
        }
    }



    namespace ns_test_402 {
        // a single overload with a single parameter

        template<int N>
        struct GeneralArrayStorage {};

        class _bz_fortranTag {
        public:
            constexpr operator GeneralArrayStorage<1>() const
            {
                return {};
            }

        };
        struct tag_ArrayStorage {};
        struct init_default_GeneralArrayStorage {
            constexpr GeneralArrayStorage<1> operator()() const { return {}; }
        };

    //    using array_storage_t = hop::tagged_ty<tag_ArrayStorage, hop::cpp_defaulted_param<GeneralArrayStorage<1>, init_default_GeneralArrayStorage>>;
        using array_storage_t = hop::cpp_defaulted_param<GeneralArrayStorage<1>, init_default_GeneralArrayStorage>;

        using overloads_t = hop::ol_list <
            hop::ol<array_storage_t>
        >;

        template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
        void foo(Ts&& ... ts) {
            using OL = decltype(hop::enable<overloads_t, Ts...>());

            //if constexpr (hop::has_tag_v<OL, tag_vector>) {
            //    //output_args(std::forward<Ts>(ts)...);
            //} else if constexpr (hop::has_tag_v<OL, tag_list_alloc>) {
            //                using Actual = hop::deduced_types<OL>;

            //                //typename hop::debug<Actual> d;
            //                //typename hop::debug<boost::mp11::mp_second<Actual>> d;
            //                using arg_0_t = boost::mp11::mp_first<Actual>;
            //                arg_0_t t;
            //                t = 42;
            //              //z  typename hop::debug<arg_0_t>::type d;
            //                int i = 42;
            //                //output_args(std::forward<Ts>(ts)...);
            //}
            ////output_args(std::forward<Ts>(ts)...);
        }



        _bz_fortranTag fortranArray;


        void test() {
          //  foo(42,"");
            //foo("hello");
            //foo(42);
            //foo(42, 1, 1);
            //foo(42, 1);
            foo(GeneralArrayStorage<1>{});
            foo(fortranArray);
            //foo();
            //foo("a",1.5, "b", "two", "c", false);
        }
    }

    int main4() {

 
#define CALL_TEST(n)    \
    std::cout << std::endl << "START TEST " #n << std::endl << std::endl;\
    ns_test_##n::test();

        CALL_TEST(402);


        //static_assert(min_size<int, double>::value == 4);
        //static_assert(min_size<char, double, std::string>::value == 1);
        return 0;
    }

}