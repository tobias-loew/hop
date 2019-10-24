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

#pragma once

#include <type_traits>
#include <algorithm>
#include <boost/mp11.hpp>


// homogeneous varaiadic overload sets
namespace hop {
    using namespace boost::mp11;

#ifdef _DEBUG
    template<class t>
    struct debug;
#endif

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
        decltype(auto) nth(Ts&& ... args) {
            return detail::at<N>{}(std::forward<Ts>(args)...);
        }
    }


    template<class... T> struct dependent_false : mp_false {};


    // template to create a repeated [min, .. , max] parameter
    static constexpr size_t infinite = std::numeric_limits<size_t>::max();

    template<class _Ty, size_t _min, size_t _max = infinite>
    struct repeat;

    // template to tag a type
    template<class _Tag, class _Ty>
    struct tagged_ty;


    // syntactic sugar

    // template to create an optional parameter (WITHOUT default value)
    template<class _Ty>
    using optional = repeat<_Ty, 0, 1>;

    // template to create a parameter pack
    template<class _Ty>
    using pack = repeat<_Ty, 0, infinite>;

    // template to create a non-empty parameter pack
    template<class _Ty>
    using non_empty_pack = repeat<_Ty, 1, infinite>;

    

    namespace impl {
        template<class _Ty>
        struct default_create {
            decltype(auto) operator()() const { return _Ty{}; }
        };
        template<class _Tag, class _Ty>
        struct default_create< tagged_ty<_Tag, _Ty>> {
            decltype(auto) operator()() const { return _Ty{}; }
        };
    }

    // template to create a parameter with default value (C++-style defaulted parameter, only at end of parameter list)
    template<class _Ty, class _Init = impl::default_create<_Ty>>
    struct cpp_defaulted_param;

    // template to create a parameter with default value (C++-style, only at end of parameter list)
    template<class _Ty, class _Init = impl::default_create<_Ty>>
    struct general_defaulted_param;

    // template to create a forwarded parameter: _Ty has to be a quoted meta-function
    template<class _Ty>
    struct tmpl;

    template<template<class...> class F>
    using tmpl_q = tmpl<mp_quote<F>>;

    // struct to create a forward-reference
    using fwd = tmpl_q<std::type_identity_t>;

    namespace impl {
        template<class _If>
        struct if_test {
        private:
            struct invalid_type_tag;

        public:
            template<class T>
            using fn = mp_if<
                typename _If::template fn<T>,
                T,
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
                T,
                invalid_type_tag
            >;

        };
    }


    // struct to create guarded forward-reference
    template<class _If>
    using fwd_if_q = tmpl<typename impl::if_test<_If>>;

    template<template<class> class _If>
    using fwd_if = fwd_if_q<mp_quote<_If>>;

    template<class _If>
    using fwd_if_not_q = tmpl<typename impl::if_not_test<_If>>;

    template<template<class> class _If>
    using fwd_if_not = fwd_if_not_q<mp_quote<_If>>;







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

        template<class T, class Init>
        struct is_cpp_defaulted_param<cpp_defaulted_param<T, Init>> : mp_true {};

        template<class T>
        struct remove_cpp_defaulted_param {
            using type = T;
        };

        template<class T, class Init>
        struct remove_cpp_defaulted_param<cpp_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_cpp_defaulted_param_t = typename remove_cpp_defaulted_param<T>::type;


        template<class T>
        struct is_general_defaulted_param : mp_false {};

        template<class T, class Init>
        struct is_general_defaulted_param<general_defaulted_param<T, Init>> : mp_true {};

        template<class T>
        struct remove_general_defaulted_param {
            using type = T;
        };

        template<class T, class Init>
        struct remove_general_defaulted_param<general_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_general_defaulted_param_t = typename remove_general_defaulted_param<T>::type;


        template<class T>
        struct is_defaulted_param : mp_or< is_cpp_defaulted_param<T>, is_general_defaulted_param<T>> {};

        template<class T>
        struct remove_defaulted_param {
            using type = T;
        };
        template<class T, class Init>
        struct remove_defaulted_param<cpp_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T, class Init>
        struct remove_defaulted_param<general_defaulted_param<T, Init>> {
            using type = T;
        };
        template<class T>
        using remove_defaulted_param_t = typename remove_defaulted_param<T>::type;





        template<class T>
        struct get_init_type {
            using type = T;
        };

        template<class T, class Init>
        struct get_init_type<cpp_defaulted_param<T, Init>> {
            using type = Init;
        };
        template<class T, class Init>
        struct get_init_type<general_defaulted_param<T, Init>> {
            using type = Init;
        };
        template<class T>
        using get_init_type_t = typename get_init_type<T>::type;



        template<class T>
        struct is_tmpl : mp_false {};

        template<class T>
        struct is_tmpl<tmpl<T>> : mp_true {};



        template<class T, class Arg>
        struct unpack_replace_tmpl {
            using type = T;
        };

        template<class T, class Arg>
        struct unpack_replace_tmpl<tmpl<T>, Arg> {
            using type = mp_invoke_q<T, Arg>;
        };


        template<class _Tag, class _Ty, class Arg>
        struct unpack_replace_tmpl<tagged_ty<_Tag, _Ty>, Arg> {
            using type = typename unpack_replace_tmpl<_Ty, Arg>::type;
        };





        // helper types for default-param
        struct not_defaulted_t;

        template<bool _specified, bool _general>
        struct defaulted_t {
            static constexpr bool specified = _specified;
            static constexpr bool general = _general;
        };

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


        //////////////////////////////////////////////////////////////////////////////////////////////////
        // core template that generates the call-operator to test
        template <size_t _Idx, class _Ty>
        struct _single_overload;

        template <size_t _Idx, class... _Tys, class _ActualTypes, class _DefaultedInfo, class _Info, class _If>
        struct _single_overload<_Idx, mp_list<mp_list<_Tys...>, _ActualTypes, _DefaultedInfo, _Info, _If>> {

            static_assert(mp_size<typename _Info::overload_t>::value ==
                mp_size<_DefaultedInfo>::value);


            template<class _Ty, class DefaultInfo>
            struct _add_default_info;

            template<class _Ty>
            struct _add_default_info<_Ty, not_defaulted_t> {
                using type = _Ty;
            };

            template<class _Ty, bool _specified, bool _general>
            struct _add_default_info<_Ty, defaulted_t<_specified, _general>> {
                using type = defaulted_type_t<_Ty, _specified, _general>;
            };

            template<class _Ty, class DefaultInfo>
            using add_default_info = typename _add_default_info<_Ty, DefaultInfo>::type;;

            using DefaultedTypeInfo = mp_transform<
                add_default_info,
                typename _Info::overload_t,
                _DefaultedInfo
            >;

            template<class... T, std::enable_if_t<mp_invoke_q<_If, T...>::value, int* > = nullptr >
            mp_list<_Info, std::integral_constant<size_t, _Idx>, _ActualTypes, DefaultedTypeInfo, mp_list<_Tys...>> test(typename unpack_replace_tmpl<_Tys, T>::type...) const;
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

            using _expanded_overload_set_<_arg_count, _Idx, mp_list<_TypeIf_list...>>::test;
        };


        // specialization to avoid compilation errors for "using ...::test" when no overload is generated
        // eventually this will result in an error, but it shall be a "no matching overload found"
        template <size_t _arg_count, size_t _Idx>
        struct _expanded_overload_set<_arg_count, _Idx, mp_list<>> {
            template<class... T, std::enable_if_t<dependent_false<T...>::value, int* > = nullptr >
            void test() const;
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
            static constexpr size_t value = _min;
        };
        template<class _Ty, size_t _min, size_t _max>
        struct _arg_count<tag_args_max, repeat<_Ty, _min, _max>> {
            static constexpr size_t value = _max;
        };


        template<class _Ty, class _Init>
        struct _arg_count<tag_args_min, cpp_defaulted_param<_Ty, _Init>> {
            static constexpr size_t value = 0;
        };

        template<class _Ty, class _Init>
        struct _arg_count<tag_args_min, general_defaulted_param<_Ty, _Init>> {
            static constexpr size_t value = 0;
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
            return rhs > lhs
                ? 0
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


        template <size_t _arg_count, class _Ty>
        struct _expand_current {
            static_assert(_arg_count == 1);
            using type = mp_list<_Ty>;          // type validated against actual paramters
            using expected_type = type;           // type with actual paramters AND default parameters (if not specified)

            using defaulted_type = not_defaulted_t;
        };


        template<size_t _arg_count, class _Ty, size_t _min, size_t _max>
        struct _expand_current<_arg_count, repeat<_Ty, _min, _max>> {
            static_assert(_arg_count >= _min && _arg_count <= _max);
            using type = mp_repeat_c<mp_list<_Ty>, _arg_count>;
            using expected_type = type;

            using defaulted_type = not_defaulted_t;
        };

        template <size_t _arg_count, class _Ty, class _Init>
        struct _expand_current<_arg_count, cpp_defaulted_param<_Ty, _Init>> {
            static_assert(_arg_count <= 1);
            using type = mp_repeat_c<mp_list<_Ty>, _arg_count>;
            using expected_type = mp_if_c< _arg_count == 1,
                type,
                mp_list<cpp_defaulted_param<_Ty, _Init>>
            >;

            using defaulted_type = defaulted_t<_arg_count == 1, false>;
        };

        template <size_t _arg_count, class _Ty, class _Init>
        struct _expand_current<_arg_count, general_defaulted_param<_Ty, _Init>> {
            static_assert(_arg_count <= 1);
            using type = mp_repeat_c<mp_list<_Ty>, _arg_count>;
            using expected_type = mp_if_c< _arg_count == 1,
                type,
                mp_list<general_defaulted_param<_Ty, _Init>>
            >;

            using defaulted_type = defaulted_t<_arg_count == 1, true>;
        };




        template <size_t _arg_count, class _Ty>
        struct is_cpp_defaulted_param_used : mp_false {};

        template <class _Ty, class _Init>
        struct is_cpp_defaulted_param_used<0, cpp_defaulted_param<_Ty, _Init>> : mp_true {};




        template <size_t _arg_count, class _Ty, class minmax_args_count_list, bool seen_defaulted>
        struct _expand_overload_set_helper;

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

        // _expand_overload_set expands a single overload-entry (_TyIf) into (at least) all valid param-sets
        template <size_t _arg_count, class _TyIf>
        struct _expand_overload_set {
            using _Ty = mp_first< _TyIf>;	// split from global enable_if condition etc.

            using minmax_args_count_list = _minmax_args_count_list<_Ty>;

            using types_no_If_unexpanded = typename _expand_overload_set_helper< _arg_count, _Ty, minmax_args_count_list, false>::type;


            template<class _Value, class _Unexpanded>
            using append_expanded =
                mp_list<
                mp_append<mp_first<_Value>, typename _Unexpanded::type>,
                mp_append<mp_second<_Value>, typename _Unexpanded::expected_type>,
                mp_push_back<mp_third<_Value>, typename _Unexpanded::defaulted_type>
                >;

            template<class _Unexpanded>
            using expand_list =
                mp_fold<
                _Unexpanded,
                mp_list<mp_list<>, mp_list<>, mp_list<>>,
                append_expanded
                >;

            using type_no_If =
                mp_transform<
                expand_list,
                types_no_If_unexpanded
                >;


            template<class T> using rejoin_If_t =
                mp_append<T, mp_rest< _TyIf>>;

            using type =
                mp_transform<
                rejoin_If_t,
                type_no_If
                >;
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






        template<class... _Ty>
        using true_t = mp_true;

        struct none_tag {};


        // information type, contains:
        // user-tag
        // is_from_base information
        template<class _OL, class _Tag, class _Is_from_base>
        struct information_t {
            using overload_t = _OL;
            using tag_t = _Tag;
            using is_from_base_t = _Is_from_base;
        };

        using is_from_base = mp_true;
        using is_not_from_base = mp_false;


        // mp_list<mp_list<_Ty...>, impl::information_t<_Tag, mp_false>, mp_quote<_If>>;
        template<class _Ty>
        using make_information_from_base_t = information_t<typename _Ty::overload_t, typename _Ty::tag_t, is_from_base>;

        template<class _Ty>
        using make_ol_from_base_t =
            mp_append<
            mp_list<mp_first<_Ty>, make_information_from_base_t<mp_second<_Ty>>>
            , mp_drop_c<_Ty, 2>
            >;

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

    template<class _Overload, class _Tag>
    using has_tag = std::is_same<typename mp_at_c<_Overload, information_index>::tag_t, _Tag>;

    template<class _Overload, class _Tag>
    static constexpr bool has_tag_v = has_tag<_Overload, _Tag>::value;


    template<class _Overload>
    using is_from_base = typename mp_at_c<_Overload, information_index>::is_from_base_t;

    template<class _Overload>
    using get_overload_set_type = typename mp_at_c<_Overload, information_index>::overload_t;

    template<class _Overload>
    static constexpr bool is_from_base_v = is_from_base<_Overload>::value;


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

    

    // this is ONLY for C++-style default params (NOT for general_defaulted_param)
    template<class _Overload, size_t _DefaultIdx>
    decltype(auto) get_cpp_defaulted_param() {
        using _Ty = get_overload_set_type<_Overload>;
        static constexpr auto begin_cpp_defaulted_param = mp_find_if<_Ty, impl::is_cpp_defaulted_param>::value;

        return impl::get_init_type_t<mp_at_c<_Ty, begin_cpp_defaulted_param + _DefaultIdx>>{}();
    }


    template<class _Overload, size_t _DefaultIdx, class... Ts>
    decltype(auto) get_value_or_default(Ts&&... ts) {
        using _Ty = get_overload_set_type<_Overload>;
        static constexpr auto begin_cpp_defaulted_param = mp_find_if<_Ty, impl::is_cpp_defaulted_param>::value;
        if constexpr (sizeof...(Ts) <= begin_cpp_defaulted_param + _DefaultIdx) {
            return get_cpp_defaulted_param<_Overload, _DefaultIdx>();
        } else {
            return fused::nth<begin_cpp_defaulted_param + _DefaultIdx>(std::forward<Ts>(ts)...);
        }
    }

    namespace impl {
        struct not_a_tag;
        template<class T>
        struct get_tag {
            using type = not_a_tag;
        };

        template<class _Tag, class _Ty>
        struct get_tag<tagged_ty<_Tag, _Ty>> {
            using type = _Tag;
        };

        template<class _Ty, size_t _min, size_t _max>
        struct get_tag<repeat<_Ty, _min, _max>> {
            using type = typename get_tag<_Ty>::type;
        };

        template<class _Ty, class _Init>
        struct get_tag<cpp_defaulted_param<_Ty, _Init>> {
            using type = typename get_tag<_Ty>::type;
        };

        template<class _Ty, class _Init>
        struct get_tag<general_defaulted_param<_Ty, _Init>> {
            using type = typename get_tag<_Ty>::type;
        };


        // specialization used in get_value_or
        template<class _Ty, bool _specified, bool _general>
        struct get_tag<defaulted_type_t<_Ty, _specified, _general>> {
            using type = typename get_tag<_Ty>::type;
        };



        template<class _Tag>
        struct has_tag {
            template<class T>
            using fn = std::is_same<_Tag, typename get_tag<T>::type>;
        };

    }

    template<class _Overload, class _Tag, class _Or, class... Ts>
    decltype(auto) get_value_or(_Or&& _or, Ts&&... ts) {
        using _Ty = actual_parameter_overload_type<_Overload>;

        static constexpr auto tag_index = mp_find_if_q<_Ty, impl::has_tag<_Tag>>::value;
        if constexpr (sizeof...(Ts) <= tag_index) {
            // tag_index out-of-bounds -> value not specified, check if it is a defaulted-type


            using defaulted_types = mp_copy_if<
                mp_at_c<_Overload, default_info_type_index>,
                impl::is_defaulted_type_t
            >;


            static constexpr auto defaulted_tag_index = mp_find_if_q<defaulted_types, impl::has_tag<_Tag>>::value;
            static constexpr auto defaulted_end = mp_size<defaulted_types>::value;

//#define MSVC_COMPILATION_ERROR
#ifdef MSVC_COMPILATION_ERROR
            if constexpr (defaulted_tag_index < defaulted_end) {
#else
            if constexpr (defaulted_end > defaulted_tag_index) {
#endif
                using defaulted_type = typename mp_at_c<defaulted_types, defaulted_tag_index>::type;
                return  impl::get_init_type_t<defaulted_type>{}();
            } else {
                return std::forward<_Or>(_or);
            }
        } else {
            return fused::nth<tag_index>(std::forward<Ts>(ts)...);
        }
    }


    namespace impl {

        template<class _Overload, class _If, size_t index_specified, size_t index_expected>
        decltype(auto) get_args_if_helper() {
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
                    static_assert(dependent_false<_Overload>);  // must be a defaulted_param
                    return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>();
                }
            } else {
                return std::tuple<>{};
            }
        }


        template<class _Overload, class _If, size_t index_specified, size_t index_expected, class T, class... Ts>
        decltype(auto) get_args_if_helper(T&& t, Ts&&... ts) {
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





        //template<class _Overload, class _If, size_t index_specified, size_t index_expected, class... Ts>
        //decltype(auto) get_args_if_helper(Ts&&... ts) {
        //    using expected_types = expected_parameter_overload_type<_Overload>;

        //    if constexpr (is_defaulted_param<mp_at_c<expected_types, index_expected>>::value) {
        //        if constexpr (_If::template fn<mp_at_c<expected_types, index_expected>>::value) {
        //            return std::tuple_cat(
        //                std::make_tuple(impl::get_init_type_t<mp_at_c<expected_types, index_expected>>{}()),
        //                get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<Ts>(ts)...)
        //            );
        //        } else {
        //            return get_args_if_helper<_Overload, _If, index_specified, index_expected + 1>(std::forward<Ts>(ts)...);
        //        }
        //    } else {
        //        if constexpr (_If::template fn<mp_at_c<actual_parameter_overload_type<_Overload>, index_specified>>::value) {
        //            return std::tuple_cat(std::forward_as_tuple(std::forward<T>(t)), get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...));
        //        } else {
        //            return get_args_if_helper<_Overload, _If, index_specified + 1, index_expected + 1>(std::forward<Ts>(ts)...);
        //        }
        //    }
        //}

    }

    template<class _Overload, class _If, class... Ts>
    decltype(auto) get_args_if_q(Ts&&... ts) {
        return impl::get_args_if_helper<_Overload, _If, 0, 0>(std::forward<Ts>(ts)...);
    }

    template<class _Overload, template<class> class _If, class... Ts>
    decltype(auto) get_args_if(Ts&&... ts) {
        return get_args_if_q< _Overload, mp_quote<_If>>(std::forward<Ts>(ts)...);
    }

    template<class _Overload, class... Ts>
    decltype(auto) get_args(Ts&&... ts) {
        return get_args_if_q< _Overload, mp_quote<impl::true_t>>(std::forward<Ts>(ts)...);
    }

    template<class _Overload, class _Tag, class... Ts>
    decltype(auto) get_tagged_args(Ts&&... ts) {
        return get_args_if_q< _Overload, impl::has_tag<_Tag>>(std::forward<Ts>(ts)...);
    }



    template<class... _Ty>
    using ol_list = mp_list<_Ty...>;

    template<class _Tag, template<class...> class _If, class... _Ty>
    using tagged_ol_if = mp_list<mp_list<_Ty...>, impl::information_t<mp_list<_Ty...>, _Tag, mp_false>, mp_quote<_If>>;

    template<template<class...> class _If, class... _Ty>
    using ol_if = tagged_ol_if<impl::none_tag, _If, _Ty...>;

    template<class _Tag, class... _Ty>
    using tagged_ol = tagged_ol_if<_Tag, impl::true_t, _Ty...>;

    template<class... _Ty>
    using ol = ol_if<impl::true_t, _Ty...>;


    template<class _Overload_Type_set, typename... _Tys>
    decltype(overload_set<_Overload_Type_set, sizeof...(_Tys)>{}.test<_Tys...>(std::declval<_Tys>()...)) enable();



    template<class _Base, class... _Ty>
    using ol_extend = mp_append<mp_list<_Ty...>, mp_transform<impl::make_ol_from_base_t, _Base>>;
}

