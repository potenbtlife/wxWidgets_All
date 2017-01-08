#ifndef FINANCEFEE2BUYDIALOG_H
#define FINANCEFEE2BUYDIALOG_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

enum
{
	ID_FEE2BUE_OK = 500, //确定按钮标识
	ID_FEE2BUE_CANCEL,
	ID_DAYS_OFMONTH_CTRL
};

//定义 BuyDialog 类相关的东西
class FinanceFee2BuyDialog: public wxDialog
{
public:
    //构造函数
    FinanceFee2BuyDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
	void OnCalcFee(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();

private:

	wxTextCtrl* _curDebetCtrl;
    wxTextCtrl* _weekRateCtrl; 
	wxTextCtrl* _daysOfMonthCtrl;
	wxTextCtrl* _feeCtrl;

	double _curCash, _debet, _curShare;
	double _rateDay; //负债日利率
};


#endif //FINANCEFEE2BUYDIALOG_H