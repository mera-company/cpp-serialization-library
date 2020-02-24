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
#include <functional>
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
        * @brief TypeSelector will select what tuple of agrs will get from function
        *        tuple fow univsality and compatibility
        * @tparam     have_agrs     have function args or no
        * @tparam     TRet          return type of function
        * @tparam     stack_args    tuple of args if function have_agrs
        * @details    Possible three variants which described below
        // 1) if have agrs - then return type must be void and function params must
        //    be refs or pointers. Selected type is tuple (stack_args)
        // 2) if have no agrs and return type is not ref or pointer then selected type is tuple<TRet>
        // 3) if have no agrs and return type is ref or pointer then selected type is tuple<pointer<TRet>>
        */
        template<bool have_agrs, typename TRet, typename stack_args>
        struct TypeSelector final
        {
            private:
                using stack_args_or_ret_value = std::conditional_t<have_agrs, stack_args, std::tuple<TRet>>;
                inline static constexpr bool is_ref = std::is_reference_v<TRet>;
                inline static constexpr bool is_pointer = std::is_pointer_v<TRet>;
                using no_ref_but_pointer = std::tuple<std::add_pointer_t<std::remove_reference_t<TRet>>>;
                using ret_pointer_or_ret_value = std::conditional_t<is_ref, no_ref_but_pointer, std::tuple<TRet>>;

            public:
                inline static constexpr bool have_agrs_value = have_agrs;
                inline static constexpr bool is_ref_value = is_ref;
                inline static constexpr bool is_pointer_value = is_pointer;
                inline static constexpr bool is_copy_value = !have_agrs_value && !is_ref_value && !is_pointer_value;
                using args_pointer_value = std::conditional_t<is_ref, ret_pointer_or_ret_value, stack_args_or_ret_value>;
                using type = std::conditional_t<is_pointer, ret_pointer_or_ret_value, args_pointer_value>;

                TypeSelector()
                {
                    if constexpr(have_agrs_value)
                    {
                        static_assert(std::is_same_v<void, TRet>, "if have args then return value must be void");
                    }
                    if constexpr(! have_agrs_value)
                    {
                        static_assert(! std::is_same_v<void, TRet>, "if have no args then return value can't be void");
                    }
                }
        };

        /**
         * @brief      The invoking step object, which also holds the tuple
         *
         * @tparam     Fx    Type of the method
         */
        template<typename Fx>
        struct OwningInvokingStep {            
            using function_info_t                    = function_info<Fx>;
            using ret_t                             = typename function_info_t::ret;
            inline static constexpr bool have_agrs  = function_info_t::args_count > 0;
            using type_selector_t                   = TypeSelector<have_agrs, ret_t, typename function_info_t::stack_args>;
            using tuple_t                           = typename type_selector_t::type;
            using qalified_t                        = typename function_info_t::args;
            using class_t                           = typename function_info_t::cl;

            // not use dicrectly but in constructor have static assert
            const function_info_t function_info_;

            tuple_t tuple;
            // only for call constructor type_selector_t and check static asserts
            const type_selector_t ts;

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
                if constexpr (type_selector_t::have_agrs_value)
                {
                    using invoking_t = typename function_info<OpFx>::cl;
                    return OwningInvokingStep<OpFx>{ aFx, std::get<invoking_t>(tuple) };
                }
                else
                {
                    if constexpr(type_selector_t::is_copy_value)
                    {
                        static_assert(std::tuple_size_v<tuple_t> == 1, "tuple size must be 1");
                        return OwningInvokingStep<OpFx>{ aFx, std::get<0>(tuple) };
                    }
                    else if constexpr(type_selector_t::is_ref_value)
                    {
                        static_assert(std::tuple_size_v<tuple_t> == 1, "tuple size must be 1");
                        return OwningInvokingStep<OpFx>{ aFx, std::get<0>(tuple) };
                    }
                    else if constexpr(type_selector_t::is_pointer_value)
                    {
                        static_assert(std::tuple_size_v<tuple_t> == 1, "tuple size must be 1");
                        return OwningInvokingStep<OpFx>{ aFx, std::get<0>(tuple) };
                    }
                    else
                    {
                        // type_selector_t::have_agrs_value myst be false here
                        static_assert(type_selector_t::have_agrs_value, "unknown compile time branch");
                    }
                }
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
                if constexpr (type_selector_t::have_agrs_value)
                {
                    using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                    static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");

                    this->invokeImpl(std::make_index_sequence<function_info_t::args_count>{}, aFx, obj);
                }
                else if constexpr (type_selector_t::is_copy_value)
                {
                    using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                    static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");

                    this->invokeImplWithret(aFx, obj);
                }
                else if constexpr (type_selector_t::is_ref_value)
                {
                    using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                    static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");

                    this->invokeImplWithRefret(aFx, obj);
                }
                else if constexpr (type_selector_t::is_pointer_value)
                {
                    using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                    static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");

                    this->invokeImplWithPtrret(aFx, obj);
                }
                else
                {
                    // type_selector_t::have_agrs_value myst be false here
                    static_assert(type_selector_t::have_agrs_value, "unknown compile time branch");
                }
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
                using compare_obj_t = std::remove_const_t<Obj>;
                static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");

                (obj.*aFx)(conditionalAddressOf<std::tuple_element_t<Idx, qalified_t>>(std::get<Idx>(tuple))...);
            }
            template<typename Obj, size_t ... Idx>
            constexpr void invokeImpl(std::index_sequence<Idx...>, Fx const & aFx, Obj* obj) {
                using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");
                if(obj == nullptr) {
                    throw std::runtime_error(std::string{"object nullptr at "} + __FUNCTION__);
                }

                (obj->*aFx)(conditionalAddressOf<std::tuple_element_t<Idx, qalified_t>>(std::get<Idx>(tuple))...);
            }

            template<typename Obj>
            constexpr void invokeImplWithret(Fx const & aFx, Obj & obj) {
                using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");
                if constexpr(std::is_pointer_v<Obj>){
                    if(obj == nullptr) {
                        throw std::runtime_error(std::string{"object nullptr at "} + __FUNCTION__);
                    }
                }

                std::get<0>(tuple) = std::invoke(aFx, obj);
            }

            template<typename Obj>
            constexpr void invokeImplWithRefret(Fx const & aFx, Obj & obj) {
                using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");
                if constexpr(std::is_pointer_v<Obj>){
                    if(obj == nullptr) {
                        throw std::runtime_error(std::string{"object nullptr at "} + __FUNCTION__);
                    }
                }

                decltype(auto) ret_val = std::invoke(aFx, obj);
                static_assert(std::is_reference_v<decltype(ret_val)>, "ret value must be reference");
                std::get<0>(tuple) = &ret_val;
            }

            template<typename Obj>
            constexpr void invokeImplWithPtrret(Fx const & aFx, Obj & obj) {
                // sometimes object can be ref to pointer
                using compare_obj_t = std::remove_const_t<std::remove_pointer_t<Obj>>;
                static_assert(std::is_same_v<std::remove_const_t<class_t>, compare_obj_t>, "must be one type");
                if constexpr(std::is_pointer_v<Obj>){
                    if(obj == nullptr) {
                        throw std::runtime_error(std::string{"object nullptr at "} + __FUNCTION__);
                    }
                }

                decltype(auto) ret_val = std::invoke(aFx, obj);
                static_assert(std::is_pointer_v<decltype(ret_val)>, "ret value must be pointer");
                std::get<0>(tuple) = ret_val;
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
