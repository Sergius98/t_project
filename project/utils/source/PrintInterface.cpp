#include "PrintInterface.hpp"

namespace printInterface{

bool PrintInterface::_logging = true;
std::string PrintInterface::_inputPointer = ">";
std::ostream *PrintInterface::_outStream {&std::cout};
std::ostream *PrintInterface::_logStream {&std::cout};

//flushes the output stream
void PrintInterface::flush(std::ostream *stream){
    stream->flush();
}
void PrintInterface::flushOutput(){
    flush(PrintInterface::_outStream);
}
void PrintInterface::flushLog(){
    flush(PrintInterface::_logStream);
}
void PrintInterface::send(std::string str, std::ostream *stream){
    *stream<<str;
}

// print new input line for user
void PrintInterface::printInputPointer(){
    print(PrintInterface::_inputPointer);
    flushOutput();
}
// log new input line for user
void PrintInterface::logInputPointer(){
    log(PrintInterface::_inputPointer);
    flushLog();
}

void PrintInterface::print(std::string str){
    send(str, PrintInterface::_outStream);
}
void PrintInterface::log(std::string str){
    if (PrintInterface::_logging){
        send(str, PrintInterface::_logStream);
    }
}

void PrintInterface::println(std::string str=""){
    print(str);
    print("\n");
    flushOutput();
}
void PrintInterface::logln(std::string str=""){
    if (PrintInterface::_logging){
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
    if (PrintInterface::_logging){
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
    if (PrintInterface::_logging){
        for (auto str:lst){
            log(str);
        }
        flushLog();
    }
}
void PrintInterface::setLog(bool logging){
    PrintInterface::_logging = logging;
}
void PrintInterface::setInputPointer(std::string inputPointer){
    PrintInterface::_inputPointer = inputPointer;
}

void PrintInterface::changeOutput(std::ostream *outStream){
    PrintInterface::_outStream = outStream;
}
void PrintInterface::changeLogOutput(std::ostream *logStream){
    PrintInterface::_logStream = logStream;
}

}