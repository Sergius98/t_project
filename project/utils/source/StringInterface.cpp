#include "StringInterface.hpp"

namespace stringInterface{
    
/*
template <typename T>
std::string StringInterface::format(std::string format_str, T t) {
    size_t start_pos = format_str.find(formater);
    if(start_pos == std::string::npos){
        throw std::invalid_argument("too many values is provided");
    }
    if (std::is_arithmetic_v<t>){
        format_str.replace(start_pos, formater.length(), std::to_string(t));
    } else {
        format_str.replace(start_pos, formater.length(), t);
    }

    start_pos = format_str.find(formater);
    if(start_pos != std::string::npos){
        throw std::invalid_argument("too little values is provided");
    }
    return format_str;
}

template<typename T, typename... Args>
std::string StringInterface::format(std::string format_str, T t, Args... args){
    size_t start_pos = format_str.find(formater);
    if(start_pos == std::string::npos){
        throw std::invalid_argument("too many values is provided");
    }
    if (std::is_arithmetic_v<t>){
        format_str.replace(start_pos, formater.length(), std::to_string(t));
    } else {
        format_str.replace(start_pos, formater.length(), t);
    }
    return format(format_str, args...) ;
}
*/

std::string StringInterface::format(std::string format_str, std::list<size_t> values){
    std::list<std::string> str_values;
    for (auto value:values){
        str_values.push_back(std::to_string(value));
    }
    return format(format_str, str_values);
}
std::string StringInterface::format(std::string format_str, std::list<std::string> values){
    for (auto value:values){
        size_t start_pos = format_str.find(formater);
        if(start_pos == std::string::npos){
            throw std::invalid_argument("too many values is provided");
        }
        format_str.replace(start_pos, formater.length(), value);
    }
    size_t start_pos = format_str.find(formater);
    if(start_pos != std::string::npos){
        throw std::invalid_argument("too little values is provided");
    }
    return format_str;
}

}
