#include <cstdint>
#include <iostream>
#include <chain_invoker.h>

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
        // return 100;std::cout << "New object \'" << typeid(T).name() << "\', addr: \'"
        //           << static_cast<void const *>(this) << "\' created, total: \'"
        //           << instances << "\'" << std::endl;
    }

    void decAndLog() const {
        --instances;
        // std::cout << "Object \'" << typeid(T).name() << "\', addr: \'"
        //           << static_cast<void const *>(this) << "\' destroyed, total: \'"
        //           << instances << "\'" << std::endl;
    }
};

template<typename T>
size_t InstanceCounter<T>::instances { 0ull };

struct Object1
    : public InstanceCounter<Object1> {
    void getValue(int & i) {
    //    std::cout << " getValue invoked!" << std::endl;
        static int ii { 11111 };
        ii += 11111;
        i = ii;
    }
};

struct Object2
    : public InstanceCounter<Object2> {
    void getObject1(Object1 & obj) {
      //  std::cout << "  getObject1 invoked!" << std::endl;
        (void)obj;
    }
};

struct Object3
    : public InstanceCounter<Object3> {
    void getObject2(Object2 * obj) {
        //std::cout << "  getObject2 invoked!" << std::endl;
    }
};

constexpr msl::serializer ser {
    msl::makeSerializer<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call1"),
    msl::makeSerializer<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call2"),
    msl::makeSerializer<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call3"),
    msl::makeSerializer<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call4")
};

int main() {
    Object3 obj {};
    msl::SerializationInterface si;

    ser(obj, si);
}
