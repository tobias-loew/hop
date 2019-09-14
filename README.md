# hop
homogenous variadic function parameters

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
using AllInts = decltype((((void)int(std::declval<Ts>())), ...));

template<typename... Args, typename = AllInts<Args...>>
void foo(Args && ... args) {}

```
in the same way we can do this for `double` 
```
template<typename... Ts>
using AllDoubles = decltype((((void)double(std::declval<Ts>())), ...));

template<typename... Args, typename = AllDoubles<Args...>>
void foo(Args && ... args) {}
```
But, when we use both overload set together, we get an error that `foo` is defined twice. ((C++17; §17.1.16) A template-parameter shall not be given default arguments by two different declarations in the same scope.) cf. https://www.fluentcpp.com/2018/05/15/make-sfinae-pretty-1-what-value-sfinae-brings-to-code/

One possible solution could be
```
template<typename... Ts>
using AllInts = decltype((((void)int(std::declval<Ts>())), ...), bool{});

template<typename... Args, AllInts<Args...> = false>
void foo(Args && ... args) {}


template<typename... Ts>
using AllDoubles = decltype((((void)double(std::declval<Ts>())), ...), bool{});

template<typename... Args, AllDoubles<Args...> = false>
void foo(Args && ... args) {}
```
But when we now call `foo(42)` or `foo(0.5, -1.3)` we always get ambigous call errors - and thats absolutely correct: both `foo` templates can accept the argument-lists and both take their arguments as *forwarding-references* so they're both equivalent perfect matches - bang! 

And here we are at the core of the problem: when we have multiple functions the way defined as above C++'s overload reolution won't step in to select the best match, since they're all best matches.

Here __hop__ can help: with __hop__ we define only a single overload of `foo` but with a quite sophisticated SFINAE condition:
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
Alternatively, we can tag the overloads, and test for it:
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


# It's a small step for man but a hop for bunnies!
![luna-bunny](/luna.jpg)
