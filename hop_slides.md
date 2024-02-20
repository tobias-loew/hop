---

marp: true
enableHtml: true
__theme: gaia
class: invert
paginate: true
_paginate: false



---

<style>

#hp  {
float: right;    
 margin: 0 0 0 15px;
 vertical-align:middle;
}

.box {
   display: flex;
   align-items:center;
   text-align:right;
   justify-content: flex-end
}

</style>

![bg](pictures/CppNowVirtualBackground_with_date2.jpg)


<!-- <h1 style='color:white; font-size: 170%'>hop: a language to design function-overload-sets</h1>
<span style='color:white; font-size: 125%;'>Tobias Loew</span> -->
<span style='color:white; font-size: 170%; font-weight: 500'>hop: a language to design function-overload-sets</span>
<span style='color:rgb(140,192,96); ; font-size: 135%;'>Tobias Loew</span>
<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'>come and hop with me!</span><img src="pictures/Luna_Meersau.png" style=" width:560px" id="hp"/>
</div>

<!-- <img src="pictures/Luna_Meersau.png" style=" width:400px; float:right" /><div style="clear:both; text-align:right"><h2>come and hop with me!</h2></div> -->



---
<!--header: 'outline'-->

# outline
- prelude: homogeneous variadic functions
    * Blitz++ & a series of blogs on Fluent-C++
    * historic view / best practices and limitations
    * overload-resolution
    * what about concepts?
    * a library for homogeneous variadic functions
* hop: function-parameter building-blocks (started as: <span style="text-decoration: underline;">ho</span>mogeneous <span style="text-decoration: underline;">p</span>arameters)
    * a grammar for function-parameters
    * defining overload sets
    * down into the rabbit hole: hop-examples and internals


---
<!--header: 'homogeneous variadic functions (HVFs): what are we talking about'-->

## homogeneous variadic functions (HVFs):<br/>what are we talking about

<pre><code><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#c8c8c8;">logger</span><span style="color:#b4b4b4;">(</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">This</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">is</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">a</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">log</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">message</span><span style="color:#e8c9bb;">&quot;</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#569cd6;">double</span>&nbsp;<span style="color:#c8c8c8;">max</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.0</span><span style="color:#b4b4b4;">,</span>&nbsp;d1<span style="color:#b4b4b4;">,</span>&nbsp;d2<span style="color:#b4b4b4;">,</span>&nbsp;d3<span style="color:#b4b4b4;">,</span>&nbsp;d4<span style="color:#b4b4b4;">,</span>&nbsp;d5<span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">Array</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#9cdcfe;">vec</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1000</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;<br/><span style="color:#4ec9b0;">Array</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#9cdcfe;">matrix</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">6</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">100</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">Array</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#9cdcfe;">cube</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">5</span><span style="color:#b4b4b4;">);</span><br/></code></pre>

* function with an arbitrary (non-zero) number of arguments, all of the **same type**<br/><pre style="_font-size:100%"><code><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">f</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">f</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">f</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">...</span><br/></code></pre>
* should also work for overloads of HVFs<br/>especially needed for constructors and function call `operator()`

<!--

The Return Type may depend on the number of arguments

-->

---
<!--header: 'Blitz++ & a series of blogs on Fluent-C++'-->

## Blitz++ & a series of blogs on Fluent-C++

- Blitz++: lin. algebra library, pioneering in expression templates, C++98
* Blitz++ `Array` class-template 
    * 48 constructors, 85 `operator()` overloads -> essentially different:
        * 13 constructors
        * 3 `operator()` and 8 `operator() const`
        * several other homogenous variadic functions from 1 - 10 arguments
    * a lot of similar code for 1 to 10 homogenous args in whole library

* Fluent-C++: "How to Define a Variadic Number of Arguments of the Same Type"<br/>three parts published 01-02/2019
    * different approaches to HVFs: pros / cons 

<!--

Blitz++: by Todd Veldhuizen, conforming to C++98

Fluent-C++: Blog by Jonathan Boccara

C++23 'Deducing this'

-->

---

# history of homogeneous variadic functions in C++ 

---
<!--header: 'C++98: write overloads manually'-->

## C++98: write overloads manually


<pre><code><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a1</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a2</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a3</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a3</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a4</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a3</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a4</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a5</span><span style="color:#b4b4b4;">);</span><br/></code></pre>


<!-- 
    JUST EXPLAIN ONCE HERE:

    Implementation strategies:
    * accumulate / recursion
    * put arguments into container and call helper / forward arguments 
    
    -->

* pros
    * explicit: easy to write and understand
    * beginner friendly, no templates or macros required
* cons
    * only up to a given number of arguments
    * anti-pattern: archetype of DRY


---
<!--header: 'C++98: Boost.Preprocessor'-->
## C++98: Boost.Preprocessor
<pre><code><span style="color:#9b9b9b;">#define</span>&nbsp;<span style="color:#beb7ff;">NUMBER_OF_ARGUMENTS_OF_F</span>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b5cea8;">10</span><br/><span style="color:#9b9b9b;">#define</span>&nbsp;<span style="color:#beb7ff;">GENERATE_OVERLOAD_OF_F</span><span style="color:#b4b4b4;">(</span>Z<span style="color:#b4b4b4;">,</span>&nbsp;N<span style="color:#b4b4b4;">,</span>&nbsp;_<span style="color:#b4b4b4;">)</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\<br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#c8c8c8;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#beb7ff;">BOOST_PP_ENUM_PARAMS</span><span style="color:#b4b4b4;">(</span>N<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span>&nbsp;a<span style="color:#b4b4b4;">))</span>&nbsp;<span style="color:#b4b4b4;">{&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/> <br/><span style="color:#beb7ff;">BOOST_PP_REPEAT_FROM_TO</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#beb7ff;">BOOST_PP_INC</span><span style="color:#b4b4b4;">(</span><span style="color:#beb7ff;">NUMBER_OF_ARGUMENTS_OF_F</span><span style="color:#b4b4b4;">),</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#beb7ff;">GENERATE_OVERLOAD_OF_F</span><span style="color:#b4b4b4;">,</span>&nbsp;_<span style="color:#b4b4b4;">)</span><br/></code></pre>
* pros
    * no repetition of code
    * number of arguments can easily be adjusted
* cons
    * declaration/definition is hidden in macros
    * hard to write, even harder to read, not to mention debugging
    * number of arguments is fixed a priori and limited by Boost.Preprocessor
    

<!-- 
max args limited be Boost.Preprocessor (was 256, now up to 1024)

 -->
 

---
<!--header: 'C++11: std::initializer_list'-->

## C++11: std::initializer_list

<pre><code><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">initializer_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">A</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#9a9a9a;">as</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">/*...*/</span><br/><span style="color:#b4b4b4;">}</span><br/></code></pre>

* pros
    * works for any number of arguments
* cons
    * requires additional braces when called: `{a1, ... , an}`
    * inflexible wrt. mutability
    * may also be called with empty list
    * just one argument (the `initializer_list`)

<!--
mutability: std::reference_wrapper
just one argument: different type deduction for braced lists
-->


---
<!--header: 'C++11: parameter pack'-->

## C++11: parameter pack

<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span><br/><span style="color:#b4b4b4;">}</span><br/></code></pre>

* pros
    * works for any number of arguments
* cons
    * matches any input
    * invalid input causes 
        * compilation errors: bad
        * logical errors: worse
        * runtime errors: havoc


<!--
matches any input: when used with forwarding references
-->

---
<!--header: 'C++11: parameter pack + SFINAE'-->

## C++11: parameter pack + SFINAE (in shorter C++14 style)
<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">enable_if_t</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">...(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;at&nbsp;least&nbsp;one&nbsp;argument</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span>is_convertible_v<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span>&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;all&nbsp;arguments&nbsp;convertible&nbsp;to&nbsp;A</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span>*&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">nullptr</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">/*...*/</span><br/><span style="color:#b4b4b4;">}</span><br/></code></pre>

* pros
    * works for any number of arguments
    * matches only valid input
* cons
    * none



---
<!--header: 'C++20: parameter pack + requires'-->

## C++20: parameter pack + requires

<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">...(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;at&nbsp;least&nbsp;one&nbsp;argument</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span>&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;all&nbsp;arguments&nbsp;convertible&nbsp;to&nbsp;A</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">/*...*/</span><br/><span style="color:#b4b4b4;">}</span><br/></code></pre>

* pros
    * works for any number of arguments
    * matches only valid input
* cons
    * none

---
<!--header: 'the future of C++'-->

## the future of C++
## P1219R2: homogeneous variadic function parameters

<pre><code><span style="color:#569cd6;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">A</span>&nbsp;<span style="color:#9a9a9a;">a</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">A</span><span style="color:#b4b4b4;">...</span>&nbsp;as<span style="color:#b4b4b4;">)</span><span style="color:#b4b4b4;">{&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/></code></pre>

* pros
    * native language support for HVFs
    * matches only valid input
* cons
    * syntax conflicts with `varargs`: `,` before trailing `...` is optional, i.e.<br/>`R foo(int, ...)` is equivalent to `R foo(int...)`
    * current status of P1219R2: dead (Belfast 2019)

<!--
Proposal 

VARARGS comma is NOT mandatory

Belfast 2019: [P1219r2] Homogeneous variadic function parameters: did not receive sufficient support to move forward.
-->

---

## HVFs: current best practice

* parameter pack + SFINAE
* parameter pack + requires
* what about overload resolution and implicit conversion?

---

<!--header: 'overload-resolution'-->

## overload-resolution and implicit conversion

simple HVFs for any number of `int` and `float`
<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">int</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/></code></pre>

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">float</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/></code></pre>


assuming both overloads of <code><span style="color:#dcdcaa;">foo</span></code> are visible, which one is called?
<pre><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(a)</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">-</span><span style="color:#b5cea8;">2.4f</span><span style="color:#b4b4b4;">);</span>&nbsp;<span style="color:#57a64a;">//&nbsp;(b)</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(c)</span><br/></code></pre>

---

<!--header: 'the hop-experts'-->

<center>

# this poll is presented to you by the hop-experts

</center>

<style>
 .hop_experts { 
     border: none;
  margin: none;
  border-collapse: collapse;
  width:100%;
  background-color: rgb(34,34,34);

      }


.hop_experts_td {
    box-sizing: content-box;
  border: none;
  margin: none;
  background-color: rgb(34,34,34);
}
</style>

<div style="margin: auto;">
<table class="hop_experts">
<tr style="border: none">
<td class="hop_experts_td" style="text-align:right; ">

# Luna

</td>

<td class="hop_experts_td" style="width:580px">

![width:500px center](pictures/hop_experts_clipped.jpg)

</td>

<td class="hop_experts_td" >

# Rolf

</td>

</tr>
</table>
</div>

---

## my naïve expectation
- <pre><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(a)&nbsp;calls&nbsp;int-overload</span></code></pre>


- <pre><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">-</span><span style="color:#b5cea8;">2.4f</span><span style="color:#b4b4b4;">);</span>&nbsp;<span style="color:#57a64a;">//&nbsp;(b)&nbsp;calls&nbsp;float-overload</span></code></pre>


- <pre><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(c)&nbsp;error: ambiguous</span><br/></code></pre>

* let's ask the compiler <a href="https://godbolt.org/z/YbbY5ah7d" target="_blank">overload test @ godbolt.org/z/YbbY5ah7d</a>



---

<!--header: 'overload-resolution'-->
<div style="margin-left: auto;margin-right: auto">
<span style="font-weight:bold;font-size:240%;color:#FFFFFF">what just happened?<br/> why are all 3 calls ambiguous? </span>
</div>


---

<!--header: 'overload-resolution:analyzing the ambiguity'-->
## analyzing the ambiguity
<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">int</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/><br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">float</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">-</span><span style="color:#b5cea8;">2.4f</span><span style="color:#b4b4b4;">);</span>&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/></code></pre>
* all arguments are implicitly convertible to `int` / `float` (both overloads are viable)
* both overloads take arguments as forwarding reference `T&&` (perfect match)
* SFINAE-condition: test if overload is viable, it is _not_ part of overload resolution
* wrt. overload resolution, both overloads are equivalent

---

<!--header: 'overload-resolution ambiguity: can concepts help out?'-->

## overload-resolution ambiguity: can C++20 concepts help out?

* [over.match.best.general]: desired overload must be _more constrained_
    * requires concepts like: `Matches_T1_BetterThan_T2`
    * effectively: rebuilding overload-resolution with concepts
    * feasible for two types, gets messy for 3 or more
    * technical issue: fold expression over constraints is an atomic constraint
<!--

    technical issue:
        cannot use fold-expressions to generate the required constraints
        a fold expression over constraints is an atomic constraint

-->

--- 

<!--header: 'overload-resolution ambiguity: can concepts help out?'-->

## overload-resolution ambiguity: what have we reached?
## HVFs: current best practice & overload resolution

* parameter pack + SFINAE / requires are the best approaches, but...
* overload-resolution is compromised<br/>if more than one is viable, then the call is ambiguous
* solution: the overloaded HVFs have to know of each other

--- 

<!--header: 'overload-resolution: Merge overloaded HVFs'-->
## merging overloaded HVFs: step-by-step guide

1) provide interface to specify the overloaded types
2) perform overload resolution
    * generate all possible overloads 
    * use built-in overload-resolution to resolve call
3) report selected type to the user

* uses C++17 for simplicity
* uses Peter Dimov's __Boost.MP11__
--- 

<!--header: 'overload-resolution: Merge overloaded HVFs'-->
## 1. provide interface to specify the overloaded types

* template parameter pack
* encapsulated in `mp_list` (Boost.MP11)
* on the call site <br/><pre><code><span style="color:#57a64a;">//&nbsp;list&nbsp;containing&nbsp;types&nbsp;for&nbsp;overload-resolution</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">overloaded_types</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">,</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">float</span><br/><span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#57a64a;">//&nbsp;helper&nbsp;alias&nbsp;template</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">enabler</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">decltype</span><span style="color:#b4b4b4;">(</span>enable<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">overloaded_types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;());</span><br/> <br/><span style="color:#57a64a;">//&nbsp;&quot;overloaded&quot;&nbsp;HVF&nbsp;for&nbsp;int&nbsp;and&nbsp;float</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">enabler</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;*</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">nullptr</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#9a9a9a;">args</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/&ast;&nbsp;...&nbsp;&ast;/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/></code></pre>

<!--

the caller: on the call site

-->

---


<!--header: 'overload-resolution: Merge overloaded HVFs'-->
## 2. perform overload resolution

* for each type, generate all possible overloads 
    * infinite number of overloads, but...
    * only a finite number viable: arity of the actual call
    * generate (the one) test-function for the type
* run built-in overload-resolution on all of them

---
## library for overloading HVFs (26 LOC)
<pre style="font-size:60%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...&gt;&gt;</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">integral_constant</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">,</span>&nbsp;Index<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...)</span>&nbsp;<span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">;</span><br/><span style="color:#b4b4b4;">};</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Indices</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">...</span>&nbsp;Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;...</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;::</span>test<span style="color:#b4b4b4;">...;</span>&nbsp;<br/><span style="color:#b4b4b4;">};</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence_for</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#b4b4b4;">{};</span><br/> <br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#569cd6;">decltype</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">...(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">)&gt;{}.</span>test<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#dcdcaa;">declval</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;()...))</span>&nbsp;<span style="color:#dcdcaa;">enable</span><span style="color:#b4b4b4;">();</span><br/></code></pre>

--- 

## analyzing the HVF-library: `enable` function 
<pre><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">constexpr</span>&nbsp;<br/><span style="color:#569cd6;">decltype</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">...(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">)&gt;{}.</span><span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#dcdcaa;">declval</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;()...))</span>&nbsp;<br/><span style="color:#dcdcaa;">enable</span><span style="color:#b4b4b4;">();</span><br/></code></pre>

* invoked by caller with type list and actual arguments
* return type: executes overload-resolution by calling <code><span style="color:#dcdcaa;">test</span></code>
* returns <code><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">integral_constant</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">,</span>&nbsp;Index<span style="color:#b4b4b4;">&gt;</span></code> indicating then best match

---

## analyzing the HVF-library: creating 'viable' type-lists
<pre style="font-size:70%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Indices</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">...</span>&nbsp;Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;...</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;::</span>test<span style="color:#b4b4b4;">...;</span>&nbsp;<span style="color:#57a64a;">//&nbsp;C++17&nbsp;required</span><br/><span style="color:#b4b4b4;">};</span><br/> <br/><span style="color:#57a64a;">//&nbsp;required&nbsp;to&nbsp;create&nbsp;compile&nbsp;time&nbsp;index-sequence</span><br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">overloads</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence_for</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#b4b4b4;">{};</span><br/></code></pre>

 <!--
 
 overload needed to generate index_sequence

 Explained on next slide !!!

 -->>

---

<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Indices</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">...</span>&nbsp;Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="background-color:#264f78;border-style: solid;border-width: 1px;border-color:#889bad"><span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span></span><span style="color:#b4b4b4;">&gt;...</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;::</span>test<span style="color:#b4b4b4;">...;</span><br/><span style="color:#b4b4b4;">};</span><br/></code></pre>

* generate list of desired arity for each type<br/><pre><code><span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span>Type<span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;</span>&nbsp;<span style="color:#b4b4b4;">==</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span>Type<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">...,</span>&nbsp;Type<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">&bsol;&nbsp;arity<span style="color:#b4b4b4;">-</span>times&nbsp;/</span><br/></code></pre>
* instantiate `single_function` with such a list<br/>for each 'overloaded' type using pack expansion

<!--

using is explained on the next slide !!!

-->

---

<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Indices</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">...</span>&nbsp;Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">_overloads</span><span style="color:#b4b4b4;">&lt;</span>arity<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">index_sequence</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">...&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span ><span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span></span><span style="color:#b4b4b4;">&gt;...</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="background-color:#264f78;border-style: solid;border-width: 1px;border-color:#889bad"><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Indices<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;&gt;::</span>test<span style="color:#b4b4b4;">...;</span></span><br/><span style="color:#b4b4b4;">};</span><br/></code></pre>

 C++17 `using`-declaration with pack expansion to pull all `test` functions into scope

<!--

C++17 feature
using needed since 'test' is defined in multiple bases, thus ambiguous with 'using'

-->

---

## core of the library: generate the test function
<pre><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...&gt;&gt;</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">integral_constant</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">,</span>&nbsp;Index<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...)</span>&nbsp;<span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">;</span><br/><span style="color:#b4b4b4;">};</span><br/></code></pre>
* instantiated for each overloaded type
* remember: `sizeof...(Params)` is the arity of the call
* defines `test` with desired argument-types and arity
* technique also used in STL-implementations:<br/>to generate the converting constructors in `std::variant` for the type-alternatives

--- 


<!--header: 'overload-resolution: Merge overloaded HVFs'-->
## 3. report selected type to the user

* in case of ambiguity: compilation error 
* otherwise: return index of selected type
    * as compile-time constant
    * usable in template-dispatching or  `if constexpr`

--- 
<!--header: 'overload-resolution: Merge overloaded HVFs'-->

<pre><code><span style="color:#57a64a;">//&nbsp;list&nbsp;containing&nbsp;types&nbsp;for&nbsp;overload-resolution</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">overloaded_types</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">float</span><span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#57a64a;">//&nbsp;helper&nbsp;alias&nbsp;template&nbsp;(simplifies&nbsp;repetitions)</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">enabler</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">decltype</span><span style="color:#b4b4b4;">(</span>enable<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">overloaded_types</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;());</span><br/> <br/><span style="color:#57a64a;">//&nbsp;&quot;overloaded&quot;&nbsp;function&nbsp;for&nbsp;int&nbsp;and&nbsp;float</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">enabler</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;*</span>&nbsp;&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">nullptr</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#9a9a9a;">args</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#569cd6;">auto</span>&nbsp;<span style="color:#9cdcfe;">selected_type_index</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">enabler</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;::</span>value<span style="color:#b4b4b4;">;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#d8a0df;">if</span>&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#9cdcfe;">selected_type_index</span>&nbsp;<span style="color:#b4b4b4;">==</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;&quot;int&quot;-overload&nbsp;invoked</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#c8c8c8;">cout</span>&nbsp;<span style="color:#b4b4b4;">&lt;&lt;</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">overload:&nbsp;(int,&nbsp;...)</span><span style="color:#e8c9bb;">&quot;</span>&nbsp;<span style="color:#b4b4b4;">&lt;&lt;</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#dcdcaa;">endl</span><span style="color:#b4b4b4;">;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">}</span>&nbsp;<span style="color:#d8a0df;">else</span>&nbsp;<span style="color:#d8a0df;">if</span>&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#9cdcfe;">selected_type_index</span>&nbsp;<span style="color:#b4b4b4;">==</span>&nbsp;<span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;&quot;float&quot;-overload&nbsp;invoked</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#c8c8c8;">cout</span>&nbsp;<span style="color:#b4b4b4;">&lt;&lt;</span>&nbsp;<span style="color:#e8c9bb;">&quot;</span><span style="color:#d69d85;">overload:&nbsp;(float,&nbsp;...)</span><span style="color:#e8c9bb;">&quot;</span>&nbsp;<span style="color:#b4b4b4;">&lt;&lt;</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#dcdcaa;">endl</span><span style="color:#b4b4b4;">;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">}</span><br/><span style="color:#b4b4b4;">}</span><br/></code></pre>


<!--
using allows all overloads from different base classes to take part in overload resolution
 -->

--- 
<!--header: 'overload-resolution: HVF live demo on godbolt'-->
<h1>
<a href="https://godbolt.org/z/94nGrbaxd" target="_blank">HVF live demo @ godbolt.org/z/94nGrbaxd</a>
</h1>

---

<center>

# questions?

</center>

![width:600px center](pictures/Luna_Pacino.jpg)



---
<!--header: 'hop'-->


# hop

## from HVFs to function-parameter building-blocks

<!--

extending the overload-library 

hop abbrev. for: _HO_mogeneous _P_arameters
-->
    

---
<!--header: 'hop: function-parameter building-blocks'-->

## hop: function-parameter building-blocks

required features
* create parameter-lists with type-generators
* create overload-resolution condition from overload-set
* within function implementation
    * identify active overload
    * accessing / forwarding arguments (esp. defaulted parameters)

---
<!--header: 'hop: type-generators'-->

## hop: type-generators

- any cv-qualified C++ type
- repetition
- sequencing
- alternatives
- trailing / non-trailing defaulted parameter
- forwarding references with / without condition
- template argument deduction (per argument / global / mixed)
- adapt existing functions
- tag types / overloads for easier access

---
<!--header: 'type-generators: repetition'-->
## repetition
* base template for repetition: match `_min` to `_max` times<br/><pre><code><span style="color:#57a64a;">//&nbsp;repetition</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;_min<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;_max&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#c8c8c8;">infinite</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
* syntactic sugar: specializations of `repeat`<br/><pre style="font-size:69%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;parameter&nbsp;pack</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">pack</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">infinite</span><span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;non-empty&nbsp;parameter&nbsp;pack</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">non_empty_pack</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#c8c8c8;">infinite</span><span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;optional&nbsp;parameter&nbsp;(w/o&nbsp;default&nbsp;value)</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">optional</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">size_t</span>&nbsp;_times<span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;exactly&nbsp;n-times</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">n_times</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;_times<span style="color:#b4b4b4;">,</span>&nbsp;_times<span style="color:#b4b4b4;">&gt;;</span><br/> <br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">eps</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">repeat</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">char</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">&gt;;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;no&nbsp;parameter&nbsp;(epsilon)</span><br/></code></pre>

---
<!--header: 'type-generators: sequencing'-->
## sequencing
* matching type lists one after another<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;sequencing&nbsp;parameters</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">;</span><br/></code></pre>

## alternatives
* matching exactly one of the specified type lists<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;alternative&nbsp;types</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">alt</span><span style="color:#b4b4b4;">;</span><br/></code></pre>

---
<!--header: 'type-generators: defaulted parameters'-->
## defaulted parameters
* eats argument, if it fits the type, otherwise injects default-created argument<br/><pre style="_font-size:100%"><code><span style="color:#57a64a;">//&nbsp;C++-style&nbsp;defaulted&nbsp;parameter:&nbsp;only&nbsp;at&nbsp;end&nbsp;of&nbsp;the&nbsp;parameter&nbsp;list</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">_Init</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#c8c8c8;">impl</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">default_create</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">cpp_defaulted_param</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#57a64a;">//&nbsp;general&nbsp;defaulted&nbsp;parameter:&nbsp;can&nbsp;appear&nbsp;anywhere&nbsp;in&nbsp;the&nbsp;parameter&nbsp;list</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">_Init</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#c8c8c8;">impl</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">default_create</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
<!-- 
when both are two times at the end there's a difference:
C++-style requires the first, when you want to specify the second
-->

---
<!--header: 'type-generators: forwarding references'-->
## forwarding references
* unrestricted perfect forwarding<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">fwd</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
* conditional perfect forwarding (SFINAE)<br/><pre style="_font-size:100%"><code><span style="color:#57a64a;">//&nbsp;forward-reference&nbsp;guarded&nbsp;by&nbsp;meta-function</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">_If</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">fwd_if</span><span style="color:#b4b4b4;">;</span><br/> <br/><span style="color:#57a64a;">//&nbsp;forward-reference&nbsp;guarded&nbsp;by&nbsp;quoted&nbsp;meta-function</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">_If</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">fwd_if_q</span><span style="color:#b4b4b4;">;</span><br/></code></pre>

<!-- 
when both are two times at the end there's a difference:
C++-style requires the first, when you want to specify the second
-->

---
<!--header: 'type-generators: template argument deduction'-->
## template argument deduction
* global: all instances deduce the same set of types<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">deduce</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
* local: deduced instances are independent of each other<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">deduce_local</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
* mixed: specify, which template arguments are matched globally<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">GlobalDeductionBindings</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Pattern</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">deduce_mixed</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
    * sequence of `vector<T, Alloc>` with same `T` but arbitrary `Alloc`
    * sequence of `map` with the same key-type


<!-- 
when both are two times at the end there's a difference:
C++-style requires the first, when you want to specify the second
-->

---
<!--header: 'type-generators: adapt existing functions'-->
## adapt existing functions
* adapting an existing function<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">auto</span>&nbsp;funtion<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">adapt</span><span style="color:#b4b4b4;">;</span><br/></code></pre>
* adapting overload-sets, templates, functions with defaulted parameters<br/><pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">Adapter</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">adapted</span><span style="color:#b4b4b4;">;</span><br/><span style="color:#57a64a;"><br/>//&nbsp;adapt&nbsp;overload-set&nbsp;&#39;qux&#39;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">adapt_qux</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">static</span>&nbsp;<span style="color:#569cd6;">decltype</span><span style="color:#b4b4b4;">(</span>qux<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#dcdcaa;">declval</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;()...))</span>&nbsp;<span style="color:#dcdcaa;">forward</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#d8a0df;">return</span>&nbsp;qux<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span>forward<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;(</span><span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)...);</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">}</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">};</span><br/></code></pre>


<!-- 
when both are two times at the end there's a difference:
C++-style requires the first, when you want to specify the second
-->

---
<!--header: 'type-generators: tagging types / overloads'-->
## auxiliary stuff
* tagging to refer by name instead by position 
    * tagging overloads
    * tagging types - e.g. for accessing arguments from a pack
* SFINAE condition on the whole overload

<!-- 
referring to arguments / specific overloads by tag-name instead of by position
-->

---
<!--header: 'hop-grammar for function-parameters'-->

## hop-grammar for function-parameters
<style>
.tab {display: block;
      border-style: none;
      _margin: none;
      _background-color:black
      }

</style>

<table class="tab"><tr style="background-color:#0a0e12">

<td >

<pre class="tab">
<code><span style="color:#569cd6;">CppType</span> ::=  
    <i>any (cv-qualified) C++ type</i>

<span style="color:#569cd6;">Type</span> ::=  
    <span style="color:#569cd6;">CppType</span>
    | <span style="color:#4ec9b0;">tagged_ty</span>&lt;<i>tag</i>, <span style="color:#569cd6;">Type</span>&gt;
    
<span style="color:#569cd6;">ArgumentList</span> ::=
    <span style="color:#569cd6;">Argument</span> 
    | <span style="color:#569cd6;">ArgumentList</span>, <span style="color:#569cd6;">Argument</span>
</code></pre>

</td>

<td>
<pre class="tab"><code><span style="color:#569cd6;">Argument</span> ::=
    <span style="color:#569cd6;">Type</span> 
    | <span style="color:#4ec9b0;">repeat</span>&lt;<span style="color:#569cd6;">Argument</span>, <i>min</i>, <i>max</i>&gt; 
    | <span style="color:#4ec9b0;">seq</span>&lt;<span style="color:#569cd6;">ArgumentList</span>&gt; 
    | <span style="color:#4ec9b0;">alt</span>&lt;<span style="color:#569cd6;">ArgumentList</span>&gt; 
    | <span style="color:#4ec9b0;">cpp_defaulted_param</span>&lt;<span style="color:#569cd6;">Type</span>, <i>init</i>&gt;
    | <span style="color:#4ec9b0;">general_defaulted_param</span>&lt;<span style="color:#569cd6;">Type</span>, <i>init</i>&gt; 
    | <span style="color:#4ec9b0;">fwd</span>
    | <span style="color:#4ec9b0;">fwd_if</span>&lt;<i>condition</i>&gt;
    | <span style="color:#4ec9b0;">deduce</span><span style="color:#c8c8c8;">{</span><span style="color:#4ec9b0;">_local</span><span style="color:#c8c8c8;">|</span><span style="color:#4ec9b0;">_mixed</span><span style="color:#c8c8c8;">}</span>&lt;<i>Pattern</i>&gt;
</code></pre>
</td>
</tr>
</table>
    

---
<!--header: 'hop-grammar: examples'-->

## hop-grammar: examples
* at least one `int`<pre style="_font-size:100%"><code><span style="color:#4ec9b0;">non_empty_pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;</span><br/></code></pre>
* list of `double` followed by optional `options_t`<pre style="_font-size:100%"><code><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">cpp_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">options_t</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/></code></pre>
* optional `options_t` followed by a list of `double` <pre style="_font-size:100%"><code><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">options_t</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/></code></pre>
* name-value pairs<pre style="_font-size:100%"><code><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">string</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">alt</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">bool</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">string</span><span style="color:#b4b4b4;">&gt;&gt;&gt;</span><br/></code></pre>


---

<!--
* global type deduction: a map and set having the same key-type<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T2</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">map_alias</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">map</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T2</span><span style="color:#b4b4b4;">&gt;</span><span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">&amp;;</span><br/> <br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T2</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">set_alias</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">set</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">&gt;</span><span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">&amp;;</span><br/> <br/><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">deduce</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">map_alias</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">deduce</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">set_alias</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/></code></pre>

-->

* local type deduction: a list of `vector` of arbitrary value-types<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">vector_alias</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">vector</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span><span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">&amp;;</span><br/><br/><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">deduce_local</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">vector_alias</span><span style="color:#b4b4b4;">&gt;&gt;</span></code></pre>

* mixed global/local type deduction: a pack of maps, all having the same key-type<pre style="_font-size:100%"><code><span style="color:#57a64a;">//&nbsp;T1&nbsp;is&nbsp;bound&nbsp;with&nbsp;global_deduction_binding</span><br/><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T2</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">map_alias</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">map</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T2</span><span style="color:#b4b4b4;">&gt;</span><span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">&amp;;</span><br/><br/><span style="color:#57a64a;">//&nbsp;map&nbsp;index&nbsp;in&nbsp;pattern&nbsp;to&nbsp;tag-type:</span><br/><span style="color:#57a64a;">//&nbsp;all&nbsp;deduced&nbsp;types&nbsp;with&nbsp;that&nbsp;index&nbsp;have&nbsp;to&nbsp;be&nbsp;the&nbsp;same!</span><br/><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">bindings</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">global_deduction_binding</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#b5cea8;">0</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">tag_map_key_type</span><span style="color:#b4b4b4;">&gt;&gt;;</span><br/><br/><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">deduce_mixed</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">bindings</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">map_alias</span><span style="color:#b4b4b4;">&gt;&gt;</span></code></pre>

---
<!--header: 'hop-grammar: examples'-->

* only types fitting into a pointer<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">is_small</span>&nbsp;<span style="color:#b4b4b4;">:</span>&nbsp;<span style="color:#4ec9b0;">mp_bool</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">remove_cvref_t</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;)</span>&nbsp;<span style="color:#b4b4b4;">&lt;=</span>&nbsp;<span style="color:#569cd6;">sizeof</span><span style="color:#b4b4b4;">(</span><span style="color:#569cd6;">void</span><span style="color:#b4b4b4;">*)&gt;</span>&nbsp;<span style="color:#b4b4b4;">{};</span><br/><br/><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">fwd_if</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">is_small</span><span style="color:#b4b4b4;">&gt;&gt;</span></code></pre>
* matching all above as an overload set<pre style="_font-size:100%"><code><span style="color:#569cd6;">using</span>&nbsp;<span style="color:#4ec9b0;">overloads</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#4ec9b0;">ol_list</span><span style="color:#b4b4b4;">&lt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">non_empty_pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">cpp_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">options_t</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">options_t</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">string</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">alt</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">bool</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">double</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">string</span><span style="color:#b4b4b4;">&gt;&gt;&gt;</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">deduce_local</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">vector_alias</span><span style="color:#b4b4b4;">&gt;&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">deduce_mixed</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">bindings</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">map_alias</span><span style="color:#b4b4b4;">&gt;&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">ol</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">fwd_if</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">is_small</span><span style="color:#b4b4b4;">&gt;&gt;</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#b4b4b4;">&gt;;</span><br/></code></pre>

<!--

most outer 'seq' can be dropped

potential ambiguities:
    - alternatives containing same type multiple times
    - repeat potentially empty parameter-lists
    - and a lot more!

-->

---
<!--header: 'generate type-lists for an overload'-->

## expanding the grammar: generate type-lists for an overload

* for HVFs
    * "repeat the type _arity_-times"<br/><pre><code><span style="color:#4ec9b0;">mp_repeat_c</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Types</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;arity<span style="color:#b4b4b4;">&gt;</span><br/></code></pre>

* combinatorial problem for hop
    * "generate all parameter-lists that accept _arity_ arguments"


---
<!--header: 'generate type-lists for an overload (cont.)'-->

## generate type-lists for an overload (cont.)

recursively traverse the overload
ensure progress (i.e. generate parameters) to avoid infinite recursion

1) for each sub-expression of the overload
    * analyse the min / max length of the producible type-lists
2) recursively traverse the overload
    * build types-lists <code>T<sub>1</sub>, ..., T<sub>n</sub></code> for all sub-expressions
    * generate cartesian product <code>T<sub>1</sub>&#10799;...&#10799;T<sub>n</sub></code>  and concatenate tuples
    * prohibit infinite recursion
        * use min / max info to limit to types-lists that can match the call's arity
        * ensure progress: do not repeat  type-lists

<!--

        * type of (unbounded) repetition may not generate empty type-list
        - only infinite only for unbounded, BUT would ANYWAY be ambiguous

-->

---

<!--header: 'generate test-function from type-list'-->

## generate test-function from type-list

* for HVFs
    * unpack `mp_list` and expand as arguments of `test`<br/><pre style="font-size:80%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span>&nbsp;Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">single_function</span><span style="color:#b4b4b4;">&lt;</span>Index<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...&gt;&gt;</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">integral_constant</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">size_t</span><span style="color:#b4b4b4;">,</span>&nbsp;Index<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Params</span><span style="color:#b4b4b4;">...)</span>&nbsp;<span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">;</span><br/><span style="color:#b4b4b4;">};</span><br/></code></pre>

* additional tasks for hop
    * 'templated' arguments
        * forwarding references, local / global conditions, adapters
    * template argument deduction
    * don't forget about: removing the tag-templates

---

<!--header: 'generate test-function from type-list (cont.)'-->

## generate test-function from type-list (cont.)

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span>&nbsp;<span style="color:#569cd6;">typename</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">enable_if_t</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_invoke_q</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">_If</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;::</span>value&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#4ec9b0;">deduction_helper</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">...&gt;,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">TypeList</span><span style="color:#b4b4b4;">...&gt;&gt;::</span>value<br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">*</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">=</span>&nbsp;<span style="color:#569cd6;">nullptr</span>&nbsp;<span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">constexpr</span>&nbsp;<span style="color:#4ec9b0;">result_t</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#569cd6;">typename</span>&nbsp;<span style="color:#4ec9b0;">unpack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">TypeList</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;::</span><span style="color:#4ec9b0;">type</span><span style="color:#b4b4b4;">...)</span>&nbsp;<span style="color:#569cd6;">const</span><span style="color:#b4b4b4;">;</span><br/></code></pre>


* `mp_invoke_q<_If, Args...>`  - checking global conditions against actual parameter types
* `deduction_helper` - global template argument deduction 
* `unpack` 
    * returns parameter type / generates type for forwarded parameters
    * checks local conditions
    * local template argument deduction
    * removes tag-types

---
<!--header: 'test-function: template argument deduction (in an ideal world)'-->

## test-function: template argument deduction (in an ideal world)

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">struct</span>&nbsp;<span style="color:#4ec9b0;">deducer_t</span>&nbsp;<span style="color:#b4b4b4;">{</span><br/><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">static</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">true_type</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">...&gt;&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">...&gt;...);</span><br/> <br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">static</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">false_type</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;&gt;&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(...);</span><br/><span style="color:#b4b4b4;">};</span><br/></code></pre>
a pack of deduction patterns validated simultaneously against a pack of types - simple as that, but...

---
<!--header: 'test-function: template argument deduction (in our world)'-->

## test-function: template argument deduction (in our universe)

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">template</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...&gt;</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">static</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#c8c8c8;">std</span><span style="color:#b4b4b4;">::</span><span style="color:#4ec9b0;">true_type</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">mp_list</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">...&gt;&gt;</span>&nbsp;<span style="color:#dcdcaa;">test</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Patterns</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">...&gt;...);</span><br/></code></pre>

is generally invalid C++:
* <code><span style="color:#4ec9b0;">Patterns</span></code> may be an alias template
* instantiating an alias template with a pack is not allowed
* using Boost.Preprocessor to generate type-deduction `test` functions
* configurable through macro <code><span style="color:#beb7ff;">HOP_MAX_DEDUCIBLE_TYPES</span><br/></code>

---
<!--header: 'generate test-function from type-list (reprise)'-->

## generate test-function from type-list (reprise)

what else to do:

* keep a record of formal parameter types (the type-list)
* keep a record of skipped defaulted parameters
* create result-type from all that, i.a.
    * index of overload in overload-set
    * generated type-list, skipped defaulted parameters
    * locally / globally deduced types

--- 

<!--header: 'hop live demo on godbolt'-->

## hop live demo on godbolt

<!-- <h1>
<a href="https://godbolt.org/z/94nGrbaxd" target="_blank">hop live demo on godbolt</a>
</h1> -->

- <a href="https://godbolt.org/z/esqc6GMsY">ints & floats @  godbolt.org/z/esqc6GMsY</a>
- <a href="https://godbolt.org/z/359Y34b9P">reference-types @ godbolt.org/z/359Y34b9P</a>
- <a href="https://godbolt.org/z/8oY5T7Wb6">tagging overloads @ godbolt.org/z/8oY5T7Wb6</a>
- <a href="https://godbolt.org/z/aWPMdb6jG">default arguments @ godbolt.org/z/aWPMdb6jG</a>
- <a href="https://godbolt.org/z/dWG75MPeq">one entry point to rule them all? @ godbolt.org/z/dWG75MPeq</a>
- <a href="https://godbolt.org/z/19ddvTfvW">template argument deduction @ godbolt.org/z/19ddvTfvW</a>

<!--
one entry point to rule them all? : kudos to Quuxplusone aka Arthur O'Dwyer
-->

---

## hop statistics

* currently ~1800 LOC

* grammar: ~150 LOC 

* template argument deduction: ~250 LOC 

* test-function generator: ~300 LOC 

* argument-access ~400 LOC

* overload generation: ~400 LOC <br/>in HVF-library it's just calling `mp_repeat`

---

<!--header: 'hopping on'-->

# summary
* homogenous variadic functions
    * analyzed the language support
    * overload-resolution problem for standalone implementations -> merge 'em
    * HVF library in 26 LOC
* hop
    * grammar for function-parameter building-blocks
    * generation of type-lists & overload resolution
    * tagging & accessing of overloads, types and  arguments

<!--

we did what Jon Kalb said: use MP11

-->

---
<!--header: 'hopping on'-->

# hopping on

* type-list generation improvements by early exit
    * divisibility-checks for repeated lists
    * check convertibility of argument to type
* use `concepts` to create a type-system for the hop-internal

---

<center>

# questions / remarks?

</center>

![width:600px center](pictures/Luna_Pacino.jpg)

<center>

visit [github.com/tobias-loew/hop](https://github.com/tobias-loew/hop)

</center>


---

<!-- 
    .vsselected{background-color:#264f78;border-style: solid;border-width: 1px;border-color:#889bad}
 -->


<style>
img[alt~="center"] {
  display: block;
  margin: 0 auto;
}
</style>


![width:700px center](pictures/Luna_Meersau.png)

<center>

# now, let's hop together!

</center>
