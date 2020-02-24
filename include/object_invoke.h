/**
 * @file      object_invoke.h
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
#ifndef INCLUDE__OBJECT_INVOKE__H
#define INCLUDE__OBJECT_INVOKE__H

/* library parts */
#include <chain_invoke.h>
#include <function_info.h>
#include <metaprogramming_base.h>

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
/**
 * @brief      detail component namespace
 */
namespace detail {
    /**
     * @brief      The delayed invoke holds information about methods chain to
     *             be invoked. Invokes it and passes the result into the invoker
     *
     * @tparam     TObjectType        Type of the object
     * @tparam     TResultAcceptor    Type of the acceptor
     */
    template<typename TObjectType, typename TResultAcceptor>
    struct delayed_invoke {
        using object_t   = TObjectType;
        using acceptor_t = TResultAcceptor;

        using invoker_ptr_t = void(*)(object_t &, char const *, acceptor_t &);

        /**
         * @brief      Creates the delayed invoker
         *
         * @tparam     fx     Functions chain to invoke
         *
         * @param[in]  <pos>  Value list forwarder
         * @param[in]  aTag   Associated tag
         */
        template<auto ... fx>
        explicit constexpr delayed_invoke(values_list<fx...>, char const * aTag)
            : m_invokerPtr { &theInvoker<fx...> }
            , m_tag        { aTag               }
        {}

        /**
         * @brief      Constexpr invoke operator, executes the memorized methods
         *             chain for the object, and passes the result into the
         *             acceptor
         *
         * @param      aObject      Object to invoke
         * @param      aAcceptor    Acceptor to pass the value
         */
        constexpr void operator()(object_t & aObject, acceptor_t & aAcceptor) const {
            (*m_invokerPtr)(aObject, m_tag, aAcceptor);
        }
        constexpr void operator()(object_t* aObject, acceptor_t & aAcceptor) const {
            (*m_invokerPtr)(*aObject, m_tag, aAcceptor);
        }
    private:
        /**
         * @brief      The private invoker, performs chain invoke for the
         *             arguments for the object passed, marks with tag and
         *             passes all the results into the acceptor
         *
         * @tparam     fx         Method addresses
         * @param      aObject    Object to start chain from
         * @param      aTag       Associated tag
         * @param      aAcceptor  The acceptor
         */
        template<auto ... fx>
        static constexpr void theInvoker(object_t & aObject, char const * aTag, acceptor_t & aAcceptor) {
            aAcceptor(aTag, chainInvoke(aObject, fx...));
        }

        /**
         * @brief      Pointer to a concrete invoker specialization
         */
        invoker_ptr_t m_invokerPtr = nullptr;

        /**
         * @brief      Associated tag
         */
        char const *  m_tag = nullptr;
    };

    /** @{ */
    /* first class meta-function */
        /**
         * @brief      Returns class of the first method
         *
         * @tparam     F    First type
         * @tparam     R    Rest types
         */
        template<typename F, typename ... R>
        struct first_class {
            using type = typename function_info<F>::cl;
        };
    /** @} */

        /**
         * @brief      Supporting forwarding type
         *
         * @tparam     fx
         */
        template<auto ... fx>
        struct delayed_invoke_forwarder {
            using cl = typename first_class<decltype(fx)...>::type;

            /**
             * @brief      Creates the forwarder
             *
             * @param      Tag to use
             */
            explicit constexpr delayed_invoke_forwarder(char const * aTag)
                : m_tag { aTag }
            {}

            /**
             * @brief      Get the Delayed Invoke object
             *
             * @tparam     TResultAcceptor    The planned acceptor type
             *
             * @return     Delayed invoker type
             */
            template<typename TResultAcceptor>
            constexpr auto getDelayedInvoke() const noexcept {
                return delayed_invoke<cl, TResultAcceptor>{ values_list<fx...>{}, m_tag };
            }
        private:
            /**
             * @brief      Associated tag
             */
            char const * m_tag;
        };
    } /* end of namespace detail */

    /**
     * @brief      Creates the delayed invoke (used to pass method list into the
     *             object_invoke)
     *
     * @tparam     fx     Methods to invoke
     *
     * @param      tag    Associated tag
     *
     * @return     Delayed invoke object
     */
    template<auto ... fx>
    constexpr auto delayedInvoke(char const * tag) {
        return detail::delayed_invoke_forwarder<fx...>{ tag };
    }


    /**
     * @brief      The object invoke which allows to "memorize" the necessary
     *             methods calls, and to call them later in a certain order
     *
     * @tparam     TObjectType        Type of the object
     * @tparam     N                  Number of parts
     * @tparam     TResultAcceptor    Callable object, which invoked with the
     *                                tag and the result of the function
     */
    template<typename TObjectType, size_t N, typename TResultAcceptor>
    struct object_invoke {
    public:
        using object_t         = TObjectType;
        using acceptor_t       = TResultAcceptor;

        using delayed_invoke_t = detail::delayed_invoke<object_t, acceptor_t>;


        /**
         * @brief      Creates the object invoke object,
         *
         * @tparam     TInvokers    Types if the invokers
         */
        template<typename ... TInvokers>
        explicit constexpr object_invoke(acceptor_t, TInvokers && ... aInvokers)
            : m_delayed_invokers { aInvokers.template getDelayedInvoke<acceptor_t>()... }
        {}

        /**
         * @brief      Invokes all the registered invokers and passes every
         *             result into the acceptor
         */
        constexpr void operator()(object_t & aObj, acceptor_t & aAcceptor) const {
            for (auto const & invoker: m_delayed_invokers) {
                invoker(aObj, aAcceptor);
            }
        }
    private:
        std::array<delayed_invoke_t, N>   m_delayed_invokers;
    };

    /* class deduction guides */
    template<typename TResultAcceptor, typename ... T>
    explicit object_invoke(TResultAcceptor, T ...) -> object_invoke<typename first_t<T...>::cl, sizeof...(T), TResultAcceptor>;

} /* end of namespace mil */

#endif /* end of #ifndef INCLUDE__OBJECT_INVOKE__H */
