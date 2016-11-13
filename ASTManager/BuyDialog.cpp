#include "common.h"
#include "BuyDialog.h"
#include "main.h"

//sqlite数据库操作对象
#define gSqlite Runtime::getInstance()->sqlite


//注册买入窗口的事件处理函数
BEGIN_EVENT_TABLE(BuyDialog, wxDialog)    
	EVT_BUTTON(ID_BUYCTRL_OK,   BuyDialog::OnOK)
	EVT_BUTTON(ID_BUYCTRL_CANCEL,  BuyDialog::OnCancel)
	EVT_TEXT(ID_BUYCODE_CTRL, OnGetCurPrice)
END_EVENT_TABLE()

//买入窗口构造函数
BuyDialog::BuyDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name)
{
	wxStaticText *buyNameText = new wxStaticText(this, -1, "股票名称:",wxPoint(20, 10),wxSize(60, 20));
	buyNameCtrl = new wxTextCtrl(this,ID_BUYNANME_CTRL, "",wxPoint(90, 10),wxSize(80, 20));
	wxStaticText *buyCodeText = new wxStaticText(this, -1, "代码:",wxPoint(200, 10),wxSize(30, 20));
	buyCodeCtrl = new wxTextCtrl(this, ID_BUYCODE_CTRL, "000",wxPoint(240, 10),wxSize(80, 20));

	wxStaticText *buyPriceText = new wxStaticText(this, -1, "买入价格:",wxPoint(20, 35),wxSize(60, 20));
	buyPriceCtrl = new wxTextCtrl(this,ID_BUYPRICE_CTRL, "0.0",wxPoint(90, 35),wxSize(80, 20));
	wxStaticText *buyNumText = new wxStaticText(this, -1, "数量:",wxPoint(200, 35),wxSize(30, 20));
	buyNumCtrl = new wxTextCtrl(this, ID_BUYNUM_CTRL, "100",wxPoint(240, 35),wxSize(80, 20));

	wxStaticText *buyTimeText = new wxStaticText(this, -1, "买入时间:",wxPoint(20, 60),wxSize(60, 20));
	//wxTextCtrl *buyTimeCtrl = new wxTextCtrl(this, ID_BUYTIME_CTRL, "",wxPoint(90, 60),wxSize(80, 20));
	buyTimeCtrl = new wxDatePickerCtrl(this,ID_BUYTIME_CTRL,wxDefaultDateTime,
		wxPoint(90, 60),wxSize(80, 20), wxDP_DROPDOWN,wxDefaultValidator,"timectrl");

    new wxStaticText(this, -1, "佣金:",wxPoint(200, 60),wxSize(60, 20));
    YongJinCtrl = new wxTextCtrl(this, ID_YONGJIN_CTRL, "",wxPoint(240, 60),wxSize(80, 20));

	wxStaticText *buyReasonText = new wxStaticText(this, -1, "买入原因:",wxPoint(20, 85),wxSize(60, 20));
	buyReasonCtrl = new wxTextCtrl(this, ID_BUYREASON_CTRL, "",wxPoint(90, 85),wxSize(230, 50));
	
	wxButton* okButton = new wxButton(this, ID_BUYCTRL_OK,"确定", wxPoint(120,150), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_BUYCTRL_CANCEL,"取消", wxPoint(210,150), wxSize(60,30));

}

void BuyDialog::OnOK(wxCommandEvent& event)//release 模式下，买入操作获取不到控件的值
{
	string stockName = buyNameCtrl->GetValue();
    stringTrim(stockName);

	string stockCode = buyCodeCtrl->GetValue();
    stringTrim(stockCode);

	string buyPrice = buyPriceCtrl->GetValue();
	string buyNum = buyNumCtrl->GetValue();

    double yongjin=0;
    if( getTradingFee(stockCode, atoi(buyNum.c_str()), atof(buyPrice.c_str()) * atoi(buyNum.c_str()), 0, yongjin) < 0 ){
        wxMessageBox("计算佣金失败，请检查佣金配置！");
    }

    double buy_allAmt = atof(buyPrice.c_str()) * atoi(buyNum.c_str()) + yongjin;

    char cyongjin[16] = {0};
    sprintf(cyongjin, "%.2f", yongjin);
    YongJinCtrl->SetValue(cyongjin);

	string buyTime = buyTimeCtrl->GetValue().Format("%Y-%m-%d");

	string buyReason = buyReasonCtrl->GetValue();
	if( stockName =="" || stockCode=="" || buyPrice=="" || buyNum=="" || buyTime=="" || buyReason=="" )
	{
		wxMessageBox( "所有选项都不能为空，请重新输入！","空值错误！", wxOK | wxICON_ERROR );
		return;
	}

    if(BuyUpdateCash(buy_allAmt) < 0){
        return;
    }

	//gSqlite.open(); //重新open会导致数据库被锁
	gSqlite.setSql(gInsertLogSql);
	gSqlite.prepare();
    gSqlite.bindInt(1, Runtime::getInstance()->CurComposeID);
	gSqlite.bindString(2, stockName.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(3, stockCode.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(4, buyTime.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(5, buyPrice.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(6, buyNum.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(7, buy_allAmt);
	gSqlite.bindString(8, buyReason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(9, "", -1, SQLITE_STATIC);
	gSqlite.bindString(10, "", -1, SQLITE_STATIC);
	gSqlite.bindString(11, "", -1, SQLITE_STATIC);
	gSqlite.bindString(12, "", -1, SQLITE_STATIC);
	gSqlite.bindString(13, "", -1, SQLITE_STATIC);
	gSqlite.bindString(14, "", -1, SQLITE_STATIC);
	gSqlite.bindString(15, "", -1, SQLITE_STATIC);
    gSqlite.bindString(16, "0", -1, SQLITE_STATIC);

	if(gSqlite.step() < 0){
        wxMessageBox(gSqlite.errString);
		return;
	};

	//刷新展现界面中的数据
	//ReLoadLogData(Runtime::getInstance()->vLogDataDet, dataSourceLog,gSqlite);
    gSqlite.finalize();
	
    //设置显示页
	//setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
	//this->GetParent()->Refresh();


    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshLogInfo();
    myFrame->RefreshGoodsAndValue();

	Destroy();
	//Close(false);
}

void BuyDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

//更新临时现金份额表
int BuyDialog::BuyUpdateCash(double changeCash){

    double curCash=0, curShare=0;
    qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, curShare);
    double allCash = curCash - changeCash;
    if (allCash < -0.005) {
        wxMessageBox("现金不够，请先调整现金！");
        return -1;
    }

    string stockName = buyNameCtrl->GetValue();
    string reasonStr = "买入:" + trim(stockName);
    InsertCashRecord(Runtime::getInstance()->CurComposeID, -1*changeCash, allCash, curShare, reasonStr);

    //更新界面显示
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_CASH_TEXTCTRL);
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);

    return 1;
}

//获取标的代码控件的id，根据此id来更新价格控件的价格
void BuyDialog::OnGetCurPrice(wxCommandEvent& event)
{
	string stockId = buyCodeCtrl->GetValue();
	double price = getPriceByStockId(stockId);

	char strPrice[32];
	sprintf(strPrice,"%.3f", price);
	buyPriceCtrl->SetValue(strPrice);
}