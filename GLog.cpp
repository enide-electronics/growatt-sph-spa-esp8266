#include "GLog.h"

Stream *GLOG::s = NULL;

void GLOG::println(unsigned char v) {
    if (GLOG::s) {
        GLOG::s->println(v);
    }
}

void GLOG::print(unsigned char v) {
    if (GLOG::s) {
        GLOG::s->print(v);
    }
}

void GLOG::println(int v) {
    if (GLOG::s) {
        GLOG::s->println(v);
    }
}

void GLOG::print(int v) {
    if (GLOG::s) {
        GLOG::s->print(v);
    }
}

void GLOG::println(const char * msg) {
    if (GLOG::s) {
        GLOG::s->println(msg);
    }
}

void GLOG::print(const char * msg) {
    if (GLOG::s) {
        GLOG::s->print(msg);
    }
}

void GLOG::println(const Printable &o) {
    if (GLOG::s) {
        GLOG::s->println(o);
    }
}

void GLOG::print(const Printable &o) {
    if (GLOG::s) {
        GLOG::s->print(o);
    }
}

void GLOG:: println(const String &o) {
    if (GLOG::s) {
        GLOG::s->println(o);
    }
}

void GLOG::print(const String &o) {
    if (GLOG::s) {
        GLOG::s->print(o);
    }
}
    
void GLOG::setOutput(Stream *stream) {
    GLOG::s = stream;
}

