/**
 * @file      chain_invoker.h
 *
 * @author    Alexander Ganyukhin (alexander.ganyukhin@mera.com)
 *
 * @date      2019-October-12
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

#ifndef INCLUDE__CHAIN_INVOKER__H
#define INCLUDE__CHAIN_INVOKER__H

#include <function_info.h>

#include <tuple>
#include <type_traits>


/**
 * @brief      msl component namespace
 */
namespace msl {
    /**
     * @brief      detail component namespace
     */
    namespace detail {
        template<typename Arg, typename T>
        inline decltype(auto) cndAddrOf (T& t) {
            if constexpr (std::is_pointer_v<Arg>) {
                return std::addressof(t);
            } else {
                return t;
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
            constexpr OwningInvokingStep(Fx const & aFx, Obj & obj)
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
                (obj.*aFx)(cndAddrOf<std::tuple_element_t<Idx, qalified_t>>(std::get<Idx>(tuple))...);
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

            constexpr FoldingBeginner(T & aObj)
                : obj { aObj }
            {}

            template<typename OpFx>
            constexpr auto operator<<(OpFx const & aFx) && {
                using invoking_t = typename function_info<OpFx>::cl;
                return OwningInvokingStep<OpFx>{ aFx, obj };
            }
        };
    } /* end of namespace detail */

    template<typename TObj, typename ... TFxs>
    auto chainInvoke(TObj && aObj, TFxs && ... aFxs) {
        return (detail::FoldingBeginner<std::decay_t<TObj>>{ aObj } << ... << std::forward<TFxs>(aFxs)).tuple;
    }





} /* end of namespace msl */

#endif /* end of #ifndef INCLUDE__CHAIN_INVOKER__H */
