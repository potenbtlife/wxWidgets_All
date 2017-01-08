// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#ifndef ASTMANAGER_MAIN_H
#define ASTMANAGER_MAIN_H

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/datectrl.h>
#include <wx/snglinst.h>
#include <wx/file.h>
#include <wx/log.h>

#ifndef WX_PRECOMP    
	#include <wx/wx.h>
#endif

#include "CDBSqlite.h"
#include "common.h"
#include "Runtime.h"
#include "BuyDialog.h"
#include "SellDialog.h"
#include "AdjustCashDialog.h"
#include <vector>
#include <set>
#include "CConfig.h"
#include "DownNewsThread.hpp"


//配置文件，键值对的形式，=为分隔符
const string configName="conf";
const string key="DBPATH"; //数据库路径的键

//sqlite数据库操作对象
#define gSqlite Runtime::getInstance()->sqlite

void DeleteFinanceIndexFromDb(string reportType);
void DeleteOneFinanceIndexFromDb(string reportType, string stockId);

class MyApp: public wxApp
{
public:
        virtual bool OnInit();

private:
    wxSingleInstanceChecker* m_checker;

};

class MyFrame: public wxFrame
{
	public:
		MyFrame(wxWindowID frameID, const wxString& title, const wxPoint& pos, const wxSize& size);
        ~MyFrame();
		
	//private:
        void initDataSourceLog();//初始化日志展示列表
        void SetCurGoodsPage(wxGridStringTable *goodsDS, double all_value); //设置存货界面, all_value:总市值

        void SetPrice2CurGoods( string detailInfo );
        void OnBuy(wxCommandEvent& event);
		void OnSell(wxCommandEvent& event);
        void OnAdjustCash(wxCommandEvent& event);
        void OnBuyFound(wxCommandEvent& event);
        void OnSellFound(wxCommandEvent& event);
		void AddDebet(wxCommandEvent& event);
		void SubDebet(wxCommandEvent& event);
		void FinanceFee2Buy(wxCommandEvent& event);
		void OnOperHistory(wxCommandEvent& event);
		//void OnExit(wxCommandEvent& event);

        void UpdateAdviceInfoInDb();
        void OnClose(wxCloseEvent& event); //右上角的关闭按钮，隐藏不退出
		void OnAbout(wxCommandEvent& event);
		void OnReviewLog(wxCommandEvent& event);
		void OnDeleteLog(wxCommandEvent& event);
		void OnPresLog(wxCommandEvent& event);
		void OnNextLog(wxCommandEvent& event);
		void OnGetReviewData(wxGridEvent& event);
		void OnCommitReview(wxCommandEvent& event);
        void OnSearchName(wxCommandEvent& event);
		void OnShowOneFinance(wxGridEvent& event);//显示某个标的的财务指标
        void OnHKCompose(wxCommandEvent& event);
        void OnShowValueReport(wxCommandEvent& event);
        void DownLoadYearThreeTbl(wxCommandEvent& event);
        void DownLoadMidThreeTbl(wxCommandEvent& event);
        void DownLoadOneThreeTbl(wxCommandEvent& event);//下载某一标的报表
        void DownLoadNews(wxCommandEvent& event);

        void OnShowThreeTable(wxCommandEvent& event);
        void OnReCalcFinanceIndex(wxCommandEvent& event);
        void UpdateAllPrice(wxCommandEvent& event);
        void OnFindAShot(wxCommandEvent& event);

        void RefreshLogInfo(); //刷新日志展现数据
        void RefreshGoodsAndValue();
        void OnACompose(wxCommandEvent& event);
        void OnFixedCompose(wxCommandEvent& event);
        void OnAllCompose(wxCommandEvent& event);
        //void OnShowBuyCondition(wxGridEvent& event);

        void LogGridAutoSize(); //自动调整日志列表grid的行和列的宽度和高度
        void RefreshDataFromDB();

        //填充市值界面, 并输出参数
        void GetCashDebetFromDb(int composeId, double& marketvalue,double& cash, double& debet,string& datetime,double& fundShare,double& fundValue);

        void SetValueText(double marketvalue, double cash, double debet, string datetime, double fundShare, double fundValue );
        void RefreshAdvice();
        void WriteBalanceToDb(string stock_id, int reportType, vector<BalanceData>& vecOneAllBalance); //向数据库登记资产负债表数据
        void WriteSunYiToDb(string stock_id, int reportType, vector<SunYiData>& vecOneAllSunYi); //向数据库登记损益表数据
        void WriteCashFlowToDb(string stock_id, int reportType, vector<CashFlowData>& vecOneAllCashFlow); //向数据库登记现金流量表数据
        void CalcFinanceIndexToDb(string stock_id, vector<BalanceData>& vecOneAllBalance, vector<SunYiData>& vecOneAllSunYi, vector<CashFlowData> vecOneAllCashFlow);

        void WriteThreeRptToDb(int reportType, string stock_id = ""); //报表入库总接口;param[in] reportType : 0: 年报； 1：中报; stock_id: 标的id，如果没有指定，取所有

		DECLARE_EVENT_TABLE();
	private:

		vector<LogDataDet> vCiReview;//保存提交总结的数据
        wxDialog *buyDialog; //买入对话框指针
        wxDialog *sellDialog; //卖出对话框指针
        wxDialog *adjustCashDialog; //调整现金对话框指针
        wxComboBox* stockName_comboBox; //名称组合框控件

        wxArrayString filterArrayString;//保存stock_name，用来搜索
        bool m_inSearch; //是否在搜索状态

        wxGrid *gridLog; //展现日志列表grid对象
        wxGrid *gridGoods; //存货列表对象

        wxTextCtrl* marketValTextCtrl;
        wxTextCtrl* cashTextCtrl;
		wxTextCtrl* debetTextCtrl;
        wxTextCtrl* stockRatioTextCtrl;
        wxTextCtrl* totalAssetTextCtrl;
		wxTextCtrl* netAssetTextCtrl;
        wxTextCtrl* netValueTextCtrl;
        wxTextCtrl* shareTextCtrl;
		wxTextCtrl* leverageTextCtrl;
        wxTextCtrl* adviceTextCtrl;
        wxTextCtrl* timeTextCtrl;
        wxMenu *composeMenu; //菜单栏组合控件
        LoadNewsThread* lnThread1;

        int updateIsCurrentCompose(int updateValue, int composeid);
        int ResetAllCurrentCompose(); //重置所有的iscurrent 字段为0

};


#endif //ASTMANAGER_MAIN_H