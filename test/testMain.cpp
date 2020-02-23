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
#erro "PRINT_DEBUG_INFO already used"
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
    void CStyleGetValue(int & i) {
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
    void CStyleGetObject1(Object1 & obj) {
#ifdef PRINT_DEBUG_INFO
        std::cout <<   __FUNCTION__ << " invoked!" << std::endl;
#endif
        obj = obj_;
    }

    Object1 retObject1() const {
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

    Object1 obj_;
};

struct Object3
    : public InstanceCounter<Object3> {
    void CStyleGetObject2(Object2 * obj) {
#ifdef PRINT_DEBUG_INFO
        std::cout << __FUNCTION__ << " invoked!" << std::endl;
#endif
        *obj = obj_;
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


constexpr mil::object_invoke invoke {    
    Serializer{},
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::CStyleGetObject1, &Object1::CStyleGetValue>("call1"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::CStyleGetObject1, &Object1::CStyleGetValue>("call2"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::CStyleGetObject1, &Object1::CStyleGetValue>("call3"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::CStyleGetObject1, &Object1::CStyleGetValue>("call4"),

    // this case not compile. it is okay because getters C style must accept only refs and pointers
    //mil::delayedInvoke<&Object3::testFail>("call5")

    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::CStyleGetObject1, &Object1::getValue>("call6"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::retObject1, &Object1::getValue>("call7"),

    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::retObject1, &Object1::refValue>("call8"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::retObject1, &Object1::ptrValue>("call9"),
    mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::refObject1, &Object1::ptrValue>("call9"),

    mil::delayedInvoke<&Object3::refObject2, &Object2::refObject1, &Object1::ptrValue>("call9")
};


int main() {
    Object3 obj;
    Serializer si;

    {
        const auto invoke_forwarder = mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::retObject1, &Object1::ptrValue>("separate_test_1");
        const auto delayed_invoker = invoke_forwarder.template getDelayedInvoke<Serializer>();

        delayed_invoker(obj, si);
    }
    {
        const auto invoke_forwarder = mil::delayedInvoke<&Object3::CStyleGetObject2, &Object2::refObject1, &Object1::ptrValue>("separate_test_2");
        const auto delayed_invoker = invoke_forwarder.template getDelayedInvoke<Serializer>();

        delayed_invoker(obj, si);
    }

    invoke(obj, si);
    
    return 0;
}
