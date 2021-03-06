#ifndef INCLUDED_HELLOWORLDAPP_H
#define INCLUDED_HELLOWORLDAPP_H

//对不支持预编译编译器要包含"wx/wx.h"
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/snglinst.h>
#include "wx/timectrl.h"
#include "wx/button.h"
#include <wx/grid.h>
#include "wx/taskbar.h"
#include "common.h"
#include <string>
#include <vector>
#include <map>
#include "CDBSqlite.h"
#include "RptDialog.h"

using namespace std;

/**
 * TimeMgrApp类 * 这个类是本程序的主窗口
 */

class TimeMgrApp : public wxApp {
public:
    virtual bool OnInit();

private:
    wxSingleInstanceChecker* m_checker;
};

enum {
    ID_RESET_BUTTON = 1000,
    ID_END_BUTTON,
    ID_MENU_REPORT,
    ID_MENU_START,
    ID_MENU_END,
    ID_HISTORY_LISTBOX
};

class MainFrame: public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    ~MainFrame();

    wxButton* resetButton; //开始按钮
    wxButton* endButton; //结束按钮
    wxTextCtrl* dotextCtrl; //事项框
    wxTextCtrl* objectCtrl; //目标框
    wxChoice* priorityChoice; //优先级选择框
    wxListBox* historyListBox; //历史事项选择框
    wxArrayString* arrayStr; //历史事项数据源数组

private:
    void OnReset(wxCommandEvent& event);
    void OnEnd(wxCommandEvent& event);
    void OnReport(wxCommandEvent& event);
    //void OnAbout(wxCommandEvent& event);

    void OnSelectHistory(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    void refreshHistoryListBox();
    void refreshObjectText();
    void saveObjectDataToDb();

    DECLARE_EVENT_TABLE();

    struct HistoryInfo{
        wxString things;
        wxString priority;
    };

    vector<HistoryInfo> m_vecHistoryInfo; //保存历史记录；
    wxTaskBarIcon taskBarIcon; //托盘图标
};


#endif // INCLUDED_HELLOWORLDAPP_H