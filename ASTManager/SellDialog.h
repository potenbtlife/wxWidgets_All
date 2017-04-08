#ifndef SELLDIALOG_H
#define SELLDIALOG_H

#include "common.h"
#include <wx/datectrl.h>
#include "Runtime.h"
#include "CDBSqlite.h"
using namespace std;

extern wxGridStringTable *dataSourceLog;

enum
{
	ID_SELLCTRL_OK = 500, //SellDialog确定按钮标识
	ID_SELLCTRL_CANCEL = 501,
	ID_SELLNANME_CTRL = 502,
	ID_SELLCODE_CTRL = 503,
	ID_SELLPRICE_CTRL = 504,
	ID_SELLNUM_CTRL = 505,
	ID_SELLTIME_CTRL = 506,
	ID_SELLREASON_CTRL = 507,
    ID_SELLYONGJIN_CTRL,
    ID_SELLKONG_CHOICE //是否卖空选择框
};

//定义 SellDialog 类相关的东西
class SellDialog: public wxDialog
{
public:
	//构造函数
	SellDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

	//插入一条卖出记录到数据库
	void insertSellLog(LogDataDet& insertData);

	//更新已有记录的卖出信息
	void updateSellLog(LogDataDet& upLogData);

	//获取一条记录可以成功卖出的数量
	int getSucessSellNum(int toSell, int canSell);

    //检查是否够库存可卖；return: false: 没有足够的库存可卖；true 校验成功 
    bool checkSellCondition(int sellNum, vector<LogDataDet>& vAllCanSellLogData, int& allCanSellNum);

    wxDatePickerCtrl *sellTimeCtrl; //时间控件

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnGetCurPrice(wxCommandEvent& event);//当标的代码框变动时，获取此代码的当前价格
    int updateCash(double cash); //更新临时现金份额表，卖出为正，买入为负

	void SellDialog::UpdateYongJin(wxCommandEvent& event);//更新佣金控件
	
	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *sellNameCtrl;
    wxTextCtrl *sellCodeCtrl;
    wxTextCtrl *sellPriceCtrl;
    wxTextCtrl *sellNumCtrl;
    wxTextCtrl *sellReasonCtrl;
    wxTextCtrl *YongJinCtrl;
    wxChoice *sellkongChoice;
};


#endif