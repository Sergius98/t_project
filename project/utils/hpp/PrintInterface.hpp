#ifndef PRINTINTERFACE_HPP
#define PRINTINTERFACE_HPP

#include <string>
#include <iostream>
#include <list>

namespace printInterface{

class PrintInterface{
    public:
        static void printInputPointer();
        static void logInputPointer();
        static void flushOutput();
        static void flushLog();
        static void println(std::string str);
        static void println(std::list<std::string> lst);
        static void logln(std::string str);
        static void logln(std::list<std::string> lst);
        static void print(std::string str);
        static void print(std::list<std::string> lst);
        static void log(std::string str);
        static void log(std::list<std::string> lst);
        static void setLog(bool logging);
        static void setInputPointer(std::string inputPointer);
        static void changeOutput(std::ostream *outStream);
        static void changeLogOutput(std::ostream *logStream);
    private:
        static bool _logging;
        static std::string _inputPointer;
        static std::ostream *_outStream;
        static std::ostream *_logStream;
        static void flush(std::ostream *stream);
        static void send(std::string str, std::ostream *stream);
};

using PrInt = PrintInterface;
}
#endif