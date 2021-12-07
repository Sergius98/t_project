#ifndef STRINGINTERFACE_HPP
#define STRINGINTERFACE_HPP
#include <string>
#include <list>
#include <stdexcept>
#include <type_traits>


class StringInterface{
    public:
        static StringInterface getDefault();
        // replacement for std::format, since it's not yet implemented by the compiler
        template<typename T>
        std::string format(std::string format_str, T t);
        template<typename T, typename... Args>
        std::string format(std::string format_str, T t, Args... args);
        // todo: replace with above
        std::string format(std::string format_str, std::list<std::string> values);
        std::string format(std::string format_str, std::list<size_t> values);
        std::string format(std::string format_str, std::string formater, std::list<std::string> values);
        std::string format(std::string format_str, std::string formater, std::list<size_t> values);
        void setFormatSymbol(std::string formater);
    private:
        std::string _formater = "{}";
};
#endif