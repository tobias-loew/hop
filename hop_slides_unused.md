---

<!--header: 'overload-resolution'-->

<div style="margin-left: auto;margin-right: auto">
<span style="font-weight:bold;font-size:600%;color:#FFFFFF;">WRONG</span>
</div>

---

<!--header: 'overload-resolution'-->

<div style="margin-left: auto;margin-right: auto">
<span style="font-weight:bold;font-size:300%;color:#FFFFFF">all 3 calls are ambiguous</span>
</div>

<br/>

<pre style="_font-size:100%"><code><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">int</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/><br/><span style="color:#569cd6;">template</span>&nbsp;<span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">class</span><span style="color:#b4b4b4;">...</span>&nbsp;<span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&gt;</span><br/>&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#569cd6;">requires</span><span style="color:#b4b4b4;">(</span><span style="color:#c8c8c8;">is_convertible_v</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;color:#dcdcaa;font-weight:bold;">float</span><span style="color:#b4b4b4;">&gt;</span>&nbsp;<span style="color:#b4b4b4;">&amp;&amp;</span>&nbsp;<span style="color:#b4b4b4;">...)</span><br/><span style="color:#4ec9b0;">R</span>&nbsp;<span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#4ec9b0;">Ts</span><span style="color:#b4b4b4;">&amp;&amp;...</span>&nbsp;<span style="color:#9a9a9a;">ts</span><span style="color:#b4b4b4;">)</span>&nbsp;<span style="color:#b4b4b4;">{</span>&nbsp;<span style="color:#57a64a;">/*&nbsp;...&nbsp;*/</span>&nbsp;<span style="color:#b4b4b4;">}</span><br/><br/><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">2</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">0.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b4b4b4;">-</span><span style="color:#b5cea8;">2.4f</span><span style="color:#b4b4b4;">);</span>&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/><span style="color:#dcdcaa;">foo</span><span style="color:#b4b4b4;">(</span><span style="color:#b5cea8;">1.5f</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#b5cea8;">3</span><span style="color:#b4b4b4;">);</span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span style="color:#57a64a;">//&nbsp;error: ambiguous</span><br/></code></pre>



---
<!--header: 'hop-grammar: potential ambiguities'-->

## hop-grammar: potential ambiguities
* alternatives containing same type multiple times<pre style="_font-size:100%"><code><span style="color:#4ec9b0;">alt</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">,</span>&nbsp;<span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;</span><br/></code></pre>
* repetition with arguments that accept empty parameter-lists<pre ><code><span style="color:#4ec9b0;">pack</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/><br/><span style="color:#57a64a;">//&nbsp;ambigous,&nbsp;when&nbsp;called&nbsp;with&nbsp;one&nbsp;argument</span><br/><span style="color:#4ec9b0;">seq</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;,</span>&nbsp;<span style="color:#4ec9b0;">general_defaulted_param</span><span style="color:#b4b4b4;">&lt;</span><span style="color:#569cd6;">int</span><span style="color:#b4b4b4;">&gt;&gt;</span><br/></code></pre>
* and a lot more!
