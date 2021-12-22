#ifndef STRINGINTERFACE_HPP
#define STRINGINTERFACE_HPP
#include <string>
#include <list>
#include <stdexcept>
#include <type_traits>

namespace stringInterface{

const std::string formater="{}";

class StringInterface{
    public:
        // replacement for std::format, since it's not yet implemented by the compiler
        /*
        template<typename T>
        static std::string format(std::string format_str, T t);
        template<typename T, typename... Args>
        static std::string format(std::string format_str, T t, Args... args);
        */
        // todo: replace with above
        static std::string format(std::string format_str, std::list<std::string> values);
        static std::string format(std::string format_str, std::list<size_t> values);
};

using StrInt = StringInterface;

}
#endif