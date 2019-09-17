# hop
homogenous variadic function parameters

Copyright Tobias Loew 2019. 

Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# What is hop
hop is a small library the enables you to create *proper* homogenous variadic function parameters

# What does *proper* mean
*proper* means, that the functions you equip with hop's homogenous variadic parameters are subject to C++ overload resolution.
Let me show you an example:

Suppose you want to have a function `foo` that accepts an arbitrary non-zero number of `int` arguments.

The traditional solution from the pre C++11 age was to create overloads for `foo` up to a required/resonable number of arguments
```
void foo(int n1);
void foo(int n1, int n2);
void foo(int n1, int n2, int n3);
void foo(int n1, int n2, int n3, int n4);
```

Now, with C++11 and variadic Templates, we can write the whole overload-set as a single function
```
template<typename... Args>
void foo(Args&&... args);
```
but wait, we haven't said anything about `int` - specified as above, `foo` can be called with *any* list of parameters. So, how can we constrain `foo` to only accept argument-list containing one or more `int` arguments ? Of course, we use SFINAE
```
template<typename... Ts>
using AllInts = typename std::conjunction<std::is_convertible<Ts, int>...>::type;

template<typename... Ts, typename = std::enable_if_t<AllInts<Ts...>::value, void>>
void foo(Ts&& ... ts) {}
```
in the same way we can do this for `double` 
```
template<typename... Ts>
using AllDoubles = typename std::conjunction<std::is_convertible<Ts, double>...>::type;

template<typename... Ts, typename = std::enable_if_t<AllDoubles<Ts...>::value, void>>
void foo(Ts&& ... ts) {}
```
But, when we use both overload set together, we get an error that `foo` is defined twice. ((C++17; §17.1.16) A template-parameter shall not be given default arguments by two different declarations in the same scope.) cf. https://www.fluentcpp.com/2018/05/15/make-sfinae-pretty-1-what-value-sfinae-brings-to-code/

One possible solution is
```
template<typename... Ts>
using AllInts = typename std::conjunction<std::is_convertible<Ts, int>...>::type;

template<typename... Ts, typename std::enable_if_t<AllInts<Ts...>::value, int> = 0>
void foo(Ts&& ... ts) {}


template<typename... Ts>
using AllDoubles = typename std::conjunction<std::is_convertible<Ts, double>...>::type;

template<typename... Ts, typename std::enable_if_t<AllDoubles<Ts...>::value, int> = 0>
void foo(Ts&& ... ts) {}
```
But when we now call `foo(42)` or `foo(0.5, -1.3)` we always get ambigous call errors - and thats absolutely correct: both `foo` templates accept the argument-lists (`int` is convertible to `double` and vice-versa) and both take their arguments as *forwarding-references* so they're both equivalent perfect matches - bang! 

And here we are at the core of the problem: when we have multiple functions defined as above C++'s overload reolution won't step in to select the best match - they're all best matches (as long as we only consider only template functions). And here __hop__ can help...

# Creating overload-sets with __hop__
With __hop__ we define only a single overload of `foo` but with a quite sophisticated SFINAE condition:
```
using overloads = hop::ol_list <
	hop::ol<hop::non_empty_pack<int>>,
	hop::ol<hop::non_empty_pack<double>>
>;


template<typename... Ts, decltype((hop::enabler<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
}
```
Now, we can call `foo` the same way we did for the traditional (bounded) overload-sets:
```
	foo(42, 17);
	foo(1.5, -0.4, 12.0);
	foo(42, 0.5);			// error: ambigous
```
Let's see what we can do inside of `foo`. The type `decltype(hop::enabler<overloads, Ts...>())` has information about the selected overload, e.g. its zero-based `index`:
```
using overloads = hop::ol_list <
	hop::ol<int, hop::non_empty_pack<int>>,
	hop::ol<double, hop::non_empty_pack<double>>
>;

template<typename Out, typename T>
void output_as(T&& t) {
	std::cout << (Out)t << std::endl;
}

template<typename... Ts, decltype((hop::enabler<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
	using _OL = decltype(hop::enabler<overloads, Ts...>());

	if constexpr (hop::index<_OL>::value == 0) {
		std::cout << "got a bunch of ints\n";
		(output_as<int>(ts),...);
		std::cout << std::endl;
	} 
	else
	if constexpr (hop::index<_OL>::value == 1) {
		std::cout << "got a bunch of doubles\n";
		(output_as<double>(ts), ...);
		std::cout << std::endl;
	}
}
```
output
```
got a bunch of ints
42
17

got a bunch of doubles
1.5
-0.4
12
```
Alternatively, we can *tag* an overload, and test for it:
```
struct tag_ints {};
struct tag_doubles {};

using overloads = hop::ol_list <
	hop::tagged_ol<tag_ints, hop::non_empty_pack<int>>,
	hop::tagged_ol<tag_doubles, hop::non_empty_pack<double>>
>;

template<typename... Ts, decltype((hop::enabler<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
	using _OL = decltype(hop::enabler<overloads, Ts...>());

	if constexpr (hop::has_tag<_OL, tag_ints>::value) {
      // ...
	} 
	else
	if constexpr (hop::has_tag<_OL, tag_doubles>::value) {
      // ...
	}
}
```
Up to now, we can create non-empty homogenous overloads for specific types. Let's see what else we can do with __hop__.
A single overload `hop::ol<...>` consisit of a list of types that are:
- normal C++ types, like `int`, `vector<string>`, user-defined type, and, of course, they can be qualified. Those types are matched as if they were types of function arguments.
- `hop::pack<T>` or `hop::non_empty_pack<T>`, but at-most one per overload. `pack ` and `non_empty_pack` exand to the appropriate (non-zero) number of `T` arguments. *Additional types (including `hop::default_value`) __after__ a `pack` are possible!*
- `hop::default_value<T, _Init = default_init<T>>`, creates an argument of type `T` or nothing. Types following a `hop::default_value` must also be a `hop::default_value` (`hop::pack<hop::default_value<T>>` is for obvious reasons not supported)
- `hop::fwd` is a place holder for a *forwarding-reference* and accepts any type
- `hop::fwd_if<template<class> class _If>` is a *forwarding-reference* with SFINAE condition applied to the actual parameter type
- finally, the following variations of `hop::ol<...>`:
  ```
	template<template<class...> class _If, class... _Ty>
	using ol_if;
  ```
  and
  ```
	template<class _Tag, template<class...> class _If, class... _Ty>
	using tagged_ol_if;
  ```
  allow to specify a additional SFINAE-condition which is applied to the complete actual parameter type pack

All overloads for a single function have to be gathered in a `hop::ol_list<...>`

Let's take a look at some more examples:
- 
  ```hop::ol_list<
       hop::ol<std::file&, hop::pack<int>
  ```


# That's one small step for man, a lot of hops for a bunny!
![luna-bunny](/luna.jpg)
`bunny(hop, hop, hop, ...);`
