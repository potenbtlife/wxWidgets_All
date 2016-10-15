#include "CConfig.h"
#include <fstream>
#include "dcommon.h"

void CConfig::GetDataFromFile() {
    if("" == m_configFileName){
        cout<<"error: m_configFileName is null!"<<endl;
        return;
    }

    ifstream inFile(m_configFileName.c_str());
    if(!inFile) {
        cerr<<"error: unable to open file["<<m_configFileName<<"]!"<<endl;
        return;
    }
    
    string line;
    while(inFile >> line) {
        
        if (line.empty()) {
            continue;
        }

        if( line.compare(0,1,"#") == 0 ) {
            continue;
        }
        
        string::size_type pos = line.find_first_of("=");
        string key(line, 0, pos);
        string value(line, pos + 1);
        
        m_mData.insert(make_pair(stringTrim(key), stringTrim(value)));
    }

    inFile.close();
    //cout<<"end!"<<endl;
}