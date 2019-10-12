#!/usr/bin/python3

singleEntry = """
/**
 * @brief      The partial specialization for qualifiers
               "{0}"
 *
 * @tparam     Ret      Type of the returned value
 * @tparam     Class    Object type
 * @tparam     Args     Type of the arguments
 */
template<typename Ret, typename Class, typename ... Args>
struct function_info<Ret(Class::*)(Args...) {0}>
    : method_function_info<Ret, Class, Args...> {{}};
"""

cvQualifiers  = ( "", "const", "volatile", "const volatile" )
refQualifiers = ( "", "&", "&&")
noexcept      = ( "", "noexcept")



def main():
    for cvq in cvQualifiers:
        for rfq in refQualifiers:
            for ne in noexcept:
                qualStr = ''
                if cvq:
                    qualStr += cvq + ' '

                if rfq:
                    qualStr += rfq + ' '

                if ne:
                    qualStr += ne + ' '

                if qualStr:
                    qualStr = qualStr[:-1]


                print(singleEntry.format(qualStr))

if __name__ == "__main__":
    main()
