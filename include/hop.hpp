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

#ifndef HOP_HOP_HPP_INCLUDED
#define HOP_HOP_HPP_INCLUDED


//#include <concepts>
#include <type_traits>
#include <algorithm>
#include <boost/version.hpp>
#include <boost/mp11.hpp>
#include <boost/preprocessor.hpp>

#define HOP_ENABLE_REPEAT_OPTIMIZATION

#ifndef HOP_MAX_DEDUDCABLE_TYPES
#define HOP_MAX_DEDUDCABLE_TYPES 10
#endif

#define HOP_MAX_DEDUDCABLE_TYPES_END BOOST_PP_INC(HOP_MAX_DEDUDCABLE_TYPES)

// homogeneous varaiadic overload sets
namespace hop {
    using namespace boost::mp11;

#if BOOST_VERSION < 107300
    // definition of mp_flatten from https://github.com/boostorg/mp11/blob/master/include/boost/mp11/algorithm.hpp (master, b24d228)
    // most likely it will be part of boost version 1.73
    // mp_flatten<L, L2 = mp_clear<L>>
    namespace detail {

        template<class L2> struct mp_flatten_impl {
            template<class T> using fn = T;
//            template<class T> using fn = mp_if<mp_similar<L2, T>, T, mp_list<T>>;
        };

    } // namespace detail

    template<class L, class L2 = mp_clear<L>> using mp_flatten = mp_apply<mp_append, mp_push_front<mp_transform_q<detail::mp_flatten_impl<L2>, L>, mp_clear<L>>>;
#endif


#ifdef _DEBUG
    template<class... t>
    struct debug_impl;
    template<class... t>
    using debug = typename debug_impl<t...>::type;
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// used concepts
//
   
#if __cplusplus >= 201703L
    
    template<typename _Ty>
    concept Quoted = requires {
        typename _Ty::template fn<void*>;
    };

    template<typename _Ty>
    concept Adapted = requires {
        _Ty::forward;
    };

    template<typename _Ty>
    concept DefaultCreator = requires {
        _Ty{}.operator ();
    };


    
#else
#define Quoted typename
#define Adapted typename
#define DefaultCreator typename
#endif

// used concepts
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // meta and fused from http://attugit.github.io/2015/02/Accessing-nth-element-of-parameter-pack/  (http://coliru.stacked-crooked.com/a/ab5f39be452d9448)
    namespace meta {
        struct ignore final {
            template <class... Ts>
            constexpr ignore(Ts&& ...) noexcept {}
        };

        template <class>
        using eat = ignore;

        template <std::size_t>
        using eat_n = ignore;
    }

    namespace fused {
        template <class Tp, class... Us>
        constexpr decltype(auto) front(Tp&& t, Us&& ...) noexcept {
            return std::forward<Tp>(t);
        }

        template <class Tp, class... Us>
        constexpr decltype(auto) back(Tp&& t, Us&& ... us) noexcept {
            return [](meta::eat<Us>..., auto&& x) -> decltype(x) {
                return std::forward<decltype(x)>(x);
            }(std::forward<Tp>(t), std::forward<Us>(us)...);
        }

        namespace detail {
            template <std::size_t N, class = std::make_index_sequence<N>>
            struct at;

            template <std::size_t N, std::size_t... skip>
            struct at<N, std::index_sequence<skip...>> {
                template <class Tp, class... Us>
                constexpr decltype(auto) operator()(meta::eat_n<skip>..., Tp&& x,
                    Us&& ...) const noexcept {
                    return std::forward<Tp>(x);
                }
            };
        }

        template <std::size_t N, class... Ts>
        constexpr decltype(auto) nth(Ts&& ... args) {
            return detail::at<N>{}(std::forward<Ts>(args)...);
        }
    }


    // helper template always returning false
    // used e.g. in static_assert in never to be choosen "if constexpr" branches
    template<class... T> struct dependent_false : mp_false {};


    // not really infinity, but 4 billion parameters should be enough
    static constexpr size_t infinite = std::numeric_limits<size_t>::max();

    // repetition
    // template to create a repeated [min, .. , max] parameter
    template<class _Ty, size_t _min, size_t _max = infinite>
    struct repeat;


    // grouping a type-sequence
    template<class... _Tys>
    struct seq;

    // alternative types
    template<class... _Tys>
    struct alt;


    // template to tag a type
    template<class _Tag, class _Ty>
    struct tagged_ty;

    // template to gather a parameter-list
    template<class _Tag, class _Ty>
    struct gather;



    // syntactic sugar

    // type to create no parameter
    using eps = repeat<char, 0, 0>;

    // template to create an optional parameter (WITHOUT default value)
    template<class _Ty>
    using optional = repeat<_Ty, 0, 1>;

    // template to create a parameter pack
    template<class _Ty>
    using pack = repeat<_Ty, 0, infinite>;

    // template to create a non-empty parameter pack
    template<class _Ty>
    using non_empty_pack = repeat<_Ty, 1, infinite>;


    // template access parameter-type in tagged_ty
    namespace impl {
        template<class _Ty>
        struct remove_tag {
            using type = _Ty;
        };

        template<class _Tag, class _Ty>
        struct remove_tag<tagged_ty<_Tag, _Ty>> {
            using type = typename remove_tag<_Ty>::type;
        };
    }

    namespace impl {

        // default creation of an instance of type _Ty
        template<class _Ty>
        struct default_create {
            constexpr decltype(auto) operator()() const { return typename remove_tag<_Ty>::type{}; }
        };
    }

    // template to create a parameter with default value (C++-style defaulted parameter, only at end of parameter list)
    template<class _Ty, DefaultCreator _Init = impl::default_create<_Ty>>
    struct cpp_defaulted_param;

    // template to create a parameter with default value (C++-style, only at end of parameter list)
    template<class _Ty, DefaultCreator _Init = impl::default_create<_Ty>>
    struct general_defaulted_param;

    // template to create a forwarded parameter: _Ty has to be a quoted meta-function
    template<Quoted _Ty>
    struct tmpl_q;

    // template to create a forwarded parameter: _Ty has to be a meta-function
    template<template<class...> class F>
    using tmpl = tmpl_q<mp_quote<F>>;

    // struct to create a forward-reference
    namespace impl {
        template <class _Ty>
        using fwd_helper_t = _Ty&&;
    }
    using fwd = tmpl<impl::fwd_helper_t>;

    namespace impl {
        template<class... _Ty>
        using true_t = mp_true;

        struct none_tag {};


        template<class _If>
        struct if_test {
        private:
            struct invalid_type_tag;

        public:
            using _if = _If;

            template<class T>
            using fn = mp_if<
                typename _If::template fn<T>,
                T&&,
                invalid_type_tag
            >;

        };

        template<class _If>
        struct if_not_test {
        private:
            struct invalid_type_tag;

        public:
            template<class T>
            using fn = mp_if_c<
                !_If::template fn<T>::value,
                T&&,
                invalid_type_tag
            >;

        };


        template<template<class...> class Pattern_>
        struct deducer_local {

            // here we really would like to write the following test function
            // but since Pattern_ may be an alias template instantiating it with a pack is not allowed
            // template<template<class...> class Pattern, class... T>
            // static mp_list<std::true_type, mp_list<T...>> test(Pattern<T...>);

#define HOP_MACRO_LOCAL_DEDUCER_TEST(z, n, data)                                                                                \
            template<template<class...> class Pattern BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>                                \
            static mp_list<std::true_type, mp_list<BOOST_PP_ENUM_PARAMS(n, T)>> test(Pattern<BOOST_PP_ENUM_PARAMS(n, T)>);

            // overloads to deduce 1 - 10 template types
            BOOST_PP_REPEAT_FROM_TO(1, HOP_MAX_DEDUDCABLE_TYPES_END, HOP_MACRO_LOCAL_DEDUCER_TEST, _)

#undef HOP_MACRO_LOCAL_DEDUCER_TEST

                struct no_match;
            template<template<class...> class Pattern>
            static mp_list<std::false_type, mp_list<>> test(...);

            template<class T>
            using fn = mp_first<decltype(test<Pattern_>(std::declval<T>()))>;

            template<class T>
            using deduced = mp_second<decltype(test<Pattern_>(std::declval<T>()))>;
        };

        template<int arity, template<class...> class Pattern>
        struct pattern_helper;

#define HOP_MACRO_PATTERN_HELPER(z, n, data)                     \
        template<template<class...> class Pattern>               \
        struct pattern_helper<n, Pattern> {                      \
            template<BOOST_PP_ENUM_PARAMS(n, class T)>           \
            using fn = Pattern<BOOST_PP_ENUM_PARAMS(n, T)>;      \
        };

        BOOST_PP_REPEAT_FROM_TO(1, HOP_MAX_DEDUDCABLE_TYPES_END, HOP_MACRO_PATTERN_HELPER, _)

#undef HOP_MACRO_PATTERN_HELPER


            template<template<class...> class Pattern>
        struct deduction_pattern {
            // this is for perfect forwarding of a single argument; deduction is done elsewhere
            template<class T>
            using fn = T&&;



#define HOP_MACRO_ARITY_TEST(z, n, data)                                                                                             \
            template<template<class...> class Pattern_ BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(n, class T, = int BOOST_PP_INTERCEPT)>   \
            static std::integral_constant<size_t, n> arity_test(Pattern_<BOOST_PP_ENUM_PARAMS(n, T)>);


            BOOST_PP_REPEAT_FROM_TO(1, HOP_MAX_DEDUDCABLE_TYPES_END, HOP_MACRO_ARITY_TEST, _)

#undef HOP_MACRO_ARITY_TEST

                template<template<class...> class Pattern_>
            static std::integral_constant<size_t, 0> arity_test(...);   //fault



            template<class _Ty>
            using arity = decltype(arity_test<Pattern>(std::declval<_Ty>()));

            template<class _Ty>
            using pattern = pattern_helper<arity<_Ty>::value, Pattern>;
        };


        template<class T>
        struct lazy_expand {
            using type = T;
        };



        template<class... Pattern_q>
        struct deducer_t {

            // here we really would like to write the following test function
            // but since Pattern_ may be an alias template instantiating it with a pack is not allowed
                       // template<template<class...> class... Pattern, class... T>
                        // static mp_list<std::true_type, mp_list<T...>> test(Pattern<T...>...);

            // overloads to deduce 0 - 10 template types
            //template<template<class...> class... Pattern, class... _Tys>
            //static mp_list<mp_list<_Tys...>, mp_list<>> test(Pattern<>..., _Tys&&...);

            //template<template<class> class lazy_expander, class... _Tys>                                                                         
            //static mp_list<std::true_type, mp_list<_Tys...>> test(typename lazy_expander<Pattern_q>::type::template fn<_Tys...>...);

#define HOP_MACRO_DEDUCER_T_TEST(z, n, data)                                                                                                                                \
            template<template<class> class lazy_expander BOOST_PP_ENUM_TRAILING_PARAMS(n, class T)>                                                                         \
            static mp_list<std::true_type, mp_list<BOOST_PP_ENUM_PARAMS(n, T)>> test(typename lazy_expander<Pattern_q>::type::template fn<BOOST_PP_ENUM_PARAMS(n, T)>...);

            // overloads to deduce 1 - 10 template types
            BOOST_PP_REPEAT_FROM_TO(1, HOP_MAX_DEDUDCABLE_TYPES_END, HOP_MACRO_DEDUCER_T_TEST, _)

#undef HOP_MACRO_DEDUCER_T_TEST


            template<template<class> class lazy_expander>
            static mp_list<std::false_type, mp_list<>> test(...);

            template<class... Tys>
            using fn = mp_first<decltype(test<lazy_expand>(std::declval<Tys>()...))>;

            template<class... Tys>
            using deduced = mp_second<decltype(test<lazy_expand>(std::declval<Tys>()...))>;
        };
    }


    // struct to create guarded forward-reference
    template<Quoted _If>
    using fwd_if_q = tmpl_q<impl::if_test<_If>>;

    template<template<class> class _If>
    using fwd_if = fwd_if_q<mp_quote<_If>>;

    template<Quoted _If>
    using fwd_if_not_q = tmpl_q<impl::if_not_test<_If>>;

    template<template<class> class _If>
    using fwd_if_not = fwd_if_not_q<mp_quote<_If>>;



    template<template<class...> class Pattern>
    using deduce_local = fwd_if_q<impl::deducer_local<Pattern>>;

    template<template<class...> class Pattern>
    using deduce = tmpl_q< impl::deduction_pattern<Pattern>>;

    namespace impl {
        template<auto f>
        struct adapter {
            template<class... Ts>
            static decltype(f(std::declval<Ts>()...)) forward(Ts&&... ts) {
                return f(std::forward<Ts>(ts)...);
            }

        };

        template<Adapted T>
        struct adapter_test {

            template<class... Ts>
            static true_t<decltype(T::forward(std::declval<Ts>()...))> test(Ts&&...);

            static std::false_type test(...);

            template<class... Ts>
            using fn = decltype(test(std::declval<Ts>()...));

        };
    }



    namespace impl {

        template<class T>
        struct is_repeat : mp_false {};

        template<class T, size_t _min, size_t _max>
        struct is_repeat<repeat<T, _min, _max>> : mp_true {};

        template<class T>
        struct remove_repeat {
            using type = T;
        };

        template<class T, size_t _min, size_t _max>
        struct remove_repeat<repeat<T, _min, _max>> {
            using type = T;
        };

        template<class T>
        using remove_repeat_t = typename remove_repeat<T>::type;



        template<class T>
        struct is_cpp_defaulted_param : mp_false {};

        template<class T, DefaultCreator Init>
        struct is_cpp_defaulted_param<cpp_defaulted_param<T, Init>> : mp_true {};

        template<class T>
        struct remove_cpp_defaulted_param {
            using type = T;
        };

        template<class T, DefaultCreator Init>
        struct remove_cpp_defaulted_param<cpp_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_cpp_defaulted_param_t = typename remove_cpp_defaulted_param<T>::type;


        template<class T>
        struct is_general_defaulted_param : mp_false {};

        template<class T, DefaultCreator Init>
        struct is_general_defaulted_param<general_defaulted_param<T, Init>> : mp_true {};

        template<class T>
        struct remove_general_defaulted_param {
            using type = T;
        };

        template<class T, DefaultCreator Init>
        struct remove_general_defaulted_param<general_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_general_defaulted_param_t = typename remove_general_defaulted_param<T>::type;


        template<class T>
        struct is_defaulted_param : mp_or< is_cpp_defaulted_param<T>, is_general_defaulted_param<T>> {};

        template<class T>
        struct is_not_defaulted_param : mp_not<is_defaulted_param<T>> {};

        template<class T>
        struct remove_defaulted_param {
            using type = T;
        };
        template<class T, DefaultCreator Init>
        struct remove_defaulted_param<cpp_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T, DefaultCreator Init>
        struct remove_defaulted_param<general_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_defaulted_param_t = typename remove_defaulted_param<T>::type;





        template<class T>
        struct get_init_type {
            using type = T;
        };

        template<class T, DefaultCreator Init>
        struct get_init_type<cpp_defaulted_param<T, Init>> {
            using type = Init;
        };
        template<class T, DefaultCreator Init>
        struct get_init_type<general_defaulted_param<T, Init>> {
            using type = Init;
        };
        template<class T>
        using get_init_type_t = typename get_init_type<T>::type;



        template<class T>
        struct is_tmpl : mp_false {};

        template<class T>
        struct is_tmpl<tmpl_q<T>> : mp_true {};



        template<class T>
        struct get_tmpl {
            using type = T;
        };

        template<class T>
        struct get_tmpl<tmpl_q<T>> {
            using type = T;
        };





        template<class T, class Arg>
        struct unpack_replace_tmpl {
            using type = T;
        };

        template<class T, class Arg>
        struct unpack_replace_tmpl<tmpl_q<T>, Arg> {
            using type = mp_invoke_q<T, Arg>;
        };


        template<class _Tag, class _Ty, class Arg>
        struct unpack_replace_tmpl<tagged_ty<_Tag, _Ty>, Arg> {
            using type = typename unpack_replace_tmpl<_Ty, Arg>::type;
        };



        // helper types for default-param

        template<class _Ty, bool _specified, bool _general>
        struct defaulted_type_t {
            using type = _Ty;
            static constexpr bool specified = _specified;
            static constexpr bool general = _general;
        };



        template<class _Ty>
        struct is_default_specified : mp_false {};

        template<class _Ty, bool _specified, bool _general>
        struct is_default_specified<defaulted_type_t<_Ty, _specified, _general>> :
            mp_bool<_specified && !_general> {};

        template<class _Ty>
        using is_default_specified_t = typename is_default_specified<_Ty>::type;



        template<class _Ty>
        struct is_defaulted_type : mp_false {};

        template<class _Ty, bool _specified, bool _general>
        struct is_defaulted_type<defaulted_type_t<_Ty, _specified, _general>> : mp_true {};

        template<class _Ty>
        using is_defaulted_type_t = typename is_defaulted_type<_Ty>::type;


        template<class T>
        struct is_seq : mp_false {};

        template<class... Ts>
        struct is_seq<seq<Ts...>> : mp_true {};


        template<class T>
        struct is_alt : mp_false {};

        template<class... Ts>
        struct is_alt<alt<Ts...>> : mp_true {};


        template<class T>
        struct is_gather : mp_false {};

        template<class _Tag, class _Ty>
        struct is_gather<gather<_Tag, _Ty>> : mp_true {};


        template<class _Ty>
        struct is_hop_type_builder : mp_or<
            is_repeat<_Ty>,
            is_defaulted_param<_Ty>,
            is_general_defaulted_param<_Ty>,
            is_seq<_Ty>,
            is_alt<_Ty>,
            is_gather<_Ty>
        > {};

        template<class _Tag, class _Ty>
        struct is_hop_type_builder<tagged_ty<_Tag, _Ty>> : is_hop_type_builder<_Ty> {};




        template<class Arg>
        struct any_deduction_pattern {
            template<class... Ts>
            using fn = Arg&&;
        };


        template<class T, class Arg>
        struct make_deduction_pattern {
            using type = any_deduction_pattern<Arg>;
        };

        template<template<class...> class Pattern, class Arg>
        struct make_deduction_pattern<tmpl_q<deduction_pattern<Pattern>>, Arg> {
            using type = typename deduction_pattern<Pattern>:: template pattern<Arg>;
        };

        template<class _Tag, class _Ty, class Arg>
        struct make_deduction_pattern<tagged_ty<_Tag, _Ty>, Arg> : make_deduction_pattern<_Ty, Arg> {
            using type = typename make_deduction_pattern<_Ty, Arg>::type;
        };



        template<class _ActualTyList, class _FormalTyList>
        struct deduction_helper;

        template<class... _ActualTys, class... _FormalTys>
        struct deduction_helper<mp_list<_ActualTys...>, mp_list<_FormalTys...>> {

            static constexpr bool has_no_deduction_pattern = (std::is_same_v<typename make_deduction_pattern<_FormalTys, _ActualTys>::type, any_deduction_pattern<_ActualTys>> && ...);

            static constexpr bool value = has_no_deduction_pattern ||
                deducer_t<typename make_deduction_pattern<_FormalTys, _ActualTys>::type...>::template fn<_ActualTys...>::value;

            using deduced_t =
                mp_eval_if_c< has_no_deduction_pattern,
                mp_list<>,
                deducer_t<typename make_deduction_pattern<_FormalTys, _ActualTys>::type...>::template deduced,
                _ActualTys...
                >;
        };


        //////////////////////////////////////////////////////////////////////////////////////////////////
        // core template that generates the call-operator to test
        template <size_t _Idx, class _ExpandedTypes, class _ExpectedTypes, class _DefaultedTypes, class _Info, class _If, class _Gathered>
        struct _single_overload_helper;

        template <size_t _Idx, class... _ExpandedTys, class... _ExpectedTys, class... _DefaultedTys, class _Info, class _If, class _Gathered>
        struct _single_overload_helper<_Idx, mp_list<_ExpandedTys...>, mp_list<_ExpectedTys...>, mp_list<_DefaultedTys...>, _Info, _If, _Gathered> {

            using expanded_types = mp_list<_ExpandedTys...>;
            using expected_types = mp_list<_ExpectedTys...>;
            using defaulted_types = mp_list<_DefaultedTys...>;

            template<
                class... T,
                std::enable_if_t<
                mp_invoke_q<_If, T...>::value
                &&
                deduction_helper<mp_list<T...>, expanded_types>::value
                , int* > = nullptr
            >
                constexpr mp_list<
                _Info,                                                              //static constexpr size_t information_index = 0;
                std::integral_constant<size_t, _Idx>,                               //static constexpr size_t overload_index = 1;
                expected_types,                                                     //static constexpr size_t expected_parameter_overload_type_index = 2;        // the type-list of the selected overload WITH default-params
                defaulted_types,                                                    //static constexpr size_t default_info_type_index = 3;        // the original secification of the selected overload with default-info
                expanded_types,                                                     //static constexpr size_t actual_parameter_overload_type_index = 4;     // the type-list of the selected overload
                mp_list<T...>,                                                      //static constexpr size_t deduced_local_parameter_overload_type_index = 5;     // the local deduced types
                typename deduction_helper<mp_list<T...>, expanded_types>::deduced_t,//static constexpr size_t deduced_parameter_overload_type_index = 6;     // the (global) deduced types
                _Gathered                                                           //static constexpr size_t gathered_type_index = 7;     // type holding the gather-tagged inductive type
                > test(typename unpack_replace_tmpl<_ExpandedTys, T>::type...) const;
        };


        template <size_t _Idx, class _Ty>
        struct _single_overload;

        template <size_t _Idx, class... _ArgTys, class _Info, class _If, class _Gathered>
        struct _single_overload<_Idx, mp_list<mp_list<_ArgTys...>, _Info, _If, _Gathered>>
            :_single_overload_helper<
            _Idx,
            mp_flatten<mp_list<typename _ArgTys::expanded_type_list ...>>,
            mp_flatten<mp_list<typename _ArgTys::expected_type_list ...>>,
            mp_flatten<mp_list<typename _ArgTys::defaulted_type_list ...>>,
            _Info,
            _If, 
            _Gathered
            > {
        };



        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // calculation of the valid number-of-arguments

        struct tag_args_min;
        struct tag_args_max;

        template<class _Tag, class _Ty>
        struct _arg_count {
            static constexpr size_t value = 1;
        };


        template<class _Ty, size_t _min, size_t _max>
        struct _arg_count<tag_args_min, repeat<_Ty, _min, _max>> {
            static constexpr size_t value = _min * _arg_count<tag_args_min, _Ty>::value;
        };
        template<class _Ty, size_t _min, size_t _max>
        struct _arg_count<tag_args_max, repeat<_Ty, _min, _max>> {
            static constexpr size_t value = _max * _arg_count<tag_args_max, _Ty>::value;
        };


        template<class _Ty, DefaultCreator _Init>
        struct _arg_count<tag_args_min, cpp_defaulted_param<_Ty, _Init>> {
            static constexpr size_t value = 0;
        };

        template<class _Ty, DefaultCreator _Init>
        struct _arg_count<tag_args_min, general_defaulted_param<_Ty, _Init>> {
            static constexpr size_t value = 0;
        };


        template<class _Tag, class... _Tys>
        struct _arg_count<_Tag, seq<_Tys...>> {
            static constexpr size_t value = (_arg_count<_Tag, _Tys>::value + ... + 0);
        };

        template<class... _Tys>
        struct _arg_count<tag_args_min, alt<_Tys...>> {
            static constexpr size_t value = std::min({ _arg_count<tag_args_min, _Tys>::value... });
        };

        template<class... _Tys>
        struct _arg_count<tag_args_max, alt<_Tys...>> {
            static constexpr size_t value = std::max({ _arg_count<tag_args_max, _Tys>::value... });
        };

        template<class _Tag, class _TyTag, class _Ty>
        struct _arg_count<_Tag, gather<_TyTag, _Ty>> {
            static constexpr size_t value = _arg_count<_Tag, _Ty>::value;
        };


        constexpr size_t add_inifinite_aware(size_t lhs, size_t rhs) {
            return lhs == infinite || rhs == infinite
                ? infinite
                : (lhs + rhs < lhs) // overflow ?
                ? infinite
                : lhs + rhs
                ;
        }

        constexpr size_t minus_non_negative(size_t lhs, size_t rhs) {
            return rhs >= lhs
                ? 0
                : lhs == infinite
                ? infinite
                : lhs - rhs
                ;
        }


        struct _add_args {
            template<class _Ty>
            static constexpr size_t arg_min = _arg_count<tag_args_min, _Ty>::value;
            template<class _Ty>
            static constexpr size_t arg_max = _arg_count<tag_args_max, _Ty>::value;

            template<class _Front, class _Ty>
            using add_min_max = mp_list_c<size_t,
                mp_at_c<_Front, 4>::value,
                mp_at_c<_Front, 5>::value,
                arg_min<_Ty>,
                arg_max<_Ty>,
                add_inifinite_aware(mp_at_c<_Front, 4>::value, arg_min<_Ty>),
                add_inifinite_aware(mp_at_c<_Front, 5>::value, arg_max<_Ty>)
            >;


            template<class _Ty, class _Value>
            using fn = mp_push_front<_Value, add_min_max<mp_front<_Value>, _Ty>>;
        };


        template<class _Ty>
        using _minmax_args_count_list = mp_reverse_fold_q<_Ty, mp_list<mp_list_c<size_t, 0, 0, 0, 0, 0, 0>>, _add_args>;


        // forward declared
        template <size_t _arg_count, class _Ty, class minmax_args_count_list, bool seen_defaulted>
        struct _expand_overload_set_helper;
        template <size_t _arg_count, class _Ty>
        struct _expand_overload_set_impl;

        // helper-template to store argument-type info
        template <class _Expanded, class _Expected, class _Defaulted>
        struct argument_t {
            using expanded_type_list = _Expanded;        // type validated against actual paramters
            using expected_type_list = _Expected;        // type expected inside the function
            using defaulted_type_list = _Defaulted;      // kind of defaulted type
        };


        template <size_t _arg_count, class _Ty, class Enable = void>
        struct _expand_current {
            static_assert(_arg_count == 1);

            using argument_type = argument_t<mp_list<_Ty>, mp_list<_Ty>, mp_list<_Ty>>;
            using single_type = mp_list<argument_type>;                 // generated argument-list
            using type = mp_list<single_type>;                          // generated argument-lists
        };

#if defined(HOP_ENABLE_REPEAT_OPTIMIZATION)
        // speed-up the simple cases
        template<size_t _arg_count, class _Ty, size_t _min, size_t _max>
        struct _expand_current<_arg_count, repeat<_Ty, _min, _max>, typename std::enable_if<!is_hop_type_builder< _Ty>::value>::type> {
            static_assert(_arg_count >= _min && _arg_count <= _max);

            using argument_type = argument_t< mp_repeat_c<mp_list<_Ty>, _arg_count>, mp_repeat_c<mp_list<_Ty>, _arg_count>, mp_repeat_c<mp_list<_Ty>, _arg_count>>;
            using single_type = mp_list<argument_type>;    // generated argument-list
            using type = mp_list<single_type>;                                      // generated argument-lists
        };
#endif


        // general case for 0
        template<class _Ty, size_t _min, size_t _max>
        struct _expand_current<0, repeat<_Ty, _min, _max>
#if defined(HOP_ENABLE_REPEAT_OPTIMIZATION)
            , typename std::enable_if<is_hop_type_builder< _Ty>::value>::type
#endif
        > {
            static constexpr size_t _arg_count = 0;

            static_assert(_arg_count >= _min && _arg_count <= _max);

            using type = mp_list<mp_list<>>;                                      // generated argument-lists
        };



        // induction case for i > 0
        template<size_t _arg_count, class _Ty, size_t _min, size_t _max>
        struct _expand_current<_arg_count, repeat<_Ty, _min, _max>
#if defined(HOP_ENABLE_REPEAT_OPTIMIZATION)
            , typename std::enable_if<is_hop_type_builder< _Ty>::value>::type
#endif
        > {
            static_assert(_arg_count >= _min && _arg_count <= _max);

            // multiple first expansion(s) with the expansion(s) of the rest
            using minmax_args_count_list = _minmax_args_count_list<mp_list<_Ty>>;
            using _minmax = mp_front< minmax_args_count_list>;
            static constexpr size_t _min_rest = mp_front<_minmax>::value;
            static constexpr size_t _max_rest = mp_second<_minmax>::value;
            static constexpr size_t _min_ = mp_at_c<_minmax, 2>::value;
            static constexpr size_t _max_ = /*seen_defaulted*/ false
                ? 0 // no further args after a daulfted
                : mp_at_c<_minmax, 3>::value;

            static_assert(_min_rest == 0, "_Ty is not is single entity");
            static_assert(_max_rest == 0, "_Ty is not is single entity");
            static_assert(_min_ > 0, "at least one argument has to be consumed by _Ty, otherwise it's ambigous");

            //            static constexpr size_t _lower_bound = std::max(_min_, minus_non_negative(_arg_count, _max_rest));
            static constexpr size_t _lower_bound = _min_;  // at least one argument has to be consumed
            static constexpr size_t _upper_bound = std::min(_max_, minus_non_negative(_arg_count, _min_rest));

            // drop funktioniert nicht, wenn mehr als der Inhalt gedroppt wird
            using valid_arg_counts = mp_drop_c<
                mp_iota_c<_upper_bound + 1>,
                std::min(_lower_bound, _upper_bound + 1)
            >;

            template<size_t args_used>
            using first_type = typename _expand_overload_set_impl< args_used, mp_list<_Ty>>::type;


            template<size_t args_used>
            using rest_type = typename _expand_overload_set_impl<
                _arg_count - args_used, mp_list<repeat<_Ty, minus_non_negative(_min, 1), minus_non_negative(_max, 1)>>
            >::type;

            // for each args_used build product of first_type x rest_type
            // append all of them

            struct build_product {

                template<class T>
                using fn =
                    mp_product<mp_append, first_type<T::value>, rest_type<T::value>>;
            };


            // the result
            using type =
                mp_flatten <
                mp_transform_q<
                build_product,
                valid_arg_counts
                >>;
        };

        template <size_t _arg_count, class _Ty, DefaultCreator _Init>
        struct _expand_current<_arg_count, cpp_defaulted_param<_Ty, _Init>> {
            static_assert(_arg_count <= 1);

            using argument_type = argument_t<
                mp_repeat_c<mp_list<_Ty>, _arg_count>,
                mp_if_c< _arg_count == 1,
                mp_list<_Ty>,
                mp_list<cpp_defaulted_param<_Ty, _Init>>
                >,
                mp_list<defaulted_type_t<cpp_defaulted_param<_Ty, _Init>, _arg_count == 1, false>>
            >;
            using single_type = mp_list<argument_type>;     // generated argument-list
            using type = mp_list<single_type>;              // generated argument-lists
        };

        template <size_t _arg_count, class _Ty, DefaultCreator _Init>
        struct _expand_current<_arg_count, general_defaulted_param<_Ty, _Init>> {
            static_assert(_arg_count <= 1);

            using argument_type = argument_t<
                mp_repeat_c<mp_list<_Ty>, _arg_count>,
                mp_if_c< _arg_count == 1,
                mp_list<_Ty>,
                mp_list<general_defaulted_param<_Ty, _Init>>
                >,
                mp_list<defaulted_type_t<general_defaulted_param<_Ty, _Init>, _arg_count == 1, true>>
            >;
            using single_type = mp_list<argument_type>;     // generated argument-list
            using type = mp_list<single_type>;              // generated argument-lists
        };



        template<size_t _arg_count, class... _Tys>
        struct _expand_current<_arg_count, seq<_Tys...>> {

            using type = typename _expand_overload_set_impl<_arg_count, mp_list<_Tys...>>::type;
        };




        template<size_t _arg_count, class... _Tys>
        struct _expand_current<_arg_count, alt<_Tys...>> {

            struct build_alt {

                template<class T>
                using fn = typename _expand_overload_set_impl< _arg_count, mp_list<T>>::type;
            };


            // the result
            using type_list =
                mp_transform_q<
                build_alt,
                mp_list<_Tys...>
                >;

            using type = mp_fold<type_list, mp_list<>, mp_append>;
        };




        template<size_t _arg_count, class _Tag, class _Ty>
        struct _expand_current<_arg_count, gather<_Tag, _Ty>> {

            // gather must be inside mp_list, otherwise it's removed by mp_product-mp_flatten
            template<class T> using add_gather = mp_list<gather<_Tag, T>>;

            using type =
                mp_transform<
                add_gather,
                typename _expand_current<_arg_count, _Ty>::type
                >;
        };

        template <size_t _arg_count, class _Ty>
        struct is_cpp_defaulted_param_used : mp_false {};

        template <class _Ty, DefaultCreator _Init>
        struct is_cpp_defaulted_param_used<0, cpp_defaulted_param<_Ty, _Init>> : mp_true {};



        template <size_t _arg_count, class minmax_args_count_list, bool seen_defaulted>
        struct _expand_overload_set_helper<_arg_count, mp_list<>, minmax_args_count_list, seen_defaulted> {
            using type =
                mp_if_c<_arg_count == 0
                , mp_list<mp_list<>>
                , mp_list<>
                >;
        };

        template <size_t _arg_count, class minmax_args_count_list, bool seen_defaulted, class _Ty, class... _TyRest>
        struct _expand_overload_set_helper<_arg_count, mp_list<_Ty, _TyRest...>, minmax_args_count_list, seen_defaulted> {

            using _minmax = mp_front< minmax_args_count_list>;
            static constexpr size_t _min_rest = mp_front<_minmax>::value;
            static constexpr size_t _max_rest = mp_second<_minmax>::value;
            static constexpr size_t _min = mp_at_c<_minmax, 2>::value;
            static constexpr size_t _max = seen_defaulted
                ? 0 // no further args after a daulfted
                : mp_at_c<_minmax, 3>::value;

            static constexpr size_t _lower_bound = std::max(_min, minus_non_negative(_arg_count, _max_rest));
            static constexpr size_t _upper_bound = std::min(_max, minus_non_negative(_arg_count, _min_rest));

            // drop funktioniert nicht, wenn mehr als der Inhalt gedroppt wird
            using valid_arg_counts = mp_drop_c<
                mp_iota_c<_upper_bound + 1>,
                std::min(_lower_bound, _upper_bound + 1)
            >;

            template<size_t args_used>
            using rest_type = typename _expand_overload_set_helper<
                _arg_count - args_used, mp_list<_TyRest...>,
                mp_rest<minmax_args_count_list>,
                seen_defaulted || is_cpp_defaulted_param_used<args_used, _Ty>::value
            >::type;


            template<size_t args_used>
            using current_t = _expand_current< args_used, _Ty>;

            struct prepend_current_t {
                template<class T>
                using fn =
                    mp_transform_q<
                    mp_bind_back<
                    mp_push_front,
                    current_t<T::value>
                    >,
                    rest_type<T::value>
                    >;
            };


            // the result
            using type_list =
                mp_transform_q<
                prepend_current_t,
                valid_arg_counts
                >;


            using type = mp_fold<type_list, mp_list<>, mp_append>;
        };

        // _expand_overload_set_impl expands a single overload-entry (_TyIf) into (at least) all valid param-sets
        template <size_t _arg_count, class _Ty>
        struct _expand_overload_set_impl {
            using minmax_args_count_list = _minmax_args_count_list<_Ty>;
            using types_unexpanded = typename _expand_overload_set_helper< _arg_count, _Ty, minmax_args_count_list, false>::type;


            template<class _Value, class _Unexpanded>
            using append_expanded =
                mp_product<mp_append, _Value, typename _Unexpanded::type>
                ;

            template<class _Unexpanded>
            using expand_list =
                mp_fold<
                _Unexpanded,
                mp_list<mp_list<>>,
                append_expanded
                >;

            using type =
                mp_flatten<
                mp_transform<
                expand_list,
                types_unexpanded
                >>;
        };


        // _expand_overload_set expands a single overload-entry (_TyIf) into (at least) all valid param-sets
        template <size_t _arg_count, class _TyIf>
        struct _expand_overload_set {
            using _Ty = mp_first< _TyIf>;	// split from global enable_if condition etc.
            using type_no_If = typename _expand_overload_set_impl<_arg_count, _Ty>::type;

#if 0
            template<class T>
            struct degather {
                using type = T;
            };

            template<class... _Tys>
            struct degather<mp_list<_Tys...>> {
                using type = mp_list< typename degather<_Tys>::type...>;
            };

            template<class _Tag, class... _Tys>
            struct degather<gather<_Tag, mp_list<_Tys...>>> {
                using type = mp_list< typename degather<_Tys>::type...>;
            };
#else
            template <class Acc, class Input>
            struct degather_helper;

            template <class... Ts, class _Tag, class... _Tys, class... Tail>
            struct degather_helper<mp_list<Ts...>, mp_list<gather<_Tag, mp_list<_Tys...>>, Tail...>> {
                using type = typename degather_helper<mp_list<Ts..., _Tys...>, mp_list<Tail...>>::type;
            };
            template <class... Ts, class Head, class... Tail>
            struct degather_helper<mp_list<Ts...>, mp_list<Head, Tail...>> {
                using argument_t_check = typename Head::defaulted_type_list; // Head should be an argument_t

                using type = typename degather_helper<mp_list<Ts..., Head>, mp_list<Tail...>>::type;
            };
            template <class... Ts>
            struct degather_helper<mp_list<Ts...>, mp_list<>> {
                using type = mp_list<Ts...>;
            };

            template<class T>
            struct degather {
                using type = typename degather_helper<mp_list<>, T>::type;
            };
#endif

            template<class T>
            using degathered_t = typename degather<T>::type;

            template<class T> using rejoin_If_t =
                mp_append<mp_list<degathered_t<T>>, mp_rest< _TyIf>, mp_list<T>>;

            using type =
                mp_transform<
                rejoin_If_t,
                type_no_If
                >;
        };



        template <size_t _arg_count, size_t _Idx, class _Types_list>
        struct _expanded_overload_set_;

        template <size_t _arg_count, size_t _Idx, class... _TypeIf_list>
        struct _expanded_overload_set_<_arg_count, _Idx, mp_list<_TypeIf_list...>>
            : _single_overload<_Idx, _TypeIf_list>... {
            using _single_overload<_Idx, _TypeIf_list>::test...;
        };


        template <size_t _arg_count, size_t _Idx, class _TypeIfs_list>
        struct _expanded_overload_set;

        template <size_t _arg_count, size_t _Idx, class... _TypeIf_list>
        struct _expanded_overload_set<_arg_count, _Idx, mp_list<_TypeIf_list...>>
            : _expanded_overload_set_<_arg_count, _Idx, mp_list<_TypeIf_list...>> {
        };


        // specialization to avoid compilation errors for "using ...::test" when no overload is generated
        // eventually this will result in an error, but it shall be a "no matching overload found"
        template <size_t _arg_count, size_t _Idx>
        struct _expanded_overload_set<_arg_count, _Idx, mp_list<>> {
            template<class... T, std::enable_if_t<dependent_false<T...>::value, int* > = nullptr >
            constexpr void test() const;
        };



        // _overload_set expands all overload-entries (_Types) into (at least) all valid param-sets
        //	size_t _arg_count: numer of actual arguments
        //  size_t... _Indices: index sequence indexing overloads in overload_set
        // _Types...: mp_list<mp_list< func-types >, quoted enable-if>

        template <size_t _arg_count, class _Indices, class... _Types>
        struct _overload_set;

        template <size_t _arg_count, size_t... _Indices, class... _Types>
        struct _overload_set<_arg_count, std::index_sequence<_Indices...>, _Types...>
            : _expanded_overload_set<_arg_count, _Indices, typename _expand_overload_set<_arg_count, _Types>::type>... {
            using _expanded_overload_set<_arg_count, _Indices, typename _expand_overload_set<_arg_count, _Types>::type>::test...;
        };






        // information type, contains:
        // user-tag
        // is_from_base information
        struct not_an_adapter {
            static constexpr bool forward{};    // definition needed to satisfy concept "Adapted"
        };

        template<class _OL, class _Tag, class _Is_from_base, Adapted _Adapter>
        struct information_t {
            using overload_t = _OL;
            using tag_t = _Tag;
            using is_from_base_t = _Is_from_base;
            using adapter_t = _Adapter;
        };

        using is_from_base = mp_true;
        using is_not_from_base = mp_false;


        // mp_list<mp_list<_Ty...>, impl::information_t<_Tag, mp_false>, mp_quote<_If>>;
        template<class _Ty>
        using make_information_from_base_t = information_t<
            typename _Ty::overload_t,
            typename _Ty::tag_t,
            is_from_base,
            typename _Ty::adapter_t
        >;

        template<class _Ty>
        using make_ol_from_base_t =
            mp_replace_second<_Ty, make_information_from_base_t<mp_second<_Ty>>>;

    }


    template <class _Overload_Type_set, size_t _arg_count>
    struct overload_set;

    template <class... _Overload_Type_set, size_t _arg_count>
    struct overload_set<mp_list<_Overload_Type_set...>, _arg_count> : impl::_overload_set<_arg_count, std::index_sequence_for<_Overload_Type_set...>, _Overload_Type_set...> {};



    // helper for inspecting current overload
    static constexpr size_t information_index = 0;
    static constexpr size_t overload_index = 1;
    static constexpr size_t expected_parameter_overload_type_index = 2;        // the type-list of the selected overload WITH default-params
    static constexpr size_t default_info_type_index = 3;        // the original secification of the selected overload with default-info
    static constexpr size_t actual_parameter_overload_type_index = 4;     // the type-list of the selected overload
    static constexpr size_t deduced_local_parameter_overload_type_index = 5;     // the local deduced types
    static constexpr size_t deduced_parameter_overload_type_index = 6;     // the (global) deduced types
    static constexpr size_t gathered_type_index = 7;        // type holding the gather-tagged inductive type

    template<class _Overload>
    using get_tag_type = typename mp_at_c<_Overload, information_index>::tag_t;

    template<class _Overload, class _Tag>
    using has_tag = std::is_same<get_tag_type<_Overload>, _Tag>;

    template<class _Overload, class _Tag>
    static constexpr bool has_tag_v = has_tag<_Overload, _Tag>::value;


    template<class _Overload>
    using is_from_base = typename mp_at_c<_Overload, information_index>::is_from_base_t;

    template<class _Overload>
    static constexpr bool is_from_base_v = is_from_base<_Overload>::value;

    template<class _Overload>
    using is_adapted = mp_not<std::is_same<typename mp_at_c<_Overload, information_index>::adapter_t, impl::not_an_adapter>>;

    template<class _Overload>
    static constexpr bool is_adapted_v = is_adapted<_Overload>::value;

    template<class _Overload, class... _Ts>
    decltype(auto) forward_adapted(_Ts&&... ts) {
        return mp_at_c<_Overload, information_index>::adapter_t::forward(std::forward<_Ts>(ts)...);
    }

    template<class _Overload>
    using get_overload_set_type = typename mp_at_c<_Overload, information_index>::overload_t;


    template<class _Overload>
    using index = mp_at_c<_Overload, overload_index>;


    template<class _Overload>
    using defaults_specified =
        mp_count_if<
        mp_at_c<_Overload, default_info_type_index>,
        impl::is_default_specified_t
        >;

    template<class _Overload>
    using actual_parameter_overload_type = mp_at_c<_Overload, actual_parameter_overload_type_index>;

    template<class _Overload>
    using expected_parameter_overload_type = mp_at_c<_Overload, expected_parameter_overload_type_index>;

    template<class _Overload>
    using deduced_local_parameter_overload_type = mp_at_c<_Overload, deduced_local_parameter_overload_type_index>;

    template<class _Overload>
    using deduced_parameter_overload_type = mp_at_c<_Overload, deduced_parameter_overload_type_index>;

    template<class _Overload>
    using gathered_type = mp_at_c<_Overload, gathered_type_index>;




    template<size_t Index, class... Ts>
    constexpr decltype(auto) get_arg_at(Ts&&... ts) {
        return fused::nth<Index>(std::forward<Ts>(ts)...);
    }

    // this is ONLY for C++-style default params (NOT for general_defaulted_param)
    template<class _Overload, size_t _DefaultIdx>
    constexpr decltype(auto) get_cpp_defaulted_param() {
        using _Ty = get_overload_set_type<_Overload>;
        constexpr auto begin_cpp_defaulted_param = mp_find_if<_Ty, impl::is_cpp_defaulted_param>::value;

        return impl::get_init_type_t<mp_at_c<_Ty, begin_cpp_defaulted_param + _DefaultIdx>>{}();
    }


    template<class _Overload, size_t _DefaultIdx, class... Ts>
    constexpr decltype(auto) get_indexed_defaulted(Ts&&... ts) {
        using _Ty = get_overload_set_type<_Overload>;
        constexpr auto begin_cpp_defaulted_param = mp_find_if<_Ty, impl::is_cpp_defaulted_param>::value;
        if constexpr (sizeof...(Ts) <= begin_cpp_defaulted_param + _DefaultIdx) {
            return get_cpp_defaulted_param<_Overload, _DefaultIdx>();
        } else {
            return fused::nth<begin_cpp_defaulted_param + _DefaultIdx>(std::forward<Ts>(ts)...);
        }
    }

    namespace impl {
        struct not_a_tag;

        template<class _Test, class T>
        struct has_tag_impl :std::false_type{};

        template<class _Test, class _Tag, class _Ty>
        struct has_tag_impl<_Test, tagged_ty<_Tag, _Ty>>:std::disjunction< std::is_same<_Test, _Tag>, has_tag_impl<_Test, _Ty>>{};

        template<class _Test, class _Ty, size_t _min, size_t _max>
        struct has_tag_impl<_Test, repeat<_Ty, _min, _max>>:has_tag_impl<_Test, _Ty> {};

        template<class _Test, class _Ty, DefaultCreator _Init>
        struct has_tag_impl<_Test, cpp_defaulted_param<_Ty, _Init>> :has_tag_impl<_Test, _Ty> {};

        template<class _Test, class _Ty, DefaultCreator _Init>
        struct has_tag_impl<_Test, general_defaulted_param<_Ty, _Init>> :has_tag_impl<_Test, _Ty> {};


        // specialization used in get_tagged_arg_or
        template<class _Test, class _Ty, bool _specified, bool _general>
        struct has_tag_impl<_Test, defaulted_type_t<_Ty, _specified, _general>> :has_tag_impl<_Test, _Ty> {};



        template<class _Tag>
        struct has_tag {
            template<class T>
            using fn = has_tag_impl<_Tag, T>;
        };

        template<class _Tag>
        struct first_has_tag {
            template<class T>
            using fn = has_tag_impl<_Tag, mp_first<T>>;
        };


        struct first_is_not_defaulted_param {
            template<class T>
            using fn = is_not_defaulted_param<mp_first<T>>;
        };



        enum class or_behaviour {
            is_a_value,
            is_a_callable,
            result_in_compilation_error
        };

        template<typename V, typename T>
        struct append_actual_index {

            using elem = mp_list< T,
                mp_if<
                is_defaulted_param<T>,
                not_a_tag,
                mp_count_if_q<V, first_is_not_defaulted_param>
                >
            >;
            using type = mp_push_back<V, elem>;
        };
        template<typename V, typename T>
        using append_actual_index_t = typename append_actual_index<V, T>::type;

        // get_tagged_arg_or will always go for the first type with a matching tag
        template<class _Overload, class _Tag, size_t tag_index, or_behaviour or_behaviour_, class _Or, class... Ts>
        constexpr decltype(auto) get_tagged_arg_or(_Or&& _or, Ts&&... ts) {
            using expected_types = expected_parameter_overload_type<_Overload>;
            using expected_types_with_actual_index = mp_fold< expected_types, mp_list<>, append_actual_index_t>;

            using tag_filtered_expected_types = mp_copy_if_q<expected_types_with_actual_index, impl::first_has_tag<_Tag>>;

            if constexpr (mp_size<tag_filtered_expected_types>::value <= tag_index) {
                if constexpr (or_behaviour_ == or_behaviour::is_a_callable) {
                    return std::apply(std::forward<_Or>(_or));
                } else if constexpr (or_behaviour_ == or_behaviour::is_a_value) {
                    return std::forward<_Or>(_or);
                } else if constexpr (or_behaviour_ == or_behaviour::result_in_compilation_error) {
                    static_assert(dependent_false<_Overload>::value, "tag not found");
                } else {
                    static_assert(dependent_false<_Overload>::value, "hop internal error: missing case");
                }
            } else if constexpr (is_defaulted_param<mp_first<mp_at_c<tag_filtered_expected_types, tag_index>>>::value) {
                using defaulted_types = mp_copy_if<
                    mp_at_c<_Overload, default_info_type_index>,
                    impl::is_defaulted_type_t
                >;

                constexpr auto defaulted_tag_index = mp_find_if_q<defaulted_types, impl::has_tag<_Tag>>::value;
                constexpr auto defaulted_end = mp_size<defaulted_types>::value;

                //#define ENFORCE_MSVC_COMPILATION_ERROR
#ifdef ENFORCE_MSVC_COMPILATION_ERROR
                if constexpr (defaulted_tag_index < defaulted_end) {
#else
                if constexpr (defaulted_end > defaulted_tag_index) {
#endif
                    using defaulted_type = typename mp_at_c<defaulted_types, defaulted_tag_index>::type;
                    return  impl::get_init_type_t<defaulted_type>{}();
                } else {
                    if constexpr (or_behaviour_ == or_behaviour::is_a_callable) {
                        return std::apply(std::forward<_Or>(_or));
                    } else if constexpr (or_behaviour_ == or_behaviour::is_a_value) {
                        return std::forward<_Or>(_or);
                    } else if constexpr (or_behaviour_ == or_behaviour::result_in_compilation_error) {
                        static_assert(dependent_false<_Overload>::value, "tag not found");
                    } else {
                        static_assert(dependent_false<_Overload>::value, "hop internal error: missing case");
                    }
                }
            } else {
                return fused::nth<mp_second<mp_at_c<tag_filtered_expected_types, tag_index>>::value>(std::forward<Ts>(ts)...);
            }
        }
    }
    // get_tagged_arg_or_call will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class _FnOr, class... Ts>
    constexpr decltype(auto) get_tagged_arg_or_call(_FnOr&& _fnor, Ts&&... ts) {
        return impl::get_tagged_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::is_a_callable>(std::forward<_FnOr>(_fnor), std::forward<Ts>(ts)...);
    }

    // get_tagged_arg_or will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class _Or, class... Ts>
    constexpr decltype(auto) get_tagged_arg_or(_Or&& _or, Ts&&... ts) {
        return impl::get_tagged_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::is_a_value>(std::forward<_Or>(_or), std::forward<Ts>(ts)...);
    }


    // get_tagged_arg will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class... Ts>
    constexpr decltype(auto) get_tagged_arg(Ts&&... ts) {
        return impl::get_tagged_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::result_in_compilation_error>(0, std::forward<Ts>(ts)...);
    }


#if 0
    namespace impl {

        template<class _Overload, class _If, size_t index_specified, size_t index_expected>
        constexpr decltype(auto) get_args_if_helper() {
            using expected_types = expected_parameter_overload_type<_Overload>;

            if constexpr (mp_size<expected_types>::value > index_expected) {
                if constexpr (is_defaulted_param<mp_at_c<expected_types, index_expected>>::value) {
                    if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
                        return std::tuple_cat(
                            std::make_tuple(impl::get_init_type_t<mp_at_c<expected_types, index_expected>>{}()),
                            get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>()
                        );
                    } else {
                        return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>();
                    }
                } else {
                    static_assert(dependent_false<_Overload>::value);  // must be a defaulted_param
                    return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>();
                }
            } else {
                return std::tuple<>{};
            }
        }


        template<class _Overload, class _If, size_t index_specified, size_t index_expected, class T, class... Ts>
        constexpr decltype(auto) get_args_if_helper(T&& t, Ts&&... ts) {
            using expected_types = expected_parameter_overload_type<_Overload>;

            if constexpr (is_defaulted_param<mp_at_c<expected_types, index_expected>>::value) {
                if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
                    return std::tuple_cat(
                        std::make_tuple(impl::get_init_type_t<mp_at_c<expected_types, index_expected>>{}()),
                        get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<T>(t), std::forward<Ts>(ts)...)
                    );
                } else {
                    return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<T>(t), std::forward<Ts>(ts)...);
                }
            } else {
                if constexpr (_If::template fn<mp_at_c<actual_parameter_overload_type<_Overload>, index_specified>>::value) {
                    return std::tuple_cat(std::forward_as_tuple(std::forward<T>(t)), get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...));
                } else {
                    return get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...);
                }
            }
        }
    }

#else
    namespace impl {
        template<class _Overload, class _If, class _Gathered, size_t index_specified, size_t index_expected>
        struct get_args_if_helper_t;

        template<class _Overload, class _If, class _Gathered, size_t index_specified, size_t index_expected>
        constexpr decltype(auto) get_args_if_helper() {
            using expected_types = expected_parameter_overload_type<_Overload>;

            if constexpr (mp_size<expected_types>::value > index_expected) {
                if constexpr (is_defaulted_param<mp_at_c<expected_types, index_expected>>::value) {
                    if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
                        return std::tuple_cat(
                            std::make_tuple(impl::get_init_type_t<mp_at_c<expected_types, index_expected>>{}()),
                            get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>()
                        );
                    } else {
                        return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>();
                    }
                } else {
                    static_assert(dependent_false<_Overload>::value);  // must be a defaulted_param
                    return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>();
                }
            } else {
                return std::tuple<>{};
            }
        }


        template<class _Overload, class _If, class _Gathered, size_t index_specified, size_t index_expected, class T, class... Ts>
        constexpr decltype(auto) get_args_if_helper(T&& t, Ts&&... ts) {
            using expected_types = expected_parameter_overload_type<_Overload>;

            if constexpr (is_defaulted_param<mp_at_c<expected_types, index_expected>>::value) {
                if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
                    return std::tuple_cat(
                        std::make_tuple(impl::get_init_type_t<mp_at_c<expected_types, index_expected>>{}()),
                        get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<T>(t), std::forward<Ts>(ts)...)
                    );
                } else {
                    return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<T>(t), std::forward<Ts>(ts)...);
                }
            } else {
                if constexpr (_If::template fn<mp_at_c<actual_parameter_overload_type<_Overload>, index_specified>>::value) {
                    return std::tuple_cat(std::forward_as_tuple(std::forward<T>(t)), get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...));
                } else {
                    return get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...);
                }
            }
        }

// TODO: implement get_args_if
        template<class _Overload, class _If, size_t index_specified, size_t index_expected>
        struct get_args_if_helper_t<_Overload, _If, mp_list<>, index_specified, index_expected> {
            static constexpr decltype(auto) get_args_if() { // no args expected
                return std::tuple<>{};
            }
        };

        template<class _Overload, class _If, class Head, class... Tail, size_t index_specified, size_t index_expected>
        struct get_args_if_helper_t<_Overload, _If, mp_list<Head, Tail...>, index_specified, index_expected> {
            template<class... Ts>
            static constexpr decltype(auto) get_args_if(Ts&&... ts) {
                return std::tuple<>{};
            }
        };


        //template<class _Overload, class _If, class _Gathered, size_t index_specified, size_t index_expected, class T, class... Ts>
            //constexpr decltype(auto) get_args_if_helper(T && t, Ts &&... ts) {

    }
#endif

    template<class _Overload, class _If, class... Ts>
    constexpr decltype(auto) get_args_if_q(Ts&&... ts) {
     //   return impl::get_args_if_helper<_Overload, _If, 0, 0>(std::forward<Ts>(ts)...);
        return impl::get_args_if_helper_t<_Overload, _If, gathered_type<_Overload>, 0, 0>::get_args_if(std::forward<Ts>(ts)...);
    }

    template<class _Overload, template<class> class _If, class... Ts>
    constexpr decltype(auto) get_args_if(Ts&&... ts) {
        return get_args_if_q< _Overload, mp_quote<_If>>(std::forward<Ts>(ts)...);
    }

    template<class _Overload, class... Ts>
    constexpr decltype(auto) get_args(Ts&&... ts) {
        return get_args_if_q< _Overload, mp_quote<impl::true_t>>(std::forward<Ts>(ts)...);
    }

    template<class _Overload, class _Tag, class... Ts>
    constexpr decltype(auto) get_tagged_args(Ts&&... ts) {
        return get_args_if_q< _Overload, impl::has_tag<_Tag>>(std::forward<Ts>(ts)...);
    }



    namespace impl {

        template<class _Overload, class _If, size_t index_expected>
        constexpr size_t get_count_if_helper() {
            using expected_types = expected_parameter_overload_type<_Overload>;

            if constexpr (mp_size<expected_types>::value > index_expected) {
                if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
                    return 1 + get_count_if_helper<_Overload, _If, index_expected + 1>();
                } else {
                    return get_count_if_helper<_Overload, _If, index_expected + 1>();
                }
            } else {
                return 0;
            }
        }
    }


    template<class _Overload, class _If>
    constexpr size_t get_count_if_q() {
        return impl::get_count_if_helper<_Overload, _If, 0>();
    }

    template<class _Overload, template<class> class _If>
    constexpr size_t get_count_if() {
        return get_count_if_q< _Overload, mp_quote<_If>>();
    }

    template<class _Overload>
    constexpr size_t get_count() {
        return get_count_if_q< _Overload, mp_quote<impl::true_t>>();
    }

    template<class _Overload, class _Tag>
    constexpr size_t get_tagged_count() {
        return get_count_if_q< _Overload, impl::has_tag<_Tag>>();
    }




    template<class _Overload, size_t index>
    using deduced_local = mp_at_c<deduced_local_parameter_overload_type<_Overload>, index>;


    template<class _Overload, size_t index>
    using deduced_local_types =
        typename impl::get_tmpl<mp_at_c<actual_parameter_overload_type< _Overload>, index>>::type::_if::template deduced<deduced_local<_Overload, index>>;

    template<class _Overload>
    using deduced_types = deduced_parameter_overload_type<_Overload>;




    template<class... _Ty>
    using ol_list = mp_list<_Ty...>;

    template<class _Tag, class _If_q, class _Tys, class _Adapter = impl::not_an_adapter>
    using packed_ol = mp_list<_Tys, impl::information_t<_Tys, _Tag, mp_false, _Adapter>, _If_q>;


    template<class _Tag, template<class...> class _If, class... _Ty>
    using tagged_ol_if = packed_ol<_Tag, mp_quote<_If>, mp_list<_Ty...>>;

    template<template<class...> class _If, class... _Ty>
    using ol_if = tagged_ol_if<impl::none_tag, _If, _Ty...>;

    template<class _Tag, class... _Ty>
    using tagged_ol = tagged_ol_if<_Tag, impl::true_t, _Ty...>;

    template<class... _Ty>
    using ol = ol_if<impl::true_t, _Ty...>;

    template<class _Tag, class _If_q, class... _Ty>
    using tagged_ol_if_q = packed_ol<_Tag, _If_q, mp_list<_Ty...>>;



    template<class _Tag, class Adapter>
    using tagged_adapted = packed_ol<_Tag, impl::adapter_test<Adapter>, mp_list<pack<fwd>>, Adapter>;

    template<class Adapter>
    using adapted = tagged_adapted<impl::none_tag, Adapter>;


    template<auto f>
    using adapt = adapted<impl::adapter<f>>;

    template<class _Tag, auto f>
    using tagged_adapt = tagged_adapted<_Tag, impl::adapter<f>>;


    template<class _Overload_Type_set, typename... _Tys>
    constexpr decltype(overload_set<_Overload_Type_set, sizeof...(_Tys)>{}.template test<_Tys...>(std::declval<_Tys>()...)) enable();



    template<class _Base, class... _Ty>
    using ol_extend = mp_append<mp_list<_Ty...>, mp_transform<impl::make_ol_from_base_t, _Base>>;


}

#endif // HOP_HOP_HPP_INCLUDED
