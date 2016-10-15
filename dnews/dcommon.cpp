#include "dcommon.h"
#include <iostream>

/*去掉string两边的空格、换行符*/
string& stringTrim(string& src) {

    int begIndex = 0;

    while (begIndex < src.length()) {
        if (src[begIndex] == ' ' || src[begIndex] == '\n' || src[begIndex] == 10) {
            //cout<<"match!"<<endl;
            ++begIndex;

        } else {
            //cout<<"not match"<<endl;
            break;
        }

    }

    src.erase(0, begIndex);

    int endIndex = src.length() - 1;

    while (endIndex >= 0) {
        if (src[endIndex] == ' ' || src[endIndex] == '\n' || src[endIndex] == 10) {
            //cout<<"match!"<<endl;
            --endIndex;

        } else {
            //cout<<"not match"<<endl;
            break;
        }

    }

    src.erase(endIndex + 1);

    return src;
}

Logger::Logger() {
}

Logger::~Logger() {
    if(fileStream!=NULL){
        fclose(fileStream);
    }
}

void Logger::close(){
    
    if(fileStream!=NULL){
        fclose(fileStream);
    }
}
int Logger::open(std::string logfile) {
    m_logFileName = logfile;

    fileStream = fopen(m_logFileName.c_str(), "w+");

    if (fileStream == NULL) {
        return errno;

    } else {
        return 0;
    }
}

void Logger::wirte(const char* format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(fileStream, format, args);
    va_end(args);
}

