#include "PrintInterface.hpp"


PrintInterface prInt = PrintInterface::getDefault();

PrintInterface PrintInterface::getDefault(){
    static PrintInterface defaultPrintInterface;
    return defaultPrintInterface;
}

//flushes the output stream
void PrintInterface::flush(std::ostream *stream){
    stream->flush();
}
void PrintInterface::flushOutput(){
    flush(_outStream);
}
void PrintInterface::flushLog(){
    flush(_logStream);
}
void PrintInterface::send(std::string str, std::ostream *stream){
    *stream<<str;
}

// print new input line for user
void PrintInterface::printInputPointer(){
    print(_inputPointer);
    flushOutput();
}
// log new input line for user
void PrintInterface::logInputPointer(){
    log(_inputPointer);
    flushLog();
}

void PrintInterface::print(std::string str){
    send(str, _outStream);
}
void PrintInterface::log(std::string str){
    if (_logging){
        send(str, _logStream);
    }
}

void PrintInterface::println(std::string str=""){
    print(str);
    print("\n");
    flushOutput();
}
void PrintInterface::logln(std::string str=""){
    if (_logging){
        log(str);
        log("\n");
        flushLog();
    }
}

void PrintInterface::println(std::list<std::string> lst){
    for (auto str:lst){
        print(str);
    }
    print("\n");
    flushOutput();
}
void PrintInterface::logln(std::list<std::string> lst){
    if (_logging){
        for (auto str:lst){
            log(str);
        }
        log("\n");
        flushLog();
    }
}
void PrintInterface::print(std::list<std::string> lst){
    for (auto str:lst){
        print(str);
    }
    flushOutput();
}
void PrintInterface::log(std::list<std::string> lst){
    if (_logging){
        for (auto str:lst){
            log(str);
        }
        flushLog();
    }
}
void PrintInterface::setLog(bool logging){
    _logging = logging;
}
void PrintInterface::setInputPointer(std::string inputPointer){
    _inputPointer = inputPointer;
}

void PrintInterface::changeOutput(std::ostream *outStream){
    _outStream = outStream;
}
void PrintInterface::changeLogOutput(std::ostream *logStream){
    _logStream = logStream;
}