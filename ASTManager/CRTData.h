#ifndef ASTMANAGER_CRTDATA_H
#define ASTMANAGER_CRTDATA_H

#include <winsock.h>
#include "common.h"
#include <string>
#include <vector>
#include <map>
//#include <WinSock2.h>
//#include <ws2def.h>
#include "base64_algo.h"
#include <assert.h>
#include <math.h>

using namespace std;

//根据域名来获取ip地址
int Domain2Ip(const char * url, char* ipaddr);
int getHttpHeadLength(const char* src);
//获取http协议的数据长度字段,-1:can not find；如果是chunked类型的数据，返回1
int getHttpDataLength(const char* src, int srcLen);
//获取第一个chunk块的长度；src：解析的数据块，必须已经去掉http头数据
int getHttpChunkLength(const char* dataSrc);
int hex_char_value(char c);
int hex_to_decimal(const char* szHex, int len);
int checkHttpHead(const char* src, int& code);

////格式：MIKO INTL,米格国际控股,0.640,0.630,0.640,0.610,0.630,0.000,0.000,0.610,0.630,247620,402000,3.520,5.079,1.280,0.600,2015/09/25,16:01
//PARKSON GROUP,百盛集团,1.060,1.030,1.080,1.020,1.060,0.030,2.913,1.060,1.070,6042972,5714165,0.000,4.717,2.380,0.990,2015/09/25,16:01
struct RTInfo{
    std::string name;
    float price; //现价
    //time_t datetime; //价格时间
};

/*
资产负债表：
报告期
报表类型
非流动资产
流动资产
流动负债
净流动资产/(负债)
非流动负债
少数股东权益 - (借)/贷
净资产/(负债)
已发行股本
储备
股东权益/(亏损)
无形资产(非流动资产)
物业、厂房及设备(非流动资产)
附属公司权益(非流动资产)
联营公司权益 (非流动资产)
其他投资(非流动资产)
应收账款(流动资产)
存货(流动资产)
现金及银行结存(流动资产)
应付帐款(流动负债)
银行贷款(流动负债)
非流动银行贷款
总资产
总负债
股份数目
币种
*/
struct BalanceData{
    string stock_id;
    string report_time;
    string report_type;
    double static_asset;
    double flow_asset;
    double flow_debt;
    double netflow_asset;
    double static_debt;
    double fewholder_asset;
    double net_asset;
    double guben;
    double chubei;
    double holder_asset; //单位为百万
    double invisible_asset;
    double building_device;
    double childcort_asset;
    double sibing_asset;
    double other_invest;
    double yingshou;
    double cunhuo;
    double cash;
    double yingfu;
    double flow_bankdept;
    double static_bankdept;
    double all_asset;
    double all_dept;
    double stock_share;
    string bizhong;
    double bond; //发行的债券
};

/*
综合损益表：
报告期
报表类型
营业额
除税前盈利/(亏损)
税项
除税后盈利/(亏损)
少数股东权益
股东应占盈利/(亏损)
股息
除税及股息后盈利/(亏损)
基本每股盈利(仙)
摊薄每股盈利(仙)
每股股息(仙)
销售成本
折旧
销售及分销费用
一般及行政费用
利息费用/融资成本
毛利
经营盈利
应占联营公司盈利
币种
*/
struct SunYiData{
    string stock_id;
    string report_time;
    string report_type;
    double yingyee;
    double untax_profit;
    double tax;
    double tax_profit;
    double fewholder_profit;
    double holder_profit;
    double dividend;
    double net_profit;
    double basic_one_profit;
    double tanxiao_one_profit;
    double ont_dividend;
    double cost;
    double depreciation;
    double sell_fee;
    double admin_fee;
    double lixi_fee;
    double gross_profit;
    double jingying_profit;
    double othercorp_profit;
    string bizhong;
};

/*
现金流量表：
报告期
报表类型
经营业务所得之现金流入净额
投资活动之现金流入净额
融资活动之现金流入净额
现金及现金等价物增加
会计年初之现金及现金等价物
会计年终之现金及现金等价物
外汇兑换率变动之影响
购置固定资产款项
币种
*/
struct CashFlowData{
    string stock_id;
    string report_time;
    string report_type;
    double jingying_net;
    double invest_net;
    double rongzi_net;
    double cash_add;
    double cash_begin;
    double cash_end;
    double waihui_profit;
    double buy_device;
    string bizhong;
};

//实时数据超基类
class CRTData {
public:
   virtual int RefreshRtData(); //获取持有股票价格

   //获取所有stock价格；return 1:正常取出，还有； 0: 正常取完；-1：异常错误
   int GetAllStockPrice(std::map<std::string, RTInfo>& mapOut, int& curpos);

   int GetRtDataFromHttp(string& allCode, std::map<std::string, RTInfo>& mOutData);

   virtual int GetHttpData(std::string url, std::string& allGetData, FILE* flstm = NULL) = 0;
   virtual int getOneReportData(string url, string& allGetData){return 0;}
   
   //get balance report, param[in] reportType : 0: 年报； 1：中报
   int GetBalanceReport(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance);
   //get sun yi report, param[in] reportType : 0: 年报； 1：中报
   int GetSunYiReport(string stock_id, int reportType,vector<SunYiData>& vecOneAllSunYi);
   //get cash flow report, param[in] reportType : 0: 年报； 1：中报
   int GetCashFlowReport(string stock_id, int reportType,vector<CashFlowData>& vecOneAllCashFlow);

   virtual int ConnectSepSocket(string& hostName) = 0;//子类实现连接socket函数

   //never use, use dnews process
   void LoadOneDayNews( wxDateTime &datetime, string outDir, int beginNum=0, int endNum=999);//下载一天指定范围的新闻

   std::map<std::string, RTInfo>& getData(){
       return m_mOutData;
   }

   void setAllCode(std::vector<std::string>& data){
       m_vCode = data;
   }

   void addOneCode(std::string stockCode){
       if(stockCode != ""){
            m_vCode.push_back(stockCode);
       }
   }

   virtual int resetSocket(string& hostName){
       closesocket(m_Socket);
       WSACleanup();
       return ConnectSepSocket(hostName);
   }

protected:

    int ConnectSocket(std::string proxyIP, int proxyPort); //建立socket连接函数

    //获取socket套接口中所有接收缓冲区的数据，返回获取到的字节数
    int Recv(int sockfd, string& allGetData, int& code, FILE* flstm = NULL);

    SOCKET m_Socket; //需要连接到服务器的sock连接，有代理的是代理服务器，没代理的是host的服务器
    std::vector<std::string> m_vCode;

    std::map<std::string, RTInfo> m_mOutData;
};

//实时对象有代理
class CRTDataProxy : public CRTData{
public:
    CRTDataProxy(char* proxyIP, int proxyPort,const char* username, const char* pwd);

    virtual int GetHttpData(std::string url, std::string& allGetData, FILE* flstm = NULL);//请求http消息

    int ConnectSepSocket(string& hostName){ //此时的hostName没用，完全为了兼用父类接口
        return ConnectSocket(m_proxyIP, m_proxyPort);
    }
protected:

    std::string m_proxyIP;//代理服务器的ip地址
    int m_proxyPort; //代理服务器的端口号
    std::string m_proxyUserName; //代理服务器的用户名
    std::string m_proxyPassword; //代理服务器的密码
};

//实时对象 不使用代理服务器版本
class CRTDataNoProxy : public CRTData {
public:

    CRTDataNoProxy(int port);
    virtual int GetHttpData(std::string stockCode, std::string& allGetData, FILE* flstm = NULL);

    int ConnectSepSocket(string& hostName);

    string m_hostIp;
    int m_hostPort;
};

#endif //ASTMANAGER_CRTDATA_H