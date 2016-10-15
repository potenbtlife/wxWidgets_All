#ifndef INCLUDE_COMMON_H
#define INCLUDE_COMMON_H

#include "wx/wxprec.h"
#include <wx/grid.h>
#ifndef WX_PRECOMP
       #include "wx/wx.h"
#endif
#include "wx/datetime.h"
#include "CDBSqlite.h"
#include <string>
#include <vector>

static wxDateTime startTime;
static wxDateTime endTime;

static std::string gDBName = "./timeMgrData.db"; //数据库路径及名称
//sqlite数据库操作对象
static CDBSqlite gSqlite(gDBName);

static std::string gInsertSql = "insert into things_set(start_time,end_time,do_things,priority) values(?,?,?,?)";

static string gQryHistory = "SELECT do_things, priority from things_set order by keyid desc";

static string gAllQrySql = "SELECT keyid,start_time,end_time,strftime('%s',end_time)-strftime('%s',start_time),do_things,priority,"
	"strftime('%w',start_time),strftime('%W',start_time) from things_set order by start_time desc";

static string gGetObject = "select things from object_plan";//正常情况下，此表只有一条记录
static string gUpdateObject = "update object_plan set things=?";//正常情况下，此表只有一条记录


//包含了时间使用数据的几个维度
class DataContainer{
public:
    int keyid;
	int iWeekofYear; //一年中的第几个星期
	int idayOfWeek; //一个星期中的第几天
	std::string startTime;
	std::string endTime;
	int usedTimeSec; //使用时间，单位为秒
	std::string doTings;
	std::string priority;
};

extern vector<DataContainer> gVecAllData; //保存所有的明细数据
extern const unsigned int lineNumEachPage; //每页的行数
extern unsigned int curPage; //当前页码

//时间转换函数，将秒按照大小转换为合适的单位，并返还转换后的string对象。
inline string timeConvert(int sec){
	char cTmp[32];
	string targetTime;
	memset(cTmp,'0',sizeof(cTmp));
	if( sec < 60){ //时间单位判断，小于60秒
		sprintf(cTmp,"%d",sec);
		targetTime = std::string(cTmp) + "秒";
	}else if( sec < 60 * 60 ){ //小于60分钟
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/60);
		targetTime = std::string(cTmp) + "分钟";
	}else if( sec < 60*60*24){ //小于24小时
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/(60*60));
		targetTime = std::string(cTmp) + "小时";		
	}else{
		sprintf(cTmp,"%.2f",static_cast<double>(sec)/(60*60*24));
		targetTime = std::string(cTmp) + "天";
	}

	return targetTime;
}

//传入数字从0开始，传出星期数
inline string GetDayOfWeek(int index){
	string target;
	switch(index){
		case 0:
			target = "星期天";
			break;
		case 1:
			target = "星期一";
			break;
		case 2:
			target = "星期二";
			break;
		case 3:
			target = "星期三";
			break;
		case 4:
			target = "星期四";
			break;
		case 5:
			target = "星期五";
			break;
		case 6:
			target = "星期六";
			break;
	}
	
	return target;
}

void fillPageFromVector2GridTable(vector<DataContainer>& vSrc, wxGridStringTable& gridStrTable, unsigned int numInOnePage, unsigned int pageNum);

inline int getRowNumInOnePage(vector<DataContainer>& vSrc, int numInOnePage, int pageNum)
{
	int intervalRowNum = (vSrc.size() - numInOnePage * (pageNum - 1));//比较用数据行数计算出来的当页行数 与 前一页的所有行数比较；如果小于0，返回0
	int VecRowNum = (intervalRowNum > 0) ? intervalRowNum : 0;
	
	return VecRowNum < numInOnePage ? VecRowNum : numInOnePage;
}


#endif