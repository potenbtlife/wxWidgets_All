#include "Timer.h"
#include "Runtime.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/window.h>
#include "main.h"

#include "DownNewsThread.hpp" //线程相关代码实现

void Timer::Notify() {
    //wxMessageBox("in Timer::Notify");

    wxDateTime datetime(wxDateTime::GetTimeNow());
    int hour = datetime.GetHour();
    int minute = datetime.GetMinute();

    bool isRegInDb = false; //是否需要记录数据库

    wxDateTime::WeekDay todayWeekDay = datetime.GetWeekDay(); //Sat, Sun 获取星期

    if (todayWeekDay == 0 || todayWeekDay == 6) { //如果是周末，查询上一工作日是否已登记，未登记的登记，已登记就返回

        wxDateSpan dateSpan(0, 0, 0, 1); //周六减1天

        if (todayWeekDay == 0) {
            dateSpan = wxDateSpan(0, 0, 0, 2); //周日减2天
        }

        wxDateTime lastDate = datetime.Subtract(dateSpan);

        if (0 == QryUnique(string(lastDate.Format("%Y-%m-%d").c_str()))) {//支持如果前一天没有登记，登记前一天数据的功能
            isRegInDb = true;

        } else {
            return;
        }

    } else if ((hour * 60 + minute) < 9 * 60 + 30) { //工作日，并小于9:30，如果前一天已登记直接返回，否则登记

        wxDateSpan dateSpan(0, 0, 0, 1);

        if (todayWeekDay == 1) { //如果是星期一早上，要退三天
            dateSpan = wxDateSpan(0, 0, 0, 3); //周日减3天
        }

        wxDateTime lastDate = datetime.Subtract(dateSpan);


        if (0 == QryUnique(string(lastDate.Format("%Y-%m-%d").c_str()))) {
            isRegInDb = true;

        } else {
            return;
        }

    } else if ((hour * 60 + minute) > 12 * 60 && (hour * 60 + minute) < 13 * 60) { //如果大于12:00时，小于13:00时，直接返回
        return;

    } else if ((hour * 60 + minute) > (17 * 60 + 30)) { //工作日，时间超过17:30，记录数据库

        if (0 == QryUnique(string(datetime.Format("%Y-%m-%d").c_str()))) {
            isRegInDb = true;

        } else {
            return;
        }

    }

    //isRegInDb = true; //for test
	if (!isRegInDb && Runtime::getInstance()->configObj->find("isNotify") != "1") { //如果不记数据，开关关闭，直接返回
		return;
	}

    Runtime::getInstance()->rtData->RefreshRtData();

    std::map<std::string, RTInfo>& rtInfo = Runtime::getInstance()->rtData->getData();
    map<string, LogDataDet>& curRecentGoods = Runtime::getInstance()->mapRecentBuy;

    if (0 == rtInfo.size()) { //获取数据失败，提醒
        Runtime::getInstance()->myTray.ShowBalloon("Faild", "get rt data failed!");
        return;
    }


    if (Runtime::getInstance()->configObj->find("isNotify") == "1") { //如果开关启用，才提醒

        //如果在实时数据中能找到，比较实时价格和买入价*1.1；如果大于就提示
        for (map<string, LogDataDet>::iterator iter = curRecentGoods.begin(); iter != curRecentGoods.end(); ++iter) {

            string type = iter->first.substr(0, 1); //type字段
            string findkey = iter->first.substr(1, iter->first.size() - 1); //去掉前面的type字段
            std::map<std::string, RTInfo>::iterator iter2 = rtInfo.find(trim(findkey));

            if (iter2 == rtInfo.end()) {
                //Runtime::getInstance()->myTray.ShowBalloon("Faild1","find rt info["+findkey+"] failed!");
                break;
            }

            float curPrice = iter2->second.price;

            if (type == "0") {
                std::string OxRemindLev = Runtime::getInstance()->configObj->find("OxRemindLev");
                double oxlev  = 0;
                sscanf(OxRemindLev.c_str(), "%lf", &oxlev);

                if (curPrice > (iter->second.buy_price * oxlev)) {
                    Runtime::getInstance()->myTray.ShowBalloon("SELL", "CAN SELL " + iter->second.stock_id);
                }

            } else { //卖空
                std::string BearRemindLev = Runtime::getInstance()->configObj->find("BearRemindLev");
                double bearlev  = 0;
                sscanf(BearRemindLev.c_str(), "%lf", &bearlev);

                if (curPrice < (iter->second.sell_price * bearlev)) {
                    Runtime::getInstance()->myTray.ShowBalloon("BUY", "CAN BUY " + iter->second.stock_id);
                }
            }

        }
    }



    if (isRegInDb) { //登记数据库，并刷新主窗口的存量和市值信息

        //根据实时数据获取所有的组合id
        std::vector<int> allComposeID;

        Runtime::getInstance()->qryAllCompseID(allComposeID);

        for (std::vector<int>::iterator iter = allComposeID.begin(); iter != allComposeID.end(); ++iter) {
            int composeId = *iter;

            //获取当前市值信息
            double marketValue = 0;
            string detailInfo;
            map<string, GoodsDet>& curGoods = Runtime::getInstance()->Compose2CurStock[composeId];

            for (map<string, GoodsDet>::iterator iter = curGoods.begin(); iter != curGoods.end(); ++iter) {

                string type = iter->first.substr(0, 1); //type字段
                string findkey = iter->first.substr(1, iter->first.size() - 1); //去掉前面的type字段

                std::map<std::string, RTInfo>::iterator iter2 = rtInfo.find(trim(findkey));

                if (iter2 == rtInfo.end()) {
                    Runtime::getInstance()->myTray.ShowBalloon("Faild2", "find rt info failed!");
                    return;
                }

                marketValue += iter->second.num * iter2->second.price;
                char tmpDetail[64] = {0};
                sprintf(tmpDetail, "%s:%d:%.3f;", stringTrim(iter->second.stock_id).c_str(), iter->second.num, iter2->second.price);
                detailInfo += tmpDetail;
            }

            marketValue = (int)(marketValue * 100 + 0.5) / 100; //保留两位小数四舍五入

            //获取参数指数值
            std::string indexCode = Runtime::getInstance()->qryCompseIndex(composeId);;

            double ref_index = 0;
            std::map<std::string, RTInfo>::iterator rtiter = rtInfo.find(trim(indexCode));

            if (rtiter != rtInfo.end()) {
                ref_index = int(rtiter->second.price * 100) / 100.0;
            }

            //获取建议，如果为当前组合，取界面值；否则取上次值
            std::string advice;

            if (composeId == Runtime::getInstance()->CurComposeID) {
                wxTextCtrl* adviceTextCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_RATIOADVICE_TEXTCTRL);
                advice = adviceTextCtrl->GetValue();

            } else {
                string tmpTime, detailInfo;
                double fundShare = 0, fundValue = 0, marketvalue = 0, cash = 0;
                qryValueInfo(composeId, tmpTime, advice, detailInfo, fundShare, fundValue, marketvalue, cash);//获取value_info数据
            }

            RegValueInfo(composeId, string(datetime.Format("%Y-%m-%d").c_str()), marketValue, advice, detailInfo, ref_index);

        }

        MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
        myFrame->RefreshGoodsAndValue();

        //获取新闻
        /*string dateStr = datetime.Format("%Y%m%d");
        string cmd = Runtime::getInstance()->configObj->find("dNewsApp") + " ";
        cmd += Runtime::getInstance()->configObj->m_configFileName + " " + dateStr + " 0 999 dnews.log";
        //string cmd ="cmd.exe";
        if( wxExecute(cmd, wxEXEC_HIDE_CONSOLE) <= 0 ){ //执行出错
            wxMessageBox("can not exec %s", cmd);
            return;
        }*/

        string dateStr = datetime.Format("%Y%m%d");
        string cmd = Runtime::getInstance()->configObj->find("dNewsApp") + " ";
        cmd += Runtime::getInstance()->configObj->m_configFileName + " " + dateStr;
        //string cmd ="cmd.exe";
        if( wxExecute(cmd + " 0 499 dnews1.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //执行出错
            wxMessageBox("can not exec %s", cmd);
            return;
        }

        if( wxExecute(cmd + " 500 999 dnews2.log", wxEXEC_HIDE_CONSOLE) <= 0 ){ //执行出错
            wxMessageBox("can not exec %s", cmd);
            return;
        }

        /*string dir = Runtime::getInstance()->configObj->find("NewsDir") + "\\";

        LoadNewsThread* lnThread1 = new LoadNewsThread(datetime, datetime, dir, 0, 999);

        if (lnThread1->Create() != wxTHREAD_NO_ERROR) {
            wxLogError(wxT("Can't create thread!"));
            return;
        }

        lnThread1->Run();*/

        /*LoadNewsThread* lnThread2 = new LoadNewsThread(datetime, datetime, dir, 500, 999);
        if (lnThread2->Create() != wxTHREAD_NO_ERROR) {
            wxLogError(wxT("Can't create thread!"));
            return;
        }
        lnThread2->Run();*/

    }

};

int Timer::RegValueInfo(int composeID, string recordtime, double marketvalue, string advice, string detailInfo, double ref_index) {

    //查询最新现金及份额，用于计算本次净值
    double fCash = 0, fdebet=0, fund_share = 0;
    qryCashAndShare(composeID, fCash, fdebet, fund_share);


    //父组合特殊处理
    string childList = Runtime::getInstance()->qryCompseChild(composeID);
    if (childList.find("null") == string::npos && childList != "") {
        vector<string> vecChildCom;
        splitString(childList, vecChildCom, ";",false);
        for (vector<string>::iterator iter = vecChildCom.begin(); iter != vecChildCom.end(); ++iter) {
            
            vector<string> vechuilv;
            splitString(*iter, vechuilv, ":",false);

            int qryComposeId = atoi(vechuilv[0].c_str());
            if(qryComposeId == composeID){ //以防配错，配了自己是自己的子组合
                continue;
            }

            double fundShare1=0, fundValue1=0, marketvalue1=0, cash1=0;
            string datetime, value_advice, detailInfo;
            qryValueInfo(qryComposeId, datetime, value_advice, detailInfo, fundShare1, fundValue1,marketvalue1,cash1);//获取value_info数据

            double huilv = 1;
            if( vechuilv.size() > 1 ){ //如果有设汇率，取出（向量中第二个）
                huilv = atof(vechuilv[1].c_str());
            }

            marketvalue = marketvalue + huilv*(marketvalue1 + cash1);
        }
    }

    double all_value = marketvalue + fCash;//总资产
	double net_value = all_value - fdebet;

    if (all_value > -0.05 && all_value < 0.05) {
        return 0;
    }

    double ratio = marketvalue / all_value;
    char stock_ratio[16];
    sprintf(stock_ratio, "%.2f%s", ratio * 100, "%"); //持仓占比

    //计算本次净值
    double fund_value = 0;

    //bool isFirst = !QryAllUnique(composeID);

    if ( fund_share <0.05) { //如果基金份额小于等于0，更新cash_flow表的份额，并将净值设置为1。
        fund_value = 1;
        fund_share = all_value / fund_value;
        string reasonStr = "第一次调整组合份额";
        InsertCashRecord(composeID,AdjustBalance, fCash, fCash, 0, fund_share, reasonStr);

    } else {
        fund_value = (int)((net_value) / fund_share * 10000 + 0.5) / 10000.00; //计算基金净值，保存四位小数，四舍五入
    }

    //插入db
    Runtime::getInstance()->sqlite.setSql(m_insertSql);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindInt(1, composeID);
    Runtime::getInstance()->sqlite.bindString(2, recordtime.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindDouble(3, marketvalue);
    Runtime::getInstance()->sqlite.bindDouble(4, fCash);
    Runtime::getInstance()->sqlite.bindString(5, stock_ratio, -1, SQLITE_STATIC);
	Runtime::getInstance()->sqlite.bindDouble(6, fdebet);
	Runtime::getInstance()->sqlite.bindDouble(7, all_value);
	Runtime::getInstance()->sqlite.bindDouble(8, net_value);
    Runtime::getInstance()->sqlite.bindString(9, stringTrim(advice).c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(10, detailInfo.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindDouble(11, fund_share);
    Runtime::getInstance()->sqlite.bindDouble(12, fund_value);
    Runtime::getInstance()->sqlite.bindDouble(13, ref_index);

    if (Runtime::getInstance()->sqlite.step() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        Runtime::getInstance()->sqlite.finalize();
        return -1;
    }

    Runtime::getInstance()->sqlite.finalize();

    return 1;
}

//检查数据库里是否已经有当天数据
//入参recordtime:时间；1:有重复记录，0：没有重复记录， -1 ：执行错误
int Timer::QryUnique(string recordtime) {
    Runtime::getInstance()->sqlite.setSql(m_qryUnique);

    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindString(1, recordtime.c_str(), -1, SQLITE_STATIC);
    //Runtime::getInstance()->sqlite.bindInt(2, composeID);

    int ret = Runtime::getInstance()->sqlite.step();
    Runtime::getInstance()->sqlite.finalize();

    return ret;
}

//检查数据库里是否是有登记此组合数据
//return：true 有；false 未有
bool Timer::QryAllUnique(int composeID) {
    Runtime::getInstance()->sqlite.setSql(m_qryComposefirst);

    Runtime::getInstance()->sqlite.prepare();
    Runtime::getInstance()->sqlite.bindInt(1, composeID);

    bool ret = false;

    if (1 == Runtime::getInstance()->sqlite.step()) {
        ret = true;
    }

    Runtime::getInstance()->sqlite.finalize();

    return ret;
}

bool Timer::UpdateAllPrice() {

    std::map<std::string, RTInfo> mAllStockInfo;

    int curPos = 0;

    while (1) {
        mAllStockInfo.clear();

        int ret = Runtime::getInstance()->rtData->GetAllStockPrice(mAllStockInfo, curPos);

        if (ret < 0) {
            wxLogError("GetAllStockPrice failed!");
            return false;
        }

        for (std::map<std::string, RTInfo>::iterator iter = mAllStockInfo.begin(); iter != mAllStockInfo.end(); ++iter) {

			if( iter->second.price <0.0001 && iter->second.price > -0.0001 ){ //如果获取到的为0，不更新，防止停牌等原因导致的为0价格
				continue;
			}

            Runtime::getInstance()->sqlite.setSql(updatePrice);

            if (Runtime::getInstance()->sqlite.prepare() < 0) {
                wxMessageBox(Runtime::getInstance()->sqlite.errString);
                return false;
            }

            Runtime::getInstance()->sqlite.bindDouble(1, iter->second.price);
            Runtime::getInstance()->sqlite.bindString(2, iter->first.c_str(), -1, SQLITE_STATIC);
            Runtime::getInstance()->sqlite.bindInt(3, 0); //hk先写死为0

            if (Runtime::getInstance()->sqlite.step() < 0) {
                wxMessageBox(Runtime::getInstance()->sqlite.errString);
                continue;
            }
        }

        if (ret == 0) {
            break;
        }
    }

    Runtime::getInstance()->sqlite.finalize();

    return true;
}