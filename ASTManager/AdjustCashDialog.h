#ifndef ADJUSTCASHDIALOG_H
#define ADJUSTCASHDIALOG_H

#include "common.h"
#include "CDBSqlite.h"
#include "Runtime.h"
using namespace std;

enum
{
    ID_ADJUSTCASHCTRL_OK = 600,
    ID_ADJUSTCASHCTRL_CANCEL,
};

//定义 BuyDialog 类相关的东西
class AdjustCashDialog: public wxDialog
{
public:
    //构造函数
    AdjustCashDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);

    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    int BuyUpdateCash(double cash); //更新临时现金份额表
    DECLARE_EVENT_TABLE();

private:

    wxTextCtrl* m_nowCashCtrl; //调整现金值的控件
    wxTextCtrl* m_nowShareCtrl; //调整基金份额的控件
    wxTextCtrl* m_reasonCtrl;

    double m_curCashVaule, m_curShare;
};


#endif //ADJUSTCASHDIALOG_H