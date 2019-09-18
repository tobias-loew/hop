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
#include <boost/mp11.hpp>


// homogeneous varaiadic overload sets
namespace hop {
	namespace mp11 = boost::mp11;

	// template to indicate a parameter pack
	template<typename _Ty>
	struct pack;

	namespace impl {
		template<class _Ty>
		struct default_create {
			decltype(auto) operator()() const { return _Ty{}; }
		};
	}

	// template to indicate a parameter with default value
	template<typename _Ty, typename _Init = impl::default_create<_Ty>>
	struct default_value;

	// template to indicate a forwarded parameter: _Ty has to be a quoted meta-function
	template<typename _Ty>
	struct tmpl;

	template<template<class...> class F>
	using tmpl_q = tmpl<mp11::mp_quote<F>>;

	// struct to create a forward-reference
	using fwd = tmpl_q<std::type_identity_t>;

	namespace impl {
		template<class _If>
		struct if_test {
		private:
			struct invalid_type_tag;

		public:
			template<class T> 
			using fn = mp11::mp_if<
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
			using fn = mp11::mp_if_c<
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
	using fwd_if = fwd_if_q<mp11::mp_quote<_If>>;

	template<class _If>
	using fwd_if_not_q = tmpl<typename impl::if_not_test<_If>>;

	template<template<class> class _If>
	using fwd_if_not = fwd_if_not_q<mp11::mp_quote<_If>>;


	// syntactic sugar
	// template to indicate a non-empty parameter pack
	// expands to _Ty, pack<_Ty>
	template<typename _Ty>
	struct non_empty_pack;




	namespace impl {

		// helper to inspect type indicators
		template<typename T>
		struct is_pack : mp11::mp_false {};

		template<typename T>
		struct is_pack<pack<T>> : mp11::mp_true {};

		template<typename T>
		struct remove_pack
		{
			using type = T;
		};

		template<typename T>
		struct remove_pack<pack<T>> {
			using type = T;
		};

		template<typename T>
		using remove_pack_t = typename remove_pack<T>::type;


		template<typename T>
		struct has_default_value : mp11::mp_false {};

		template<typename T, typename Init>
		struct has_default_value<default_value<T, Init>> : mp11::mp_true {};

		template<typename T>
		struct remove_default_value
		{
			using type = T;
		};

		template<typename T, typename Init>
		struct remove_default_value<default_value<T, Init>> {
			using type = T;
		};
		template<typename T>
		using remove_default_value_t = typename remove_default_value<T>::type;


		template<typename T>
		struct get_init_type
		{
			using type = T;
		};

		template<typename T, typename Init>
		struct get_init_type<default_value<T, Init>> {
			using type = Init;
		};
		template<typename T>
		using get_init_type_t = typename get_init_type<T>::type;



		template<typename T>
		struct is_tmpl : mp11::mp_false {};

		template<typename T>
		struct is_tmpl<tmpl<T>> : mp11::mp_true {};



		template<typename T, typename Arg>
		struct replace_tmpl {
			using type = T;
		};

		template<typename T, typename Arg>
		struct replace_tmpl<tmpl<T>, Arg> {
			using type = mp11::mp_invoke_q<T,Arg>;
		};


		template<typename T>
		struct is_non_empty_pack : mp11::mp_false {};

		template<typename T>
		struct is_non_empty_pack<non_empty_pack<T>> : mp11::mp_true {};


		template<typename T>
		struct remove_non_empty_pack
		{
			using type = T;
		};

		template<typename T>
		struct remove_non_empty_pack<non_empty_pack<T>> {
			using type = T;
		};

		template<typename T>
		using remove_non_empty_pack_t = typename remove_non_empty_pack<T>::type;

		//template<typename... T/*, typename Arg*/>
		//struct single_type_test: mp11::mp_true {
		//};

		//struct single_type_test {
		//	template<typename... T/*, typename Arg*/>
		//	using fn = mp11::mp_true;
		//};

		//template<typename T, typename Arg>
		//struct single_type_test<tmpl<T>, Arg> : mp11::mp_invoke_q<T, Arg> {
		//};

		//template<typename Ts, typename Args>
		//using single_type_tests = mp11::mp_all<mp11::mp_transform<single_type_test, Ts, Args>>;

		//template<typename T, typename Arg>
		//struct single_type_test<tmpl<T>, Arg> {
		//	using type = mp11::mp_invoke_q<T,Arg>;
		//};

		template<typename T, typename Arg>
		struct single_type_test : mp11::mp_true {
		};




		// core template that generates the call-operator to test
		template <size_t _Idx, size_t _DefaultsSpecified, class _Ty>
		struct _single_overload;

		template <size_t _Idx, size_t _DefaultsSpecified, class... _Tys, class _Tag, class _If>
		struct _single_overload<_Idx, _DefaultsSpecified, mp11::mp_list<mp11::mp_list<_Tys...>, _Tag, _If>> {

#if 0	
			// returning the function type by cast-operator does not work with template parameter (they cannot be specified explicitly)
			//using _overload_test =
			//	mp11::mp_if_c< std::is_same_v<int, mp11::mp_at_c<mp11::mp_list<_Tys...>,0>>,
			//	mp11::mp_list<std::integral_constant<size_t, _Idx>, std::integral_constant<size_t, _DefaultsSpecified>, mp11::mp_list<_Tys...>>(*)(_Tys...),
			//	void
			//	>;
			//template<class... T>
			//using _overload_test = mp11::mp_list<std::integral_constant<size_t, _Idx>, std::integral_constant<size_t, _DefaultsSpecified>, mp11::mp_list<_Tys...>>(*)(_Tys...);

			//template<class... T>
			//operator _overload_test<T...>();

			using _overload_test = mp11::mp_list<std::integral_constant<size_t, _Idx>, std::integral_constant<size_t, _DefaultsSpecified>, mp11::mp_list<_Tys...>>(*)(_Tys...);

			operator _overload_test();
#endif
			//template<class... T, std::enable_if_t < /*mp11::mp_invoke_q<_If, T...>::value &&*/ (single_type_test<_Tys, T>::value && ... && true), int* > = nullptr >
			//mp11::mp_list<std::integral_constant<size_t, _Idx>, std::integral_constant<size_t, _DefaultsSpecified>, mp11::mp_list<_Tys...>> test(typename replace_tmpl<_Tys, T>::type...) const;
			template<class... T, std::enable_if_t<mp11::mp_invoke_q<_If, T...>::value, int* > = nullptr >
			mp11::mp_list<_Tag, std::integral_constant<size_t, _Idx>, std::integral_constant<size_t, _DefaultsSpecified>, mp11::mp_list<_Tys...>> test(typename replace_tmpl<_Tys, T>::type...) const;
		};




		template <size_t _arg_count, size_t pack_index>
		struct _expand_pack_impl {
			template<class _Ty> using fn =
				mp11::mp_append<
				    mp11::mp_take_c<_Ty, pack_index>
				  , mp11::mp_repeat_c<
				        mp11::mp_list<remove_pack_t< mp11::mp_at_c<_Ty, pack_index >>>,
				        (_arg_count + 1 >= mp11::mp_size<_Ty>::value ? _arg_count + 1 - mp11::mp_size<_Ty>::value : 0) // avoid negative overflow
				    >
				  , mp11::mp_drop_c<_Ty, pack_index + 1>
				>;
		};

		template <size_t _arg_count, class _TyIf>
		struct _expand_packs {
			using _Ty = mp11::mp_first< _TyIf>;
			static constexpr auto pack_index = mp11::mp_find_if<_Ty, is_pack>::value;
			using type_no_If = typename mp11::mp_eval_if_q < mp11::mp_bool<(pack_index >= mp11::mp_size<_Ty>::value)>,
				_Ty,												// true-case
				_expand_pack_impl< _arg_count, pack_index>, _Ty		// false-case
			>;

			using type = mp11::mp_push_front<mp11::mp_rest< _TyIf>, type_no_If>;
		};


		template <size_t _arg_count, size_t _Idx, class _DefaultsSpecifiedIndices, class _Types_list>
		struct _defaulted_overload_set_;

		template <size_t _arg_count, size_t _Idx, size_t... _DefaultsSpecifiedIndices, class... _TypeIf_list>
		struct _defaulted_overload_set_<_arg_count, _Idx, std::index_sequence<_DefaultsSpecifiedIndices...>, mp11::mp_list<_TypeIf_list...>>
			: _single_overload<_Idx, _DefaultsSpecifiedIndices, typename _expand_packs<_arg_count, _TypeIf_list>::type>... {
			using _single_overload<_Idx, _DefaultsSpecifiedIndices, typename _expand_packs<_arg_count, _TypeIf_list>::type>::test...;
		};


		template <size_t _arg_count, size_t _Idx, class _TypeIfs_list>
		struct _defaulted_overload_set;

		template <size_t _arg_count, size_t _Idx, class... _TypeIf_list>
		struct _defaulted_overload_set<_arg_count, _Idx, mp11::mp_list<_TypeIf_list...>>
			: _defaulted_overload_set_<_arg_count, _Idx, std::index_sequence_for<_TypeIf_list...>, mp11::mp_list<_TypeIf_list...>> {
		
			using _defaulted_overload_set_<_arg_count, _Idx, std::index_sequence_for<_TypeIf_list...>, mp11::mp_list<_TypeIf_list...>>::test;
		};



		// turns a single mp_list<mp_list<function-types>, quoted enable-if> into an mp_list with all possible "default-argument" overloads
		// mp_list<mp_list<mp_list<function-types-def-args-expanded>, quoted enable-if>, ...>
		template <class _TyIf>
		struct _generate_default_args {

			using _Ty = mp11::mp_first< _TyIf>;

			static constexpr auto begin_default_value = mp11::mp_find_if<_Ty, has_default_value>::value;

			using _Ty_default_values_removed = mp11::mp_transform<remove_default_value_t, _Ty>;

			using valid_argcounts = mp11::mp_drop_c<
				mp11::mp_iota_c<1 + mp11::mp_size<_Ty>()>,
				begin_default_value
			>;

			using type_no_If =
				mp11::mp_transform_q<
				mp11::mp_bind_front<mp11::mp_take, _Ty_default_values_removed>,
				valid_argcounts
				>;

			template<class T> using create_if_t =
				typename mp11::mp_push_front<mp11::mp_rest< _TyIf>, T>;  // std::add_pointer_t in C++14

			using type =
				mp11::mp_transform<
				create_if_t,
				type_no_If
				>;
		};




		template <size_t non_empty_pack_index>
		struct _replace_non_empty_pack_impl {
			template<class _Ty> using fn =
				mp11::mp_append<
					mp11::mp_take_c<_Ty, non_empty_pack_index>
				  , mp11::mp_list<remove_non_empty_pack_t< mp11::mp_at_c<_Ty, non_empty_pack_index >>, pack<remove_non_empty_pack_t< mp11::mp_at_c<_Ty, non_empty_pack_index >>>>
				  , mp11::mp_drop_c<_Ty, non_empty_pack_index + 1>
				>;
		};

		// replace syntactic-sugar types
		template <class _TyIf>
		struct _replace_syntactic_sugar {

			using _Ty = mp11::mp_first< _TyIf>;

			static constexpr auto non_empty_pack_index = mp11::mp_find_if<_Ty, is_non_empty_pack>::value;
			using type_no_If = typename mp11::mp_eval_if_q < mp11::mp_bool<(non_empty_pack_index >= mp11::mp_size<_Ty>::value)>,
				_Ty,												// true-case
				_replace_non_empty_pack_impl<non_empty_pack_index>, _Ty		// false-case
			>;

			using type = mp11::mp_push_front<mp11::mp_rest< _TyIf>, type_no_If>;
		};


		template <size_t _arg_count, class _Indices, class... _Types>
		struct _overload_set;




		//	size_t _arg_count: numer of actual arguments
		//  size_t... _Indices: index sequence indexing overloads in overload_set
		// _Types...: mp_list<mp_list< func-types >, quoted enable-if>
		template <size_t _arg_count, size_t... _Indices, class... _Types>
		struct _overload_set<_arg_count, std::index_sequence<_Indices...>, _Types...>
			: _defaulted_overload_set<_arg_count, _Indices, typename _generate_default_args<typename _replace_syntactic_sugar<_Types>::type>::type>... {
			using _defaulted_overload_set<_arg_count, _Indices, typename _generate_default_args<typename _replace_syntactic_sugar<_Types>::type>::type>::test...;
		};



		template<class... _Ty>
		using true_t = mp11::mp_true;

		struct none_tag{};
	}


	template <class _Overload_Type_set, size_t _arg_count>
	struct overload_set;

	template <class... _Overload_Type_set, size_t _arg_count>
	struct overload_set<mp11::mp_list<_Overload_Type_set...>, _arg_count> : impl::_overload_set<_arg_count, std::index_sequence_for<_Overload_Type_set...>, _Overload_Type_set...> {};



	// helper for inspecting current overload
	static constexpr size_t tag_index = 0;
	static constexpr size_t overload_index = 1;
	static constexpr size_t defaults_specified_index = 2;
	static constexpr size_t overload_type_index = 3;

	template<class _Overload, class _Tag>
	using has_tag = std::is_same<mp11::mp_at_c<_Overload, tag_index>, _Tag>;

	template<class _Overload>
	using index = mp11::mp_at_c<_Overload, overload_index>;

	template<class _Overload>
	using defaults_specified = mp11::mp_at_c<_Overload, defaults_specified_index>;

	template<class _Overload_Type_set, class _Overload, size_t _DefaultIdx>
	decltype(auto) get_default_value() {
		using _Ty = mp11::mp_first<mp11::mp_at_c<_Overload_Type_set, index<_Overload>::value>>;
		static constexpr auto begin_default_value = mp11::mp_find_if<_Ty, impl::has_default_value>::value;

		return impl::get_init_type_t<mp11::mp_at_c<_Ty, begin_default_value + _DefaultIdx>>{}();
	}



	template<class... _Ty>
	using ol_list = mp11::mp_list<_Ty...>;

	template<class _Tag, template<class...> class _If, class... _Ty>
	using tagged_ol_if = mp11::mp_list<mp11::mp_list<_Ty...>, _Tag, mp11::mp_quote<_If>>;

	template<template<class...> class _If, class... _Ty>
	using ol_if = tagged_ol_if<impl::none_tag, _If, _Ty...>;

	template<class _Tag, class... _Ty>
	using tagged_ol = tagged_ol_if<_Tag, impl::true_t, _Ty...>;

	template<class... _Ty>
	using ol = ol_if<impl::true_t, _Ty...>;


	//template<typename... Ts, decltype((hop::overload_set<overloads_t, sizeof...(Ts)>{}.test<Ts...>(std::declval<Ts>()...)), 0) = 0 >
	template<class _Overload_Type_set, typename... _Tys>
	decltype(overload_set<_Overload_Type_set, sizeof...(_Tys)>{}.test<_Tys...>(std::declval<_Tys>()...)) enabler();


	template<class T> struct dependent_false : std::false_type {};
}





