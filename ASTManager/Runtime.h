#ifndef ASTMANAGER_RUNTIME_H
#define ASTMANAGER_RUNTIME_H

#include "common.h"
#include "CDBSqlite.h"
#include "tray.h"
#include "CRTData.h"
#include <vector>
#include <map>
#include <string>
#include "Timer.h"
#include "CConfig.h"

using namespace std;


//单例类，保存本程序的一些共享数据
class Runtime
{
public:
    static Runtime* getInstance(){
        if( 0 ==_instance ){
            _instance = new Runtime();
        }
        return _instance;
    }

    CDBSqlite sqlite; //sqlite数据库操作对象
    int curPage; //当前展现页
    int rowNumPage; //一页展现的行数
    vector<LogDataDet> vLogDataDet; //保存日志详细数据的向量，需要时时保证和数据库的数据一致；
    vector<LogDataDet> vSearchResultData; //保存搜索后的结果数据
    //map<string, GoodsDet, greater<std::string> > mapGoodsDet; //保存存量数据
    
    map<int, map<string, GoodsDet> > Compose2CurStock; //现存的组合和stockid、存量信息对应关系；用于每天登记所有组合的净值
    map<string, LogDataDet> mapRecentBuy; //库存量大于0的标的最近的买入记录

    CRTData* rtData;//实时数据对象
    Timer myTimer; //定时器，用于实时刷新实时数据
    MyTray myTray;//托盘图标
    CConfig* configObj;

    int CurComposeID;

    inline int initSqlite(string dbname) {
        sqlite = CDBSqlite(dbname);
        sqlite.open();

        return 1;
    }

    //从日志记录中获取当前存量，保存到mapCurGoods中
    void RefreshCurrentGoods();

    void RefreshRecentBuyRecord();//更新最近买入记录集

    //设置RT对象中的实时刷新的代码
    inline void SetRtCode(){
        vector<std::string> vTmpCode;

        map<int, map<string, GoodsDet> >::iterator iter=Compose2CurStock.begin();
        for(; iter != Compose2CurStock.end(); ++iter){

            vTmpCode.push_back(qryCompseIndex(iter->first));
            for(map<string, GoodsDet>::iterator iter1 = iter->second.begin(); iter1 != iter->second.end(); ++iter1){
                vTmpCode.push_back(iter1->second.stock_id);
            }
        }

        rtData->setAllCode(vTmpCode);
    }

    //从数据库获取当前组合id
    int qryCurCompseID();
    std::string qryCurCompseName();
    std::string qryCompseIndex(int composeId);
    std::string qryCompseChild(int composeId);
    int qryAllCompseID(std::vector<int>& vec);

protected:
    Runtime():curPage(1), rowNumPage(10),CurComposeID(1){};

private:
    static Runtime* _instance;

};

#endif //ASTMANAGER_RUNTIME_H