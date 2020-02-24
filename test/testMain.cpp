/**
 * @file      testMain.cpp
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

#include <cstdint>
#include <iostream>

#include <object_invoke.h>

#ifdef PRINT_DEBUG_INFO
#error "PRINT_DEBUG_INFO already used"
#endif
//#define PRINT_DEBUG_INFO

template<typename T>
struct InstanceCounter {
    static size_t instances;

    InstanceCounter() { this->incAndLog(); }
    InstanceCounter(InstanceCounter const &) { this->incAndLog(); }
    InstanceCounter(InstanceCounter &&) noexcept { this->incAndLog(); }
    ~InstanceCounter() { this->decAndLog(); }

    InstanceCounter & operator=(InstanceCounter const &) = default;
    InstanceCounter & operator=(InstanceCounter &&) = default;

    void incAndLog() const {
        ++instances;
#ifdef PRINT_DEBUG_INFO
        std::cout << "    +++New object \'" << typeid(T).name() << "\', addr: \'"
                  << static_cast<void const *>(this) << "\' created, total: \'"
                  << instances << "\'" << std::endl;
#endif
    }

    void decAndLog() const {
        --instances;
#ifdef PRINT_DEBUG_INFO
        std::cout << "    ---Object \'" << typeid(T).name() << "\', addr: \'"
                  << static_cast<void const *>(this) << "\' destroyed, total: \'"
                  << instances << "\'" << std::endl;
#endif
    }
};

template<typename T>
size_t InstanceCounter<T>::instances { 0ull };

struct Object1
    : public InstanceCounter<Object1> {
    void CStyleGetValue(int & i) const {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << "  invoked!" << std::endl;
#endif
        ii += 1;
        i = ii;
    }

    int getValue() const
    {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << std::endl;
#endif
        ii += 1;
        return ii;
    }

    const int& refValue() const
    {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << std::endl;
#endif
        ii += 1;
        return ii;
    }

    const int* ptrValue() const
    {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << std::endl;
#endif
        ii += 1;
        return &ii;
    }

    inline static int ii { 0 };
};

struct Object2
    : public InstanceCounter<Object2> {
    void CStyleGetObject1(Object1 & obj) const {
#ifdef PRINT_DEBUG_INFO
        std::cout <<   __FUNCTION__ << " invoked!" << std::endl;
#endif
        obj = obj_;
    }

    Object1 getObject1() const {
#ifdef PRINT_DEBUG_INFO
        std::cout <<  __FUNCTION__ << " invoked" << std::endl;
#endif
        return obj_;
    }

    const Object1& refObject1() const {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked" << std::endl;
#endif
        return obj_;
    }

    const Object1& ptrObject1() const {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked" << std::endl;
#endif
        return obj_;
    }

    Object1 obj_;
};

struct Object3
    : public InstanceCounter<Object3> {
    void CStyleGetObject2(Object2 * obj) const {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked!" << std::endl;
#endif
        *obj = obj_;
    }

    Object2 getObject2() const {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked!" << std::endl;
#endif
        return obj_;
    }

    const Object2& refObject2() {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked!" << std::endl;
#endif
        return obj_;
    }

    const Object2* ptrObject2() {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked!" << std::endl;
#endif
        return &obj_;
    }

    void testFail(int c)
    {
        c = 15;
        std::cout << c << std::endl;
    }

    Object2 obj_;
};

struct Serializer {
    template<typename ... T>
    void operator()(char const * tag, std::tuple<T...> const & aTuple) {
        std::cout << "\'" << tag << "\': \'";
        putStream(std::cout, aTuple, std::make_index_sequence<sizeof...(T)>{});
        std::cout << "\'\n";
    }

    template<typename T>
    static auto& get_wrapper(const T& val)
    {
        constexpr bool is_ponter = std::is_pointer_v<std::remove_reference_t<decltype(val)>>;
        if constexpr(is_ponter) {
            return *val;
        } else {
           return val;
        }
    }

    template<typename Tuple, size_t ... Idx>
    static std::ostream & putStream(std::ostream & aOs, Tuple const & t, std::index_sequence<Idx...>) {
        (aOs << ... << get_wrapper(std::get<Idx>(t)));

        return aOs;
    }
};

template<auto val>
struct value {
    constexpr inline static auto F = val;
};

template<auto ... T>
using values_tuple = std::tuple<value<T>...>;

using obj3_methods_t    = values_tuple<&Object3::CStyleGetObject2, &Object3::getObject2, &Object3::refObject2, &Object3::ptrObject2>;
using obj2_methods_t    = values_tuple<&Object2::CStyleGetObject1, &Object2::getObject1, &Object2::refObject1, &Object2::ptrObject1>;
using obj1_methods_t    = values_tuple<&Object1::CStyleGetValue,   &Object1::getValue,   &Object1::refValue,   &Object1::ptrValue>;

template<auto ... T>
void constexpr one_test(Object3& obj, Serializer& si)
{
    constexpr mil::object_invoke invoke {
        Serializer{},
        mil::delayedInvoke<T ...>("tag1")
    };

    invoke(obj, si);

    {
        const auto invoke_forwarder = mil::delayedInvoke<T ...>("tag2");
        const auto delayed_invoker = invoke_forwarder.template getDelayedInvoke<Serializer>();

        delayed_invoker(obj, si);
    }
}

template<size_t NObj3, size_t NObj2, size_t NObj1>
void constexpr cross_test(Object3& obj, Serializer& si,
                          const obj3_methods_t& obj3_methods, const obj2_methods_t& obj2_methods, const obj1_methods_t& obj1_methods)
{
    one_test<std::get<NObj3>(obj3_methods).F, std::get<NObj2>(obj2_methods).F, std::get<NObj1>(obj1_methods).F>(obj, si);
    if constexpr(NObj2 != 0) {
        cross_test<NObj3, NObj2 - 1,NObj1>(obj, si, obj3_methods, obj2_methods, obj1_methods);

        if constexpr(NObj1 != 0) {
            cross_test<NObj3, NObj2,NObj1 - 1>(obj, si, obj3_methods, obj2_methods, obj1_methods);
        }
    }
}

template<size_t N>
void constexpr block_test()
{
    Object3 obj;
    Serializer si;

    const obj3_methods_t obj3_methods{};
    const obj2_methods_t obj2_methods{};
    const obj1_methods_t obj1_methods{};

    cross_test<N, N, N>(obj, si, obj3_methods, obj2_methods, obj1_methods);
    if constexpr (N != 0) {
        cross_test<N-1, N, N>(obj, si, obj3_methods, obj2_methods, obj1_methods);
    }
}

int main() {
    block_test<3>();
    
    return 0;
}
