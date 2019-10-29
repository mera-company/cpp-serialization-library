/**
 * @file      metaprogramming_base.h
 *
 * @brief     Contains various useful metaprogramming definitions, which are not
 *            directly related to the MIL subject
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

#ifndef INCLUDE__METAPROGRAMMING_BASE__H
#define INCLUDE__METAPROGRAMMING_BASE__H

/**
 * @brief      mil component namespace
 */
namespace mil {
    /**
     * @brief      Compile-time list of values
     *
     * @tparam     vals
     */
    template<auto ... vals>
    struct values_list {};

    /**
     * @brief      Compile-time list of types
     *
     * @tparam     T
     */
    template<typename ... T>
    struct types_list {};

/** @{ */
/* first meta-function, which returns first type or value */
    /**
     * @brief      Initial definition
     */
    template<typename ...>
    struct first;

    /**
     * @brief      Overload for variadic type-pack
     *
     * @tparam     TFirst    First type
     * @tparam     TRest     The rest types
     */
    template<typename TFirst, typename ... TRest>
    struct first<TFirst, TRest...> {
        using type = TFirst;
    };

    /**
     * @brief      stl's _t standalone type
     *
     * @tparam     Types    types pack
     */
    template<typename ... Types>
    using first_t = typename first<Types...>::type;

/** @} */

} /* end of namespace mil */

#endif /* end of #ifndef INCLUDE__METAPROGRAMMING_BASE__H */
