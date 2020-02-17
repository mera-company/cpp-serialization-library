/**
 * @file      function_info.h
 *
 * @author    Alexander Ganyukhin (alexander.ganyukhin@mera.com)
 *
 * @date      2019-October-11
 *
 * Copyright 2019 Mera
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef INCLUDE__FUNCTION_INFO__H
#define INCLUDE__FUNCTION_INFO__H

#include <tuple>
#include <type_traits>

/**
 * @brief      msl component namespace
 */
namespace msl {
    /**
     * @brief      detail
     */
    namespace detail {
        /**
         * @brief      Helping type-defing structure
         *
         * @tparam     Ret     Returned  type
         * @tparam     Class   Class     type
         * @tparam     Args    Arguments type
         */
        template<typename Ret, typename Class, typename ... Args>
        struct method_function_info {
            using ret        = Ret;
            using cl         = Class;
            using args       = std::tuple<Args...>;
            inline static constexpr size_t args_count = sizeof...(Args);
            using stack_args = std::tuple<
                                        std::remove_pointer_t<
                                            std::decay_t<Args>
                                        >...
                            >;
        };

        template<typename ... TArgs>
        void checkThanRefOrPointer()
        {
            static_assert(((std::is_reference_v<TArgs> || std::is_pointer_v<TArgs>) && ...), "must be pointer or reference");
        }

    } /* end of namespace detal */

// this macro will be unded in this file
#ifdef MARCRO_CHECK_THAN_REF_OR_POINTER
#error "MARCRO_CHECK_THAN_REF_OR_POINTER already defined"
#endif
#define MARCRO_CHECK_THAN_REF_OR_POINTER(ClassName)\
    {\
        ClassName()\
        {\
            detail::checkThanRefOrPointer<Args...>();\
        }\
    };
// endmacro defrnition

    /**
     * @brief      The function info base definition
     *
     * @tparam     <arg>    Any function type
     */
    template<typename>
    struct function_info;

    /**
     * @brief      The partial specialization for qualifiers
                 ""
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) >
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "&"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) &>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)

    /**
     * @brief      The partial specialization for qualifiers
                 "& noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) & noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "&&"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) &&>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "&& noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) && noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const &"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const &>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const & noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const & noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const &&"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const &&>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const && noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const && noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile &"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile &>
        : detail::method_function_info<Ret, Class, Args...>\
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile & noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile & noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile &&"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile &&>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "volatile && noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) volatile && noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile &"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile &>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile & noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile & noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile &&"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile &&>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)


    /**
     * @brief      The partial specialization for qualifiers
                 "const volatile && noexcept"
    *
    * @tparam     Ret      Type of the returned value
    * @tparam     Class    Object type
    * @tparam     Args     Type of the arguments
    */
    template<typename Ret, typename Class, typename ... Args>
    struct function_info<Ret(Class::*)(Args...) const volatile && noexcept>
        : detail::method_function_info<Ret, Class, Args...>
MARCRO_CHECK_THAN_REF_OR_POINTER(function_info)

#ifdef MARCRO_CHECK_THAN_REF_OR_POINTER
#undef MARCRO_CHECK_THAN_REF_OR_POINTER
#endif

} /* end of namespace msl */

#endif /* end of #ifndef INCLUDE__FUNCTION_INFO__H */
