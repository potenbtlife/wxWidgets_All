#ifndef _CCONFIG_H_
#define _CCONFIG_H_

#include <string>
#include <map>
#include <iostream>

using namespace std;

class CConfig{
public:
    CConfig(const string& configFileName) {
        m_configFileName = configFileName;
        GetDataFromFile();
    }

    void GetDataFromFile();

    //根据键找值
    string find(const string& key){
        map<string, string>::iterator iter = m_mData.find(key);
        if (iter != m_mData.end()) {
            return iter->second;
        }else{
            return "";
        }
    };

    map<string, string> m_mData; //保存键值对
    string m_configFileName;
};

#endif