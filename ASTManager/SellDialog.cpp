#include "common.h"
#include "SellDialog.h"
#include "main.h"

//sqlite数据库操作对象
#define gSqlite Runtime::getInstance()->sqlite

//注册卖出窗口的事件处理函数
BEGIN_EVENT_TABLE(SellDialog, wxDialog)    
	EVT_BUTTON(ID_SELLCTRL_OK,   SellDialog::OnOK)
	EVT_BUTTON(ID_SELLCTRL_CANCEL,  SellDialog::OnCancel)
	EVT_TEXT(ID_SELLCODE_CTRL, OnGetCurPrice)
END_EVENT_TABLE()

//卖出窗口构造函数
SellDialog::SellDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name)
{
	wxStaticText *sellCodeText = new wxStaticText(this, -1, "代码:",wxPoint(20, 10),wxSize(30, 20));
	sellCodeCtrl = new wxTextCtrl(this, ID_SELLCODE_CTRL, "00",wxPoint(90, 10),wxSize(80, 20));
	wxStaticText *sellNameText = new wxStaticText(this, -1, "股票名称:",wxPoint(180, 10),wxSize(60, 20));
	sellNameCtrl = new wxTextCtrl(this,ID_SELLNANME_CTRL, "",wxPoint(240, 10),wxSize(80, 20));

	wxStaticText *sellPriceText = new wxStaticText(this, -1, "卖出价格:",wxPoint(20, 35),wxSize(60, 20));
	sellPriceCtrl = new wxTextCtrl(this,ID_SELLPRICE_CTRL, "0.0",wxPoint(90, 35),wxSize(80, 20));
	wxStaticText *sellNumText = new wxStaticText(this, -1, "数量:",wxPoint(200, 35),wxSize(30, 20));
	sellNumCtrl = new wxTextCtrl(this, ID_SELLNUM_CTRL, "100",wxPoint(240, 35),wxSize(80, 20));

	wxStaticText *sellTimeText = new wxStaticText(this, -1, "卖出时间:",wxPoint(20, 60),wxSize(60, 20));
	sellTimeCtrl = new wxDatePickerCtrl(this,ID_SELLTIME_CTRL,wxDefaultDateTime,
		wxPoint(90, 60),wxSize(80, 20), wxDP_DROPDOWN,wxDefaultValidator,"timectrl");

    new wxStaticText(this, -1, "佣金:",wxPoint(200, 60),wxSize(60, 20));
    YongJinCtrl = new wxTextCtrl(this, ID_SELLYONGJIN_CTRL, "32.6",wxPoint(240, 60),wxSize(80, 20));

	wxStaticText *sellReasonText = new wxStaticText(this, -1, "卖出原因:",wxPoint(20, 85),wxSize(60, 20));
	sellReasonCtrl = new wxTextCtrl(this, ID_SELLREASON_CTRL, "",wxPoint(90, 85),wxSize(230, 50));
	
    wxStaticText *sellKongText = new wxStaticText(this, -1, "是否卖空:",wxPoint(20, 140),wxSize(60, 20));
    wxString strTest[2] = {"否","是"};
    sellkongChoice = new wxChoice(this, ID_SELLKONG_CHOICE, wxPoint(90, 140),wxSize(80, 20), 2, strTest);
    sellkongChoice->SetSelection(0);

	wxButton* okButton = new wxButton(this, ID_SELLCTRL_OK,"确定", wxPoint(120,170), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_SELLCTRL_CANCEL,"取消", wxPoint(210,170), wxSize(60,30));
	
}

void SellDialog::OnOK(wxCommandEvent& event)
{
	LogDataDet sellLogData;//保存卖出的信息
	sellLogData.stock_name = sellNameCtrl->GetValue();
    stringTrim(sellLogData.stock_name);

	sellLogData.stock_id = sellCodeCtrl->GetValue();
    stringTrim(sellLogData.stock_id);

	sellLogData.sell_price = strtod(sellPriceCtrl->GetValue(),NULL);
	sellLogData.sell_num = atoi(sellNumCtrl->GetValue().c_str());
	sellLogData.sell_time = sellTimeCtrl->GetValue().Format("%Y-%m-%d");

	sellLogData.sell_reason = sellReasonCtrl->GetValue();
	if("" == sellLogData.stock_name || "" == sellLogData.stock_id || (sellLogData.sell_price < 0.05 && sellLogData.sell_price > 0.05)
		|| 0 == sellLogData.sell_num || "" == sellLogData.sell_time || "" == sellLogData.sell_reason)
	{
		wxMessageBox( "所有选项都不能为空，请重新输入！","空值错误！", wxOK | wxICON_ERROR );
		return;
	}

	//gSqlite.open();
	gSqlite.setSql(gFindCanSell); //根据stockname到数据库去查已有的数据，是否够库存卖出

	if(gSqlite.prepare() < 0){
		wxMessageBox(gSqlite.errString);
		return;
	}

	gSqlite.bindString(1, sellLogData.stock_id.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindInt(2, Runtime::getInstance()->CurComposeID);

	vector<LogDataDet> vAllCanSellLogData; //保存所有余量卖出的日志记录
	while( 1== gSqlite.step() ){
		LogDataDet tmpData;
		tmpData.keyid = gSqlite.getColumnInt(0);
		tmpData.buy_num = gSqlite.getColumnInt(1);
		tmpData.sell_num = gSqlite.getColumnInt(2);
		tmpData.sell_price = gSqlite.getColumnDouble(5);
		tmpData.sell_allAmt = gSqlite.getColumnInt(3);
		tmpData.buy_allAmt = gSqlite.getColumnDouble(4);

		vAllCanSellLogData.push_back(tmpData);
	};
	//gSqlite.finalize();

    int allCanSellNum = 0;
	if(!checkSellCondition(sellLogData.sell_num, vAllCanSellLogData, allCanSellNum)) {
        char cAllCanSellNum[16];
        itoa(allCanSellNum,cAllCanSellNum,10);
		wxMessageBox("标的代码["+sellLogData.stock_id+"] 库存["+cAllCanSellNum+"]，没有足够库存可供卖出！");
		return;
	}

    int sucessSellNumAll = 0;
    double yongjin = 0;
    if(sellkongChoice->GetSelection() ==0 ){ //非卖空情况
    
	    vector<LogDataDet>::iterator iter = vAllCanSellLogData.begin();
	    while(sellLogData.sell_num > 0 && iter != vAllCanSellLogData.end()){ //循环扣减余量日志库存
		    LogDataDet upLogData(*iter);
    		
		    int sucessSellNum = getSucessSellNum(sellLogData.sell_num, iter->buy_num - iter->sell_num);
		    upLogData.sell_num = iter->sell_num + sucessSellNum;
            
            upLogData.sell_allAmt = iter->sell_allAmt + sellLogData.sell_price * sucessSellNum;

            if (sellLogData.sell_num == sucessSellNum) { //最后一条更新的记录，加上交易费用
                //计算佣金
                if( getTradingFee(sellLogData.stock_id, sellLogData.sell_num, sellLogData.sell_price * sellLogData.sell_num, 1, yongjin) < 0 ){
                    wxMessageBox("计算佣金失败，请检查佣金配置！");
                }

                upLogData.sell_allAmt = iter->sell_allAmt + sellLogData.sell_price * sucessSellNum - yongjin;
            }

		    upLogData.trade_sumamt = upLogData.sell_allAmt - iter->buy_allAmt;
		    upLogData.sell_price = (upLogData.sell_allAmt + yongjin)/upLogData.sell_num;
		    upLogData.sell_time = sellLogData.sell_time;
		    upLogData.sell_reason = sellLogData.sell_reason;

		    //更新日志记录
		    updateSellLog(upLogData);

		    //更新余下的量
		    sellLogData.sell_num -= sucessSellNum;
		    ++iter;

            sucessSellNumAll += sucessSellNum;
	    }

    }else{ //卖空情况，添加一条卖出记录
        sellLogData.type = "1";
        sucessSellNumAll = sellLogData.sell_num;

        //计算佣金
        if( getTradingFee(sellLogData.stock_id, sellLogData.sell_num, sellLogData.sell_price * sellLogData.sell_num, 1, yongjin) < 0 ){
            wxMessageBox("计算佣金失败，请检查佣金配置！");
        }

        insertSellLog(sellLogData);
    }
	
    char cyongjin[16] = {0};
    sprintf(cyongjin, "%.2f", yongjin);
    YongJinCtrl->SetValue(cyongjin);

    //更新现金值
    double changeCash = sellLogData.sell_price * sucessSellNumAll - yongjin;
    
    updateCash(changeCash); //更新临时现金份额表,卖出为正，买入为负

    //TODO：delete, and check total can sell
	/*if(sellLogData.sell_num > 0){ //全部的日志库存都用光了，还有余量则新增一条记录
		//新增一行卖出记录
		insertSellLog(sellLogData);
	}*/

	//重新加载日志表数据
	//ReLoadLogData(Runtime::getInstance()->vLogDataDet, dataSourceLog,gSqlite);

	//设置显示页
	//setPageFromVector2GridTable(Runtime::getInstance()->vLogDataDet, *dataSourceLog, Runtime::getInstance()->rowNumPage, Runtime::getInstance()->curPage);
    //this->GetParent()->Refresh();

    //Runtime::getInstance()->RefreshCurrentGoods();
    //Runtime::getInstance()->RefreshRecentBuyRecord();
    //Runtime::getInstance()->SetRtCode();//更新实时数据刷新的代码

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshLogInfo();
    myFrame->RefreshGoodsAndValue();

	Destroy();
	//Close(false);
}

void SellDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

//获取标的代码控件的id，根据此id来更新价格控件的价格
void SellDialog::OnGetCurPrice(wxCommandEvent& event)
{
	string stockId = sellCodeCtrl->GetValue();
	double price = getPriceByStockId(stockId);
	string stockName = getNameByStockId(stockId);
	
	char strPrice[32];
	sprintf(strPrice,"%.3f", price);
	sellPriceCtrl->SetValue(strPrice);
	sellNameCtrl->SetValue(stockName.c_str());
}

//插入一条卖出记录到数据库
void SellDialog::insertSellLog(LogDataDet& insertData){

	gSqlite.setSql(gInsertLogSql);
	gSqlite.prepare();
    gSqlite.bindInt(1, Runtime::getInstance()->CurComposeID);
	gSqlite.bindString(2, insertData.stock_name.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(3, insertData.stock_id.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(4, "", -1, SQLITE_STATIC);
	gSqlite.bindString(5, "", -1, SQLITE_STATIC);
	gSqlite.bindString(6, "", -1, SQLITE_STATIC);
	gSqlite.bindString(7, "", -1, SQLITE_STATIC);
	gSqlite.bindString(8, "", -1, SQLITE_STATIC);
	gSqlite.bindString(9, insertData.sell_time.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(10, insertData.sell_price);
	gSqlite.bindInt(11, insertData.sell_num);
	gSqlite.bindDouble(12, insertData.sell_price * insertData.sell_num);
	gSqlite.bindString(13, insertData.sell_reason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindString(14, "", -1, SQLITE_STATIC);
	gSqlite.bindString(15, "", -1, SQLITE_STATIC);
    gSqlite.bindString(16, insertData.type.c_str(), -1, SQLITE_STATIC);

	if(gSqlite.step() < 0 && gSqlite.finalize()){
		wxMessageBox(gSqlite.errString);
		return;
	}

	gSqlite.finalize();
}

//更新已有记录的卖出信息
void SellDialog::updateSellLog(LogDataDet& upLogData){

	gSqlite.setSql(gSellUpdate);
	gSqlite.prepare();
	gSqlite.bindString(1, upLogData.sell_time.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(2, upLogData.sell_price);
	gSqlite.bindInt(3, upLogData.sell_num);
	gSqlite.bindDouble(4, upLogData.sell_allAmt);
	gSqlite.bindString(5, upLogData.sell_reason.c_str(), -1, SQLITE_STATIC);
	gSqlite.bindDouble(6, upLogData.trade_sumamt);
	gSqlite.bindInt(7, upLogData.keyid);

	if(gSqlite.step() < 0 && gSqlite.finalize()){
		wxMessageBox(gSqlite.errString);
		return;
	};
	gSqlite.finalize();
}

//获取一条记录可以成功卖出的数量
int SellDialog::getSucessSellNum(int toSell, int canSell){
	if(toSell <= canSell){//可以完成抵消此次卖出
		return toSell;
	}else{
		return canSell; //此次卖出的量为iter->buy_num - iter->sell_num
	}
}

//检查是否够库存可卖；return: false: 没有足够的库存可卖；true 校验成功 
bool SellDialog::checkSellCondition(int sellNum, vector<LogDataDet>& vAllCanSellLogData, int& allCanSellNum){
    allCanSellNum=0;
    for(int i=0; i<vAllCanSellLogData.size(); ++i) {
        allCanSellNum += (vAllCanSellLogData[i].buy_num - vAllCanSellLogData[i].sell_num);
    }

    if( allCanSellNum < sellNum ) { //没有足够的库存可卖,返回false
        return false;
    } else {
        return true;
    }
}

//更新临时现金份额表,卖出为正，买入为负
int SellDialog::updateCash(double changeCash){

    double curCash=0, debet=0, curShare=0;
    qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, debet, curShare);

    double allCash = curCash + changeCash;
    string stockName = sellNameCtrl->GetValue();
    string reasonStr = "卖出:" + trim(stockName);

    InsertCashRecord(Runtime::getInstance()->CurComposeID, sellStockType, changeCash, allCash, debet, curShare, reasonStr);


    //更新界面显示
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(1008);
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);

    return 1;
}
