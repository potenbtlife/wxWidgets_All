#include "dnews.h"
#include <stdio.h>
#include <string>
#include "dcommon.h"
#include "CConfig.h"
#include "CRTData.h"

Logger logger;

using namespace std;

int main(int argc, char** argv){
    if(argc <5 ){
        printf("Usage: %s confFile date startPos endPos logFile\n", argv[0]);
        printf("eg: %s ./conf date 0 999 dnews.log\n", argv[0]);
        
        return 0;
    }
	
    string confFile(argv[1]);
    string dateStr(argv[2]);
    int startPos = atoi(argv[3]);
    int endPos = atoi(argv[4]);
    string logfile(argv[5]);

    logger.open(logfile);
    //printf("%s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);
    logger.wirte("%s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);

    CConfig* confObj = new CConfig(confFile);

    CRTData* rtDataObj;
	createRtDataObj(confObj, &rtDataObj);
    rtDataObj->LoadOneDayNews(dateStr, confObj->find("NewsDir"), startPos, endPos);

    return 1;

}


void createRtDataObj(CConfig* cObj, CRTData** destObj) 
{
    string proxyAddr = cObj->find("ProxyAddr");
    string proxyPort = cObj->find("ProxyPort");
    string userName = cObj->find("ProxyUser");
    string passWord = cObj->find("ProxyPwd");

    if(proxyAddr == ""){ //不用代理的情况
        *destObj = new CRTDataNoProxy(80);

    }else{
        char ipaddr[20] = {0};
        Domain2Ip(proxyAddr.c_str(), ipaddr);
        *destObj = new CRTDataProxy(ipaddr, atoi(proxyPort.c_str()), userName.c_str(), passWord.c_str());
    }
}