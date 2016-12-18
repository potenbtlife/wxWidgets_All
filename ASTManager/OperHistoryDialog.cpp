#include "common.h"
#include "OperHistoryDialog.h"
#include "main.h"

const string QRY_CASH_FLOW="select keyid,compose_id, oper_type,change_cash,after_cash,fund_share,change_time,change_reason from cash_flow where compose_id=? order by keyid desc";
const string DEL_CASH_FLOW="delete from cash_flow where keyid=?";
const string DEL_TRADE_INFO="delete from trade_info where keyid in (select max(keyid) from trade_info)";

//注册买入窗口的事件处理函数
BEGIN_EVENT_TABLE(OperHistoryDialog, wxFrame)
	EVT_BUTTON(ID_OPER_HISTORY_NEXT,  OnNext)
	EVT_BUTTON(ID_OPER_HISTORY_PREVIOUS,  OnPrevious)
	EVT_BUTTON(ID_OPER_HISTORY_ROLLBACK,  OnRollback)
	EVT_BUTTON(ID_OPER_HISTORY_CANCEL,  OnCancel)
END_EVENT_TABLE()

//操作历史窗口构造函数
OperHistoryDialog::OperHistoryDialog(wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxFrame(parent,id,title,pos,size,style,name),curPage(1),numOnePage(8)
{

	//添加grid控件，展示操作日志
	_pOperHisgrid = new wxGrid(this, ID_OPER_HISTORY_GRID, wxPoint(0, 0), size);
	
	_pDS = new wxGridStringTable(1, 8);
	_pDS->SetColLabelValue(0, "操作时间");
	_pDS->SetColLabelValue(1, "组合名称");
	_pDS->SetColLabelValue(2, "操作类型");
	_pDS->SetColLabelValue(3, "变前余额");
	_pDS->SetColLabelValue(4, "变动金额");
	_pDS->SetColLabelValue(5, "变后金额");
	_pDS->SetColLabelValue(6, "基金份额");
	_pDS->SetColLabelValue(7, "变动原因");

	GetCashFlowData(_vecCashFlowData);

	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->SetTable(_pDS, true);
	_pOperHisgrid->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();
	_pOperHisgrid->SetRowLabelSize(30);

	wxButton* nextButton = new wxButton(this, ID_OPER_HISTORY_NEXT,"下一页");
	wxButton* previousButton = new wxButton(this, ID_OPER_HISTORY_PREVIOUS,"上一页");
	wxButton* cancelButton = new wxButton(this, ID_OPER_HISTORY_CANCEL,"取消");
	wxButton* rollbackButton = new wxButton(this, ID_OPER_HISTORY_ROLLBACK,"回滚最近一条操作");

	//布局调整器
	wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bottomSizer = new wxBoxSizer(wxHORIZONTAL);  //界面下面的布局器
	bottomSizer->AddSpacer(20);
	bottomSizer->Add(previousButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->Add(nextButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->Add(cancelButton, 0, wxALL|wxALIGN_CENTRE, 10);
	bottomSizer->AddSpacer(300);
	bottomSizer->Add(rollbackButton, 0, wxFIXED_MINSIZE|wxALIGN_CENTRE, 10);

	topSizer->Add(_pOperHisgrid, 0, wxEXPAND | wxALL, 2);
	topSizer->AddSpacer(2);
	topSizer->Add(bottomSizer, 0, wxEXPAND | wxALL, 2);


	SetSizerAndFit(topSizer);
}

//获取现金变动表的记录
int OperHistoryDialog::GetCashFlowData(vector<CASHFLOW>& vecOut){
	
	//查询现金
	Runtime::getInstance()->sqlite.setSql(QRY_CASH_FLOW);
	Runtime::getInstance()->sqlite.prepare();
	Runtime::getInstance()->sqlite.bindInt(1, Runtime::getInstance()->CurComposeID);

	while ( 1 == Runtime::getInstance()->sqlite.step() ) {
		CASHFLOW tmpCashFlow;
		tmpCashFlow.keyid = Runtime::getInstance()->sqlite.getColumnInt(0);
		tmpCashFlow.composeid = Runtime::getInstance()->sqlite.getColumnInt(1);
		tmpCashFlow.operType = Runtime::getInstance()->sqlite.getColumnInt(2);
		tmpCashFlow.changeCash = Runtime::getInstance()->sqlite.getColumnDouble(3);
		tmpCashFlow.afterCash = Runtime::getInstance()->sqlite.getColumnDouble(4);
		tmpCashFlow.fundShare = Runtime::getInstance()->sqlite.getColumnDouble(5);
		tmpCashFlow.changeTime = Runtime::getInstance()->sqlite.getColumnString(6);
		tmpCashFlow.reason = Runtime::getInstance()->sqlite.getColumnString(7);

		vecOut.push_back(tmpCashFlow);
	}

	return vecOut.size();
}

void OperHistoryDialog::OnRollback(wxCommandEvent& event)//release 模式下，买入操作获取不到控件的值
{
	CASHFLOW rollbackData = *_vecCashFlowData.begin();

	if(rollbackData.operType == sellStockType){
		wxMessageBox("卖出暂不支持回滚，请手工回滚现金表和交易表");//因为如果回滚，回删除整条交易记录，会连同以前的买入记录一起删除
		return;
	}

	//只能回滚当天的操作，隔天的由于已登记市值表，暂不支持回滚
	wxDateTime datetime(wxDateTime::GetTimeNow());
	string todayStr = datetime.Format("%Y-%m-%d");
	string rollDay = rollbackData.changeTime.substr(0,10);
	if(todayStr != rollDay){
		wxMessageBox("只支持回滚当天的操作");
		return;
	}

	wxMessageBox("注意：只能回滚最近一条，并只能用于记录净值之前！如果不是，请退出进程");

	if(rollbackData.operType == buyStockType){//还需要删除交易记录
		Runtime::getInstance()->sqlite.setSql(DEL_TRADE_INFO);
		Runtime::getInstance()->sqlite.prepare();

		if (Runtime::getInstance()->sqlite.step() < 0) {
			wxMessageBox(Runtime::getInstance()->sqlite.errString);
			return;
		}

		MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
		myFrame->RefreshLogInfo();
		myFrame->RefreshGoodsAndValue();
	}


	Runtime::getInstance()->sqlite.setSql(DEL_CASH_FLOW);
	Runtime::getInstance()->sqlite.prepare();
	Runtime::getInstance()->sqlite.bindInt(1, rollbackData.keyid);

	if (Runtime::getInstance()->sqlite.step() < 0) {
		wxMessageBox(Runtime::getInstance()->sqlite.errString);
		return;
	}

	_vecCashFlowData.clear();
	GetCashFlowData(_vecCashFlowData);
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);
	_pOperHisgrid->Refresh();
	wxMessageBox("回滚成功！");
}

void OperHistoryDialog::OnNext(wxCommandEvent& event)
{
	if(_vecCashFlowData.size() <= curPage*numOnePage ){
		return;
	}
	++curPage;
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();
}

void OperHistoryDialog::OnPrevious(wxCommandEvent& event)
{
	if(curPage==1){
		return;
	}

	--curPage;
	setPageFromVector2GridTable(_vecCashFlowData, _pDS, numOnePage, curPage);

	_pOperHisgrid->AutoSizeRows();
	_pOperHisgrid->AutoSizeColumns();

}

void OperHistoryDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

/*获取第N页的数据
*@vSrc：数据源向量；gridStrTab：grid表格的数据源；numInOnePage：一页中的行数；pageNum：填充到grid表格的页数
*/
void OperHistoryDialog::setPageFromVector2GridTable(vector<CASHFLOW>& vSrc, wxGridStringTable* gridStrTab,
													int numInOnePage, int pageNum)
{	
	vector<CASHFLOW> tmpSrc;
	for(int i=0; i<vSrc.size(); ++i) { //过滤掉不是当前组合的信息
		if(vSrc[i].composeid == Runtime::getInstance()->CurComposeID){
			tmpSrc.push_back(vSrc[i]);
		}
	}

	if (tmpSrc.size() == 0 && gridStrTab->GetNumberRows()<=1) { //以防日志记录为空时，没有数据导致界面被扭曲的情况
		gridStrTab->AppendRows(8);
		return;
	}

	if( tmpSrc.size() < numInOnePage * (pageNum - 1) ){ //记录数小于上一页的总行数，直接返回
		return;
	}

	gridStrTab->Clear();
	//规整gridStringTable中的行数为numInOnePage
	if(gridStrTab->GetNumberRows() > getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum)){
		gridStrTab->DeleteRows(0, gridStrTab->GetNumberRows() - getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum));

	}else{
		gridStrTab->AppendRows(getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) - gridStrTab->GetNumberRows());
	}

	vector<CASHFLOW>::iterator iter = tmpSrc.begin();
	iter += numInOnePage * (pageNum - 1);// 跳转到pageNum页的记录
	for(int i = 0; i < getRowNumInOnePage(tmpSrc.size(), numInOnePage, pageNum) && iter != tmpSrc.end(); ++i, ++iter){
		gridStrTab->SetValue(i,0,iter->changeTime);
		gridStrTab->SetValue(i,1,wxString::Format("%d", iter->composeid));
		gridStrTab->SetValue(i,2,GetNameOfOperType(iter->operType));
		gridStrTab->SetValue(i,3,wxString::Format("%g", (iter->afterCash - iter->changeCash)));
		gridStrTab->SetValue(i,4,wxString::Format("%g", (iter->changeCash)));
		gridStrTab->SetValue(i,5,wxString::Format("%g", (iter->afterCash)));
		gridStrTab->SetValue(i,6,wxString::Format("%g", (iter->fundShare)));
		gridStrTab->SetValue(i,7,iter->reason);
	}
}

