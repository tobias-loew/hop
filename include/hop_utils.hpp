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

#ifndef HOP_HOP_UTILS_HPP_INCLUDED
#define HOP_HOP_UTILS_HPP_INCLUDED


#include <type_traits>
#include <string>

namespace hop {

    // utility functions
    namespace utils {
        namespace aux {
            using std::to_string;
            std::string to_string(std::string s) { return s; }
        }

        // print out simple values, annotate type
        template<typename Arg>
        inline std::string annotate_type(Arg&& arg) {
            return std::string(typeid(arg).name())
                + (std::is_const_v<std::remove_reference_t<Arg>> ? " const" : "")
                + (std::is_rvalue_reference_v<Arg> ? "&&" : std::is_lvalue_reference_v<Arg> ? "&" : "");
        }


        // print out simple values, annotate type
        template<typename Arg>
        inline std::string to_string_annotate_type(Arg&& arg) {
            return aux::to_string(arg) + ':' + annotate_type(std::forward<Arg>(arg));
        }
    }

}

#endif // HOP_HOP_UTILS_HPP_INCLUDED
