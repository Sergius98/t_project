#ifndef PRINTINTERFACE_HPP
#define PRINTINTERFACE_HPP

#include <string>
#include <iostream>
#include <list>


class PrintInterface{
    public:
        static PrintInterface getDefault();
        
        void printInputPointer();
        void logInputPointer();
        void flushOutput();
        void flushLog();
        void println(std::string str);
        void println(std::list<std::string> lst);
        void logln(std::string str);
        void logln(std::list<std::string> lst);
        void print(std::string str);
        void print(std::list<std::string> lst);
        void log(std::string str);
        void log(std::list<std::string> lst);
        void setLog(bool logging);
        void setInputPointer(std::string inputPointer);
        void changeOutput(std::ostream *outStream);
        void changeLogOutput(std::ostream *logStream);
    private:
        bool _logging=true;
        std::string _inputPointer = ">";
        std::ostream *_outStream = &std::cout;
        std::ostream *_logStream = &std::cout;
        void flush(std::ostream *stream);
        void send(std::string str, std::ostream *stream);
};
#endif