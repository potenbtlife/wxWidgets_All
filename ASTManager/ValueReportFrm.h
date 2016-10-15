#ifndef AST_VALUEREPORT_H
#define AST_VALUEREPORT_H
#include "common.h"
#include "Runtime.h"
#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"
#include <wx/aui/aui.h>

const std::string QryReportDataSql = "select recordtime,fund_value,reference_index from value_info \
                where compose_id=? order by recordtime asc";

struct MYDATA{
    wxDateTime recored_time;
    double value;
    double index;
}; //保存组成报表的原子数据


enum REPORT_ID{
    ID_DATA_PANEL,
    ID_SHOWMONTH_BUTTON,
    ID_SHOWYEAR_BUTTON,
    ID_SHOWALL_BUTTON
};

class ValueReportFrm: public wxFrame
{
public:

    //构造函数
    /*ValueReportDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr);*/

    ValueReportFrm();
    virtual ~ValueReportFrm();
    
private:
    wxChartPanel *m_chartPanel;

    //param[in]cycle（周期类型）：0：展示所有， 1:一月内，2：一年内
    Chart* CreateChart(int cycle = 0);
    int getReportDataFromDb(vector<MYDATA>& data);
    
    //get change of recent one day
    //param[in] type 0: value change; 1: index change
    std::string getLastDayChange(int type); 

    //get change of recent one month
    //param[in] type 0: value change; 1: index change
    std::string getRecMonthChange(int type);

    //get change of recent one year
    //param[in] type 0: value change; 1: index change
    std::string getRecYearChange(int type);

    void OnShowMonth(wxCommandEvent& event);
    void OnShowYear(wxCommandEvent& event);
    void OnShowAll(wxCommandEvent& event);
    void OnExit(wxCommandEvent& WXUNUSED(event));

    vector<MYDATA> m_vecData;

    wxAuiManager *m_auiMan;

    DECLARE_EVENT_TABLE();
};


#endif //AST_VALUEREPORT_H

