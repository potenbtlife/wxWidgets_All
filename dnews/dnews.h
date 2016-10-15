/************************************************************************/
/* dnews程序，用于下载新闻
用法：dnews conf date destdir startpos endpos
*/
/************************************************************************/

#ifndef DNEWS_MAIN_H
#define DNEWS_MAIN_H

class CConfig;
class CRTData;
void createRtDataObj(CConfig* cObj, CRTData** destObj);

#endif