#include <iostream>
#include <string>
#include <../include/hop.hpp>
#include <../include/hop_utils.hpp>
#include <map>
#include <set>
#include <vector>


// list containing types for overload-resolution
using overloads_t = hop::ol_list<
    hop::ol<hop::pack<int const&>>,
    hop::ol<hop::pack<int&>>,
    hop::ol<hop::pack<int&&>>
>;

// "overloaded" function for int and float
template<typename... Args, hop::enable_t<overloads_t, Args...>* = nullptr >
void foo(Args&& ... args) {

    using ols = hop::enable_t<overloads_t, Args...>;

    constexpr auto selected_type_index = hop::index<ols>::value;
    if constexpr (selected_type_index == 0) {
        // "int"-overload invoked
        ((std::cout << "int const&-overload, arguments: ") << ... << (hop::utils::to_string_annotate_type(std::forward<Args>(args)) + ", ")) << std::endl;
    } else if constexpr (selected_type_index == 2) {
        // "int"-overload invoked
        ((std::cout << "int&-overload, arguments: ") << ... << (hop::utils::to_string_annotate_type(std::forward<Args>(args)) + ", ")) << std::endl;
    } else if constexpr (selected_type_index == 3) {
        // "int"-overload invoked
        ((std::cout << "int&&-overload, arguments: ") << ... << (hop::utils::to_string_annotate_type(std::forward<Args>(args)) + ", ")) << std::endl;
    }

}

using options_t = int;
using namespace hop;
using namespace std;

using h = ol<pack<seq<string, alt<bool, int, double, string>>>>;



template<class T1, class T2>
using map_alias = map<T1, T2>const&;

template<class T1, class T2>
using set_alias = set<T2>const&;

//ol<pack<fwd_if_q<map_alias>, deduce<set_alias>>>

template<class T, int i>
using Array = std::vector<T>;

using R = int;
using T = int;

R f(T);   R f(T, T);   R f(T, T, T);   ...

int main() {

    Array<double, 1> vec(1000);   
    Array<double, 2> matrix(6, 100);
    Array<double, 3> cube(2, 3, 5);

    foo(1, 2);
    foo('a', 'b');
    foo(1.7, 2.4f);
    int n = 5;
    int const m = 6;
    foo(n, m);
    foo(n, n);
    //foo();
}
