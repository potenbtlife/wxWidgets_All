#ifndef DNEWS_DCOMMON_H
#define DNEWS_DCOMMON_H
#include <string>
#include <stdarg.h>

using namespace std;

/*去掉string两边的空格、换行符*/
string& stringTrim(string& src);

inline std::string& rtrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    std::string::size_type pos = str.find_last_not_of(whitespaces);

    if (pos != std::string::npos) {
        str.erase(pos + 1);

    } else {
        str.clear();
    }

    return str;
}

inline std::string& ltrim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return str.erase(0, str.find_first_not_of(whitespaces));
}

inline
std::string& trim(std::string& str, const char* whitespaces = " \t\f\v\n\r") {
    return ltrim(rtrim(str, whitespaces), whitespaces);
}

class Logger{
public:
    Logger();
    ~Logger();

    //打开文件，return: 0:成功返回；>0错误码
    int open(string logfile);

    void wirte(const char *format,...);
    void close();

private:
    string m_logFileName;
    FILE* fileStream;
};

#endif

