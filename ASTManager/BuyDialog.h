#ifndef BUYDIALOG_H
#define BUYDIALOG_H

#include "common.h"
#include <wx/datectrl.h>
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

extern wxGridStringTable *dataSourceLog;

enum
{
	ID_BUYCTRL_OK = 400, //BuyDialog确定按钮标识
	ID_BUYCTRL_CANCEL = 401, //BuyDialog取消按钮标识
	ID_BUYNANME_CTRL = 402,
	ID_BUYCODE_CTRL = 403,
	ID_BUYPRICE_CTRL = 404,
	ID_BUYNUM_CTRL = 405,
	ID_BUYTIME_CTRL = 406,
	ID_BUYREASON_CTRL = 407,
    ID_YONGJIN_CTRL = 408
};

//定义 BuyDialog 类相关的东西
class BuyDialog: public wxDialog
{
public:
	//构造函数
	BuyDialog(wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

    
    wxDatePickerCtrl *buyTimeCtrl; //时间控件

	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

    int BuyUpdateCash(double cash); //更新临时现金份额表
	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *buyNameCtrl;
    wxTextCtrl *buyCodeCtrl;
    wxTextCtrl *buyPriceCtrl;
    wxTextCtrl *buyNumCtrl;
    wxTextCtrl *buyReasonCtrl;
    wxTextCtrl *YongJinCtrl;
};


#endif //BUYDIALOG_H