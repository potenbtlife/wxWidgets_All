#ifndef OperHistoryDialog_H
#define OperHistoryDialog_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

struct CASHFLOW{
	int keyid;
	int composeid;
	int operType;
	double changeCash;
	double afterCash;
	double fundShare;
	string changeTime;
	string reason;
};

enum
{
	ID_OPER_HISTORY_ROLLBACK = 1400, //OperHistoryDialog 回滚按钮标识
	ID_OPER_HISTORY_CANCEL = 1401, //OperHistoryDialog取消按钮标识
	ID_OPER_HISTORY_GRID,
	ID_OPER_HISTORY_NEXT,
	ID_OPER_HISTORY_PREVIOUS
};

//定义 OperHistoryDialog 类相关的东西
class OperHistoryDialog: public wxFrame
{
public:
	//构造函数
	OperHistoryDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

	int GetCashFlowData(vector<CASHFLOW>& vecOut); //获取现金变动表的记录

	void OnRollback(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

private:
	wxGrid* _pOperHisgrid; //展现操作历史列表grid对象

	void setPageFromVector2GridTable(vector<CASHFLOW>& vSrc, wxGridStringTable* gridStrTab,
		int numInOnePage, int pageNum);

	void OnNext(wxCommandEvent& event);
	void OnPrevious(wxCommandEvent& event);

	int curPage;//当前页码
	int numOnePage;//一页的行数
	vector<CASHFLOW> _vecCashFlowData;
	wxGridStringTable* _pDS;
};


#endif //OperHistoryDialog_H