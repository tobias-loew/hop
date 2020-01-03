# hop
homogeneous variadic function parameters

Copyright Tobias Loew 2019. 

Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# What is hop
hop is a small library the enables you to create *proper* homogeneous variadic function parameters

# What does *proper* mean
*proper* means, that the functions you equip with hop's homogeneous variadic parameters are subject to C++ overload resolution.
Let me show you an example:

Suppose you want to have a function `foo` that accepts an arbitrary non-zero number of `int` arguments.

The traditional solution from the pre C++11 age was to create overloads for `foo` up to a required/reasonable number of arguments
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
But when we now call `foo(42)` or `foo(0.5, -1.3)` we always get ambigous call errors - and that's absolutely correct: both `foo` templates accept the argument-lists (`int` is convertible to `double` and vice-versa) and both take their arguments as *forwarding-references* so they're both equivalent perfect matches - bang! 

And here we are at the core of the problem: when we have multiple functions defined as above C++'s overload resolution won't step in to select the best match - they're all best matches (as long as we only consider only template functions). And here __hop__ can help...

# Creating overload-sets with __hop__
With __hop__ we define only a single overload of `foo` but with a quite sophisticated SFINAE condition:
```
using overloads = hop::ol_list <
    hop::ol<hop::non_empty_pack<int>>,
    hop::ol<hop::non_empty_pack<double>>
>;


template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
}
```
Now, we can call `foo` the same way we did for the traditional (bounded) overload-sets:
```
    foo(42, 17);
    foo(1.5, -0.4, 12.0);
    foo(42, 0.5);           // error: ambigous
```
Let's see what we can do inside of `foo`. The type `decltype(hop::enable<overloads, Ts...>())` holds information about the selected overload, e.g. its zero-based `index`:
```
using overloads = hop::ol_list <
    hop::ol<int, hop::non_empty_pack<int>>,
    hop::ol<double, hop::non_empty_pack<double>>
>;

template<typename Out, typename T>
void output_as(T&& t) {
    std::cout << (Out)t << std::endl;
}

template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
    using OL = decltype(hop::enable<overloads, Ts...>());

    if constexpr (hop::index<OL>::value == 0) {
        std::cout << "got a bunch of ints\n";
        (output_as<int>(ts),...);
        std::cout << std::endl;
    } 
    else
    if constexpr (hop::index<OL>::value == 1) {
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

template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
    using OL = decltype(hop::enable<overloads, Ts...>());

    if constexpr (hop::has_tag<OL, tag_ints>::value) {
      // ...
    } 
    else
    if constexpr (hop::has_tag<OL, tag_doubles>::value) {
      // ...
    }
}
```
We can also tag types of an overload. This is useful, when we want to access the argument(s) belonging to a certain type of the overload: 
```
struct tag_ints {};
struct tag_double {};
struct tag_numeric {};

using overloads = hop::ol_list <
    hop::tagged_ol<tag_ints, std::string, hop::non_empty_pack<hop::tagged_ty<tag_numeric, int>>>,
    hop::tagged_ol<tag_doubles, std::string, hop::non_empty_pack<hop::tagged_ty<tag_numeric, double>>>
>;

template<typename... Ts, decltype((hop::enable<overloads, Ts...>()), 0) = 0 >
void foo(Ts&& ... ts) {
    using OL = decltype(hop::enable<overloads, Ts...>());

    if constexpr (hop::has_tag<OL, tag_ints>::value) {
          auto&& numeric_args = hop::get_tagged_args<OL, tag_numeric>(std::forward<Ts>(ts)...);
          // numeric_args is a std::tuple containing all the int args
          // ...
    } 
    else
    if constexpr (hop::has_tag<OL, tag_doubles>::value) {
        auto&& numeric_args = hop::get_tagged_args<OL, tag_numeric>(std::forward<Ts>(ts)...);
        // numeric_args is a std::tuple containing all the double args
        // ...
    }
}
```

Up to now, we can create non-empty homogeneous overloads for specific types. Let's see what else we can do with __hop__.
A single overload `hop::ol<...>` consists of a list of types that are:
- normal C++ types, like `int`, `vector<string>`, user-defined types, which may be qualified. Those types are matched as if they were types of function arguments.
- `hop::repeat<T, min>`, `hop::repeat<T, min, max>` at least `min` (and up to `max`) times the argument-list generated by `T`. If `max` is not specified, then `repeat` is unbounded. Multiple `repeat`s (even unbounded) in a single overload are possible! Also all other types/type-constructs after `repeat` are possible.
- `hop::pack<T>` or `hop::non_empty_pack<T>` are aliases for `hop::repeat<T, 0>` resp. `hop::repeat<T, 1>`.
- `hop::optional<T>` is an alias for `hop::repeat<T, 0, 1>`
- `hop::eps` is a typedef for `hop::repeat<char, 0, 0>` (it consumes no argument)
- `hop::seq<T1,...,TN>` appends the argument-lists generated by `T1`, ... , `TN`
- `hop::alt<T1,...,TN>` generates the argument-lists for `T1`, ... , `TN` and handles each as a separate case
- `hop::cpp_defaulted_param<T, _Init = default_init<T>>` creates an argument of type `T` or nothing. `hop::cpp_defaulted_param` creates a C++-style defult-param: types following a `hop::cpp_defaulted_param` must also be a `hop::cpp_defaulted_param`
- `hop::general_defaulted_param<T, _Init = default_init<T>>` creates an argument of type `T` or nothing. `hop::general_defaulted_param` can appear in any position of the type-list
- `hop::fwd` is a place holder for a *forwarding-reference* and accepts any type
- `hop::fwd_if<template<class> class _If>` is a *forwarding-reference* with SFINAE condition applied to the actual parameter type
- `hop::adapt` adapts an existing function as an overload: `hop::adapt<bar>`
- `hop::adapted` can be used to adapt existing overload-sets or templates:
  ```
  
    void bar(int n, std::string s) {
       ...
    }
    
    template<class T>
    auto qux(T&& t, double d, std::string const& s) {
       ...
    }

    struct adapt_qux {
        template<class... Ts>
        static decltype(qux(std::declval<Ts>()...)) forward(Ts&&... ts) {
            return qux(std::forward<Ts>(ts)...);
        }
    };
    
    using overloads_t = hop::ol_list <
      hop::adapt<bar>,
      hop::adapted<adapt_qux>
    >;
    
    template<typename... Ts, decltype((hop::enable<overloads_t, Ts...>()), 0) = 0 >
    void foo(Ts&& ... ts) {
        using OL = decltype(hop::enable<overloads_t, Ts...>());
        if constexpr (hop::is_adapted_v<OL>) {
            return hop::forward_adapted<OL>(std::forward<Ts>(ts)...);
        }
    }
    
  ```
- for template type deduction there is a *global* and a *local* version:
  - the *global* version corresponds to the usual template type deducing. Let's look a an example:
    ```
    template<class T1, class T2>
    using map_alias = std::map<T1, T2>const&;

    template<class T1, class T2>   // !!! class T1 is required
    using set_alias = std::set<T2>const&;
    
    ...
  
    hop::ol<hop::deduce<map_alias>, hop::deduce<set_alias>>
  
    ...
    std::map<int, std::string> my_map;
    std::set<std::string> my_set;
    foo(my_map, my_set);

    std::set<double> another_set;
    foo(my_map, another_set); // error
    ``` 
    All arguments specified with `hop::deduce` take part in the global type-deduction, thus `foo` can only be called with a map and a set, where the set-type is the same as the mapped-to-type.
    Please note that in the definition of the template-alias for `set_alias` the unused template type `class T1` is required, since `T1` and `T2` are deduced by matching `map_alias` and `set_alias` *simultaneously*.
    
  - in the *local* version the types are deduced intependently for each argument, for example
    ```
    template<class T>
    using map_vector = std::vector<T>const&;

    ...
  
    hop::ol<hop::pack<hop::deduce_local<map_vector>>>
  
    ...
    std::vector<int> v1;
    std::vector<double> v2;
    std::vector<std::string> v3;
    foo(v1, v2, v3);
    ```
    `foo` matches any list of `std::vector`s. Note, that this cannot be achived with global-deduction as the number of deduced-types is variable.  
- types can be tagged with `hop::tagged_ty<tag_type, T>` for accessing the arguments of an overload
- argument-lists can be *gathered* with `hop::gather<tag_type, L>` where `L` is a list build with the above constructors.
  `gather` does not affect the generated argument-list but gathers it's content into a single `std::tuple` when accessing the actual parameters.
  Additionally, `gather` is tagged to distinguish different gatherings
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
  allow to specify an additional SFINAE-condition which is applied to the actual parameter type pack. There is also version `tagged_ol_if_q` with expects a quoted meta-function as SFINAE-condition.

All overloads for a single function are gathered in a `hop::ol_list<...>`

The following grammar describes how to build argument-lists for overload-sets:
<pre><code>
CppType =  
    <i>any C++ type</i>
    ;

Type =  
    CppType
    | tagged_ty&lt;<i>tag</i>, Type> 
    ;

ArgumentList =
    Type 
    | repeat&lt;ArgumentList, <i>min</i>, <i>max</i>> 
    | seq&lt;ArgumentList,...,ArgumentList> 
    | alt&lt;ArgumentList,...,ArgumentList> 
    | cpp_defaulted_param&lt;Type, <i>init</i>>
    | general_defaulted_param&lt;Type, <i>init</i>> 
    | fwd
    | fwd_if&lt;<i>condition</i>>
    | gather&lt;<i>tag</i>, ArgumentList> 
</code></pre>

Inside a function `hop` provides several templates and functions for inspecting the current overload and accessing function arguments:
- `get_count...` returns the number of arguments (having a certain tag or satisfying a certain condition)
  ```
    template<class _Overload>
    constexpr size_t get_count();

    template<class _Overload, class _Tag>
    constexpr size_t get_tagged_count();

    template<class _Overload, class _If>
    constexpr size_t get_count_if_q();
    
    template<class _Overload, template<class> class _If>
    constexpr size_t get_count_if();
  ``` 

- `get_args...(std::forward<Ts>(ts))...)` returns the arguments (having a certain tag or satisfying a certain condition) as a tuple of references
  ```
    template<class _Overload, class... Ts>
    constexpr decltype(auto) get_args(Ts &&... ts);

    template<class _Overload, class _Tag, class... Ts>
    constexpr decltype(auto) get_tagged_args(Ts &&... ts);

    template<class _Overload, class _If, class... Ts>
    constexpr decltype(auto) get_args_if_q(Ts &&... ts);

    template<class _Overload, template<class> class _If, class... Ts>
    constexpr decltype(auto) get_args_if(Ts &&... ts);
  ```


- 
  ```
    template<class _Overload, class _Tag, size_t tag_index = 0, class... Ts>
    constexpr decltype(auto) get_arg(Ts &&... ts);
  ```
  returns 
    template<class _Overload, class _Tag, class... Ts>
    constexpr decltype(auto) get_tagged_args(Ts &&... ts);

    template<class _Overload, class _If, class... Ts>
    constexpr decltype(auto) get_args_if_q(Ts &&... ts);

    template<class _Overload, template<class> class _If, class... Ts>
    constexpr decltype(auto) get_args_if(Ts &&... ts);
  ```


    // get_arg_or_call will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class _FnOr, class... Ts>
    constexpr decltype(auto) get_arg_or_call(_FnOr&& _fnor, Ts&&... ts) {
        return impl::get_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::is_a_callable>(std::forward<_FnOr>(_fnor), std::forward<Ts>(ts)...);
    }

    // get_arg_or will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class _Or, class... Ts>
    constexpr decltype(auto) get_arg_or(_Or && _or, Ts &&... ts) {
        return impl::get_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::is_a_value>(std::forward<_Or>(_or), std::forward<Ts>(ts)...);
    }


    // get_arg will always go for the first type with a matching tag
    template<class _Overload, class _Tag, size_t tag_index = 0, class... Ts>
    constexpr decltype(auto) get_arg(Ts &&... ts) {
        return impl::get_arg_or<_Overload, _Tag, tag_index, impl::or_behaviour::result_in_compilation_error>(0, std::forward<Ts>(ts)...);
    }

Examples can be found in test\hop_test.cpp.


# That's one small step for man, a lot of hops for a bunny!
![luna-bunny](/luna.png)
`bunny(hop, hop, hop, ...);`
