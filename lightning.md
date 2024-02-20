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
<span style='color:white; font-size: 125%;'>lightning talk - Tobias Loew</span> -->
<span style='color:white; font-size: 170%; font-weight: 500'>homogenous variadic functions - <br/>a lightning-library approach in 12 sec/LOC</span>
<span style='color:rgb(140,192,96); ; font-size: 100%;'>Tobias Loew - lightning talk</span>
<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'>come down with me into the rabbit hole!</span><img src="pictures/Luna_Meersau.png" style=" width:560px" id="hp"/>
</div>

<!-- <img src="pictures/Luna_Meersau.png" style=" width:400px; float:right" /><div style="clear:both; text-align:right"><h2>come and hop with me!</h2></div> -->


---
<!--header: ' '-->


<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'><br/>bunnies love tunnels<br/><br/><br/><br/><br/><br/></span><img src="pictures/accelerator.png" style=" width:520px" id="hp"/>
</div>

---
<!--header: ' '-->


<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'><br/>bunnies love tunnels<br/>cycle very fast<br/><br/><br/><br/><br/></span><img src="pictures/accelerator_running.png" style=" width:520px" id="hp"/>
</div>


---
<!--header: ' '-->


<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'><br/>bunnies love tunnels<br/>cycle very fast<br/>travel back in time<br/><br/><br/><br/></span><img src="pictures/time_travel.jpg" style=" width:520px" id="hp"/>
</div>

---
<!--header: ' '-->


<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'><br/>bunnies love tunnels<br/>cycle very fast<br/>travel back in time<br/>fix C++ right from the start<br/><br/><br/></span><img src="pictures/fixing_cpp_sm.png" style=" width:520px" id="hp"/>
</div>

---
<!--header: ' '-->


<div class="box" >
<span style='color:white; font-size: 150%;font-weight: 500'><br/>bunnies love tunnels<br/>cycle very fast<br/>travel back in time<br/>fix C++ right from the start<br/>and we're done!<br/><br/></span><img src="pictures/relaxing.png" style=" width:520px" id="hp"/>
</div>


---
<!--header: ''-->

<center>

<span style='color:white; font-size: 250%;font-weight: 500'>back to our timeline...</span>

</center>

---
<!--header: ''-->
 
# homogeneous variadic function (HVF)
- function (overload-set) with an arbitrary number of arguments, all of the same type<br/><pre style="_font-size:100%"><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t1</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t2</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t3</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t3</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span>&nbsp;<span style="color:#9a9a9a;">t4</span><span style="color:#b4b4b4;">);</span><br/><span style="color:#b4b4b4;">...</span><br/></code></pre>
- proposal for language support (P1219R2): <code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span></code> - buried in Belfast 2019


---
<!--header: ' '-->

## C++20 best-practice: parameter pack + requires

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">requires</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#4ec9b0;">T</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span>&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;all&nbsp;args&nbsp;are&nbsp;convertible&nbsp;to&nbsp;T</span><br/><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">args</span><span style="color:#b4b4b4;">)</span><br/><span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*...*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/></code></pre>

- pros
    - works for any number of arguments
    - matches only valid input
- cons
    - none (so far)


---

<!--header: ' '-->

## overload-resolution

HVFs for `int` and `float`
<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">requires</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;int&nbsp;args</span><br/><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">args</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{}</span><br/> <br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&gt;</span><br/><span style="color:#569cd6;">requires</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">float</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span>&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;float&nbsp;args</span><br/><span style="color:#569cd6;">void</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Args</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">args</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{}</span><br/></code></pre>


<pre><code><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(a)</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">-</span><span style="color:#b5cea8;">2.4f</span><span style="color:#b4b4b4;">);</span>&nbsp;<span style="color:#57a64a;">//&nbsp;(b)</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;(c)</span><br/></code></pre>

which overload of <code><span style="color:#dcdcaa;">foo</span></code> is called?

---
<!--header: ' '-->

![center](pictures/fake_poll_results.png)

---
<!--header: ''-->

let's ask the compiler <a href="https://godbolt.org/z/rEja84djj" target="_blank">overload test @ godbolt.org/z/rEja84djj</a>


---
<!--header: ''-->

![bg right:50% width:500px ](pictures/Luna_Pacino.jpg)

<center>

## concepts won't help!

</center>

--- 

<!--header: ' '-->
## merge overloaded HVFs 

![bg right:30% width:450px ](pictures/hop_experts_clipped.jpg)

1. put types into `mp_list`
2. generate homogenous test-functions with call's arity
3. run built-in overload resolution
3. report selected type to the user (fail on ambiguity)

hey Jon look: __Boost.MP11__ is used!

<center>
<a href="https://godbolt.org/z/bj1n51fWf" target="_blank">HVF-library @ godbolt.org/z/bj1n51fWf</a>
<br/><br/>

visit [github.com/tobias-loew/hop](https://github.com/tobias-loew/hop)

</center>


---
<!--header: ' '-->

<center>

# that's all rodents!

</center>

![bg width:600px right](pictures/Luna_Meersau.png)

<center>

visit [github.com/tobias-loew/hop](https://github.com/tobias-loew/hop)

and let's hop together!

</center>


