#ifndef INCLUDED_RPTDIALOG_H
#define INCLUDED_RPTDIALOG_H

//对不支持预编译编译器要包含"wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
       #include "wx/wx.h"
#endif

#include "common.h"
#include <map>

enum{
	ID_REPORT_GRID,
	ID_PREVIOUS_BUTTON,
	ID_NEXTPAGE_BUTTON
};

class RptDialog: public wxDialog{
public:
	RptDialog(wxFrame *parent, wxWindowID id, const wxString &title, wxPoint& point, wxSize& size);
    ~RptDialog();

    DECLARE_EVENT_TABLE();

private:
    void OnNextPage(wxCommandEvent& event);
    void OnPrePage(wxCommandEvent& event);

    //计算每个象限的占用时间比率
    void RptDialog::calcRatioOfEachPriority(vector<DataContainer>& vecAllData, wxGridStringTable& statisticDS, int numInOnePage, int pageNum);

    wxGrid *rptGrid; //报表表格
    wxGridStringTable *dataSource; //报表表格数据源
    wxGrid *statisticGrid; //统计信息表格
    wxGridStringTable *statisticDS; //统计信息数据源
};

#endif