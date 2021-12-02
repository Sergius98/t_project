#include "StringInterface.hpp"

StringInterface strInt = StringInterface::getDefault();


StringInterface StringInterface::getDefault(){
    static StringInterface defaultStringInterface;
    return defaultStringInterface;
}
std::string StringInterface::format(std::string format_str, std::list<std::string> values){
    return format(format_str, _formater, values);
}
std::string StringInterface::format(std::string format_str, std::list<size_t> values){
    return format(format_str, _formater, values);
}
std::string StringInterface::format(std::string format_str, std::string formater, std::list<size_t> values){
    std::list<std::string> str_values;
    for (auto value:values){
        str_values.push_back(std::to_string(value));
    }
    return format(format_str, _formater, str_values);
}
std::string StringInterface::format(std::string format_str, std::string formater, std::list<std::string> values){
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

void StringInterface::setFormatSymbol(std::string formater){
    _formater = formater;
}