#ifndef G_LOG_H
#define G_LOG_H

#include <Arduino.h>

class GLOG {
    public:
        static void println(unsigned char v);
        static void print(unsigned char v);
        static void println(int v);
        static void print(int v);
        static void println(const char * msg);
        static void print(const char * msg);
        static void println(const Printable &o);
        static void print(const Printable &o);
        static void println(const String &o);
        static void print(const String &o);
        
        static void setOutput(Stream *s);
        
    private:
        static Stream *s;
};

#endif
