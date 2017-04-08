#ifndef COMMON_H
#define COMMON_H

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/datectrl.h>

#ifndef WX_PRECOMP    
	#include <wx/wx.h>
#endif
#include "CDBSqlite.h"
//#include "CRTData.h"
#include <vector>
#include <map>
#include <sstream>

static string gQryLogSql="select compose_id,stock_id,stock_name,buy_time,buy_price,buy_num,buy_allAmt,buy_reason,\
                         sell_time,sell_price,sell_num,sell_allAmt,sell_reason,trade_sumamt,trade_review,keyid,type \
                         , (buy_time<ifnull(sell_time,'1970-01-01')),case (buy_time<ifnull(sell_time,'1970-01-01')) when 1 then sell_time else buy_time end as sortid \
				 from trade_info order by compose_id,sortid desc";
static string gInsertLogSql="insert into trade_info(compose_id,stock_name,stock_id,buy_time,buy_price,buy_num,buy_allAmt,buy_reason,\
				 sell_time,sell_price,sell_num,sell_allAmt,sell_reason,trade_sumamt,trade_review,type)\
				  values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
static string gFindCanSell="select keyid,buy_num,sell_num,sell_allAmt,buy_allAmt,sell_price,sell_time,\
				 sell_reason,trade_sumamt,sell_price from trade_info where ((buy_num-sell_num)>0 or sell_num is null) \
                 and stock_id=? and compose_id=? order by buy_time desc";

static string gSellUpdate="update trade_info set sell_time=?,sell_price=?,sell_num=?,sell_allAmt=?, \
                          sell_reason=?,trade_sumamt=? where rowid=?";

static string gDeleteLog="delete from trade_info where keyid=?";
static string gUpdateReview="update trade_info set trade_review=?  where rowid=?";


//查询最近的份额
const std::string qryCashSql = "select after_cash, debet, fund_share from cash_flow a where a.compose_id=? and a.change_time=(select max(change_time) from cash_flow where compose_id=a.compose_id)";
const std::string insertCashSql = "insert into cash_flow(compose_id,oper_type,change_cash,after_cash,debet,fund_share,change_time,change_reason) values(?,?,?,?,?,?,datetime('now'),?)";

//sqlite数据库操作对象
//extern CDBSqlite gSqlite;

//操作类型，1买入；2卖出；3申购（包含股息收入）；4赎回；5直接调整现金
enum OPER_TYPE{
	buyStockType =1,
	sellStockType=2,
	buyFundType=3,
	sellFundType=4,
	AdjustBalance=5, //手工调整
	addDebet,
	subDebet
};

typedef enum {
	ADD =1,
	SUB=2
} DEBET_OPER_TYPE;

inline string GetNameOfOperType(int type){

	string name="未知类型";
	switch(type){
		case buyStockType:
			name="买入";
			break;
		case sellStockType:
			name="卖出";
			break;
		case buyFundType:
			name="申购";
			break;
		case sellFundType:
			name="赎回";
			break;
		case AdjustBalance:
			name="调整余额";
			break;
		case addDebet:
			name="加负债";
			break;
		case subDebet:
			name="减负债";
			break;
		default:
			break;
	}

	return name;
}

enum MainID
{
    ID_MAIN_FRAME=999,
    ID_COMMON_REVIEW_BUTTON=1000,
    ID_DELETE_BUTTON=1001,
    ID_PREVIOUS_BUTTON=1002,
    ID_NEXTPAGE_BUTTON=1003,
    ID_SEARCH_LOG_BUTTON=1004,
    ID_STOCKNAME_COMBOBOX=1005,
    ID_VALUESHOW_PANEL=1006,
    ID_MARKETVAL_TEXTCTRL=1007,
    ID_CASH_TEXTCTRL=1008,
	ID_DEBET_TEXTCTRL,
    ID_STOCKRATIO_TEXTCTRL,
    ID_TOTALASSET_TEXTCTRL,
	ID_NETASSET_TEXTCTRL,
    ID_RATIOADVICE_TEXTCTRL,
    ID_NETVALUE_TEXTCTRL,
    ID_SHARE_TEXTCTRL,
	ID_REVERAGE_TEXTCTRL,
    ID_HKCOM,
    ID_ACOM,
    ID_FIXEDCOM,
    ID_ALLCOM,
    ID_BUY ,
    ID_SELL,
    ID_ADJUSTCASH,
    ID_BUG_FOUND,
    ID_SELL_FOUND,
	ID_ADD_DEBET_FOUND,
	ID_SUB_DEBET_FOUND,
	ID_OPER_HISTORY,
	ID_SUB_FINANCE_FEE_BUY,
    ID_REPORT_MENU,
    ID_DOWNLOAD_YEAR_THREETBL,
    ID_DOWNLOAD_MID_THREETBL,
    ID_DOWNLOAD_ONE_THREETBL,
    ID_UPDATE_ALLPRICE,
    ID_SHOW_THREE_TABLE,
    ID_FIND_ASHOT,
    ID_CALCINDEX_TABLE,
    ID_LOG_GRID,
    ID_GOODS_GRID,
    ID_LOAD_NEWS
};

struct LogDataDet
{
	int keyid;
    int composeid;
	string stock_name;
	string stock_id;
	string buy_time;
	double buy_price; 
	int buy_num;
	double buy_allAmt;
	string buy_reason;
	string sell_time; 
	double sell_price;
	int sell_num;
	double sell_allAmt;
	string sell_reason;
	double trade_sumamt;
	string trade_review;
    string type;//交易类型，1为卖空；0为非卖空
};

struct GoodsDet{
    string stock_name;
    string stock_id;
    int num;
    double price; 
    double amt;
    bool isBear; //是否卖空；true是；false否

    friend bool operator <(const GoodsDet& la, const GoodsDet& rb);
};

/*刷新第N页的数据
*@vSrc：数据源向量；ds：grid表格的数据源；numInOnePage：一页中的行数；pageNum：填充到ds的页数
*/
void setPageFromVector2GridTable(vector<LogDataDet>& vSrc, wxGridStringTable& gridStrTab, int numInOnePage, int pageNum);

inline int getRowNumInOnePage(int allNum, int numInOnePage, int pageNum)
{
	/*int test5 = (numInOnePage * (pageNum - 1));
	int test6 = vSrc.size();
	bool test7 = ((test6 - test5) > 0);
	int test8 = vSrc.size() - test5;
	bool test2 = ((vSrc.size() - test5) > 0); //当vSrc.size()为6， test5为10 时，此处为什么返回了true？怪异
	bool test4 = test1 > 0;
	int test3 = (vSrc.size() - numInOnePage * (pageNum - 1));*/

	int intervalRowNum = (allNum - numInOnePage * (pageNum - 1));//比较用数据行数计算出来的当页行数 与 前一页的所有行数比较；如果小于0，返回0
	int VecRowNum = (intervalRowNum > 0) ? intervalRowNum : 0;
	
	return VecRowNum < numInOnePage ? VecRowNum : numInOnePage;
}

/*去掉string两边的空格、换行符*/
string& stringTrim(string& src);

//字符串转换为时间格式
static time_t StringToDatetime(const char *str)  
{  
    tm tm_;  
    int year, month, day, hour=0, minute=0,second=0;  
    sscanf(str,"%d-%d-%d", &year, &month, &day);
    tm_.tm_year  = year-1900;  
    tm_.tm_mon   = month-1;  
    tm_.tm_mday  = day;  
    tm_.tm_hour  = hour;  
    tm_.tm_min   = minute;  
    tm_.tm_sec   = second;  
    tm_.tm_isdst = 0;

    time_t t_ = mktime(&tm_); //已经减了8个时区  
    return t_; //秒时间  
}  

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


inline int splitString(const std::string& str, std::vector<std::string >& vec, const std::string spChs, bool onlyFirst, bool multiChar = false) {
    vec.clear();
    std::string workStr = str;
    std::string tmpStr;
    
    if (trim(workStr, spChs.c_str()).empty()) {
        return 0;
    }

    int curPos = 0, bakPos = 0;

    bool isfirst = true;
    do {

        if (multiChar) { //查找多字符的分隔符
            if(!isfirst) {
                bakPos = curPos + spChs.size();
            }
            isfirst = false;
            curPos = workStr.find(spChs.c_str(), bakPos);
            
        }else{
            bakPos = workStr.find_first_not_of(spChs.c_str(), curPos);
            curPos = workStr.find_first_of(spChs.c_str(), bakPos);
        }

        if (curPos != std::string::npos) {  //lint !e650 !e737  l00294687 pclint 2015.6.4 
            vec.push_back(workStr.substr(bakPos, curPos - bakPos));

            if (onlyFirst) {
                tmpStr = workStr.substr(curPos + 1, workStr.length() - curPos - 1); //lint !e737 by wWX238163
                trim(tmpStr);
                vec.push_back(tmpStr);
                break;
            }

        } else {
            tmpStr = workStr.substr(bakPos, workStr.length() - bakPos); //lint !e737 by wWX238163
            trim(tmpStr);
            vec.push_back(tmpStr);
            break;
        }

        //curPos += 1;
    } while (true);

    return 1;
}

//输入一个日期（输入的month从0开始），返回这个月的最后一天
//返回：输入月的最后一天；如果是-1，出错；
inline int GetLastDate(int month,int year)
{
    if(year<=1) {
        return -1;
    }
    if(month<1||month>12){
        return -1;
    }

    int months[]={31,0,31,30,31,30,31,31,30,31,30,31};
    if(year % 4 ==0 && year % 100 !=0 || year % 400 ==0)
        months[1]=29;
    else months[2]=28;

    return months[month];
}

/*计算交易手续费 
* param[in] stockid 标的id；tradeNum 交易股数； tradeAmt 计算金额; type 交易类型：0 买；1 卖
* param[out] tax, 出参，总佣金
* return: <0, 计算佣金失败
*/
int getTradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax);

//计算港股的交易手续费；tradeAmt,总成交额；return 交易手续费
int getHKTradingFee(double tradeAmt, double& tax);

/*计算a股手续费 
* param[in] stockid 标的id；tradeNum 交易股数； tradeAmt 计算金额; type 交易类型：0 买；1 卖
* param[out] tax, 出参，总佣金
* return: <0, 计算佣金失败
*/
int getATradingFee(std::string stockid, int tradeNum, double tradeAmt, int type, double& tax);

/*根据短的stockid，生成用于url请求的code
* return url请求的code
*/
std::string genUrlRequestCode(std::string shortCode);

//genUrlRequestCode 函数的反函数
std::string reverseUrlCode(std::string urlCode);

//检查数据库里是否已经有数据,入参recordtime:时间；return：true 已有数据；false 未有数据
bool QryUniqueRptData(string querySql, string stock_id, string recordtime);

//获取所有的标的列表
int getAllStockIdName(vector<string>& vecStockIdName);

int getAllStockId(vector<string>& vecStockId); //仅获取所有stockid

double getPriceByStockId(string& stockId);//根据stock_id 获取最新价格
string getNameByStockId(string& stockId); //根据stock_id 获取名称

void qryCashAndShare(int composeId, double& cashVaule, double& debet, double& curShare); //查询最新的现金和分额

//operType，1买入；2卖出；3申购（包含股息收入）；4赎回；5直接调整现金
void InsertCashRecord(int composeId, int operType, double changeCash, double afterCash, double debet, double share, string &reasonStr );//插入记录

void qryValueInfo(int composeId, string& datetime,string& value_advice,string& detailInfo,double& fundShare,double& fundValue, double& marketvalue,double& cash);

class CRTData;
void createRtDataObj(CRTData** destObj);//创建 CRTData 对象

double getMarketValue(int composeId);//获取指定组合的市值

#endif