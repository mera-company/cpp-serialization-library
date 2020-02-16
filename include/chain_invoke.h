/**
 * @file      chain_invoke.h
 *
 * @author    Alexander Ganyukhin (alexander.ganyukhin@mera.com)
 *
 * @date      2019-October-23
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
#ifndef INCLUDE__CHAIN_INVOKE__H
#define INCLUDE__CHAIN_INVOKE__H

/* Part of the library */
#include <function_info.h>

/* STL */
#include <tuple>
#include <array>
#include <type_traits>

/**
 * @brief      mil component namespace
 *
 * @note       MIL - Metaprogramming Invoking Library
 */
namespace mil {
using namespace msl;
    /**
     * @brief      Contains various definitions for
     */
    namespace detail {
        /**
         * @brief      Tricky meta-function, which dereferences passed argument, if
         *             the passed template type is pointer type
         *
         * @tparam     RealType     Real type of the argument
         * @tparam     PassedType   Type of passed value
         *
         * @param      aVal          The value itself
         *
         * @return     Passed arg reference OR address of the provided reference
         */
        template<typename RealType, typename PassedType>
        constexpr inline decltype(auto) conditionalAddressOf(PassedType && aVal) {
            if constexpr (std::is_pointer_v<RealType>) {
                return std::addressof(aVal);
            } else {
                return std::forward<PassedType>(aVal);
            }
        }

        /**
         * @brief      The invoking step object, which also holds the tuple
         *
         * @tparam     Fx    Type of the method
         */
        template<typename Fx>
        struct OwningInvokingStep {
            using tuple_t    = typename function_info<Fx>::stack_args;
            using qalified_t = typename function_info<Fx>::args;
            using class_t    = typename function_info<Fx>::cl;

            static constexpr size_t TUPLE_SIZE { std::tuple_size_v<tuple_t> };

            tuple_t tuple;

            /**
             * @brief      The streaming operator which simply deduces type of
             *             the next class type in a getters chain
             *
             * @tparam     OpFx   Type of the next function
             *
             * @param      aFx    The next function ptr
             *
             * @return     Next invoker
             */
            template<typename OpFx>
            constexpr auto operator<<(OpFx const & aFx) && {
                using invoking_t = typename function_info<OpFx>::cl;
                return OwningInvokingStep<OpFx>{ aFx, std::get<invoking_t>(tuple) };
            }

            /**
             * @brief      Creates the invoker and also invokes the function to
             *             fill the tuple with arguments
             *
             * @tparam     Obj
             */
            template<typename Obj>
            explicit constexpr OwningInvokingStep(Fx const & aFx, Obj & obj)
                : tuple { }
            {
                this->invokeImpl(std::make_index_sequence<TUPLE_SIZE>{}, aFx, obj);
            }
        private:
            /**
             * @brief      The implementation of the invoke, which expands tuple and invokes the
             *
             * @tparam     Obj
             * @tparam     Idx
             */
            template<typename Obj, size_t ... Idx>
            constexpr void invokeImpl(std::index_sequence<Idx...>, Fx const & aFx, Obj & obj) {
                (obj.*aFx)(conditionalAddressOf<std::tuple_element_t<Idx, qalified_t>>(std::get<Idx>(tuple))...);
            }
        };

        /**
         * @brief      The struct defines necessary stuff to start chain
         *             invoking via folding expression
         *
         * @tparam     T  type of the object
         */
        template<typename T>
        struct FoldingBeginner {
            T & obj;

            /**
             * @brief      Creates the folding beginner
             */
            explicit constexpr FoldingBeginner(T & aObj)
                : obj { aObj }
            {}

            /**
             * @brief      The streaming operator, which simply initiates the
             *             chain invoke
             *
             * @tparam     OpFx    Type of the function
             *
             * @param      aFx     Function address
             *
             * @return     Owning invoking step
             */
            template<typename OpFx>
            constexpr auto operator<<(OpFx const & aFx) && {
                return OwningInvokingStep<OpFx>{ aFx, obj };
            }
        };
    } /* end of namespace detail */

    /**
     * @brief      Invoeks the first for the object, second for the result of
     *             the first, third for result of the second an so on
     *
     * @tparam     TObj   Type of the object
     * @tparam     TFxs   Type of the function
     *
     * @param      aObj   Object
     * @param      aFxs   Functions
     *
     * @return     Result of the last function
     */
    template<typename TObj, typename ... TFxs>
    constexpr auto chainInvoke(TObj && aObj, TFxs && ... aFxs) {
        return (detail::FoldingBeginner<std::decay_t<TObj>>{ aObj } << ... << std::forward<TFxs>(aFxs)).tuple;
    }
} /* end of namespace mil */

#endif /* end of #ifndef INCLUDE__CHAIN_INVOKE__H */
