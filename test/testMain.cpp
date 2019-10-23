#include <cstdint>
#include <iostream>

#include <object_invoke.h>

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
        std::cout << "    +++New object \'" << typeid(T).name() << "\', addr: \'"
                  << static_cast<void const *>(this) << "\' created, total: \'"
                  << instances << "\'" << std::endl;
    }

    void decAndLog() const {
        --instances;
        std::cout << "    ---Object \'" << typeid(T).name() << "\', addr: \'"
                  << static_cast<void const *>(this) << "\' destroyed, total: \'"
                  << instances << "\'" << std::endl;
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

struct Serializer {
    template<typename ... T>
    void operator()(char const * tag, std::tuple<T...> const & aTuple) {
        std::cout << "\'" << tag << "\': \'";
        putStream(std::cout, aTuple, std::make_index_sequence<sizeof...(T)>{});
        std::cout << "\'\n";
    }

    template<typename Tuple, size_t ... Idx>
    static std::ostream & putStream(std::ostream & aOs, Tuple const & t, std::index_sequence<Idx...>) {
        return (aOs << ... << std::get<Idx>(t));
    }
};



constexpr mil::object_invoke invoke {
    mil::useAcceptor<Serializer>(),
    mil::delayedInvoke<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call1"),
    mil::delayedInvoke<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call2"),
    mil::delayedInvoke<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call3"),
    mil::delayedInvoke<&Object3::getObject2, &Object2::getObject1, &Object1::getValue>("call4")
};


int main() {
    Object3 obj {};
    Serializer si;

    invoke(obj, si);
}
