#include "timeMgr.h"

IMPLEMENT_APP(TimeMgrApp)

/* 程序从这里开始执行，类似非wxWindows程序中的main() */
bool TimeMgrApp::OnInit() {
    MainFrame* frame = new MainFrame(_T("时间管理--时光匆匆，最大此刻价值"), wxPoint(0, 0), wxSize(800, 600));
    frame->CreateStatusBar();

    //wxStaticText *startTimeStaticText = new wxStaticText(frame,-1,"开始时间：",wxPoint(0,0),wxSize(60,50));
    //wxTimePickerCtrl *startTimePickerCtrl = new wxTimePickerCtrl(frame,100,wxDefaultDateTime,wxPoint(0,35),wxSize(100,30),wxTP_DEFAULT,wxDefaultValidator,"my time ctrl");

    frame->Centre();
    frame->Show(TRUE);
    SetTopWindow(frame);

    /*StartDialog *mydialog = new StartDialog(NULL,100,"开始 对话框",wxPoint(0, 0),wxSize(240, 120),wxDEFAULT_DIALOG_STYLE,"11111");
    mydialog ->Centre();
    mydialog->Show();*/

    return true;
}

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
    //文件菜单
    wxMenu* menuFile = new wxMenu;

    menuFile->Append(ID_MENU_REPORT, "&Report\tCtrl-R", "显示报表");
    menuFile->Append(ID_MENU_START, "&Start...\tCtrl-S", "开始计时");
    menuFile->Append(ID_MENU_END, "&End...\tCtrl-E", "结束计时");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    /*  //帮助菜单
        wxMenu *menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT);
        wxMenuBar *menuBar = new wxMenuBar;

        //添加到菜单栏中
        menuBar->Append( menuFile, "&File" );
        menuBar->Append( menuHelp, "&Help" );
        SetMenuBar( menuBar );
        CreateStatusBar();
        SetStatusText( "Welcome to wxWidgets!" );*/

    //添加到菜单栏中
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    SetMenuBar(menuBar);

    wxStaticText* objectText = new wxStaticText(this, -1, "目标:", wxDefaultPosition, wxDefaultSize);
    objectCtrl = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(180, 300), wxTE_MULTILINE | wxHSCROLL);

    wxStaticText* dotextText = new wxStaticText(this, -1, "事项:", wxDefaultPosition, wxDefaultSize);
    dotextCtrl = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(180, 120), wxTE_MULTILINE | wxHSCROLL);

    wxStaticText* historyText = new wxStaticText(this, -1, "历史事项:", wxDefaultPosition, wxSize(60,15));
    historyListBox = new wxListBox(this, ID_HISTORY_LISTBOX, wxDefaultPosition, wxSize(180, 180), 0, NULL, wxLB_HSCROLL);

    resetButton = new wxButton(this, ID_RESET_BUTTON, "置0", wxDefaultPosition, wxSize(30,30), 0);
    endButton = new wxButton(this, ID_END_BUTTON, "结束", wxDefaultPosition, wxSize(70,100), 0);

    wxString tmpString[] = {"第1象限", "第2象限", "第3象限", "第4象限"};
    priorityChoice = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize, 4, tmpString);

    priorityChoice->SetSelection(0);
    //resetButton->CentreOnParent(wxHORIZONTAL);
    //endButton->CentreOnParent(wxHORIZONTAL);
    //endButton->Disable();
   

    wxBoxSizer* objectSizer = new wxBoxSizer(wxVERTICAL);
    objectSizer->Add(objectText, 0, wxALIGN_LEFT|wxSHAPED, 0);
    objectSizer->Add(objectCtrl, 0, wxALIGN_LEFT|wxSHAPED, 0);

    wxBoxSizer* thingsSizer = new wxBoxSizer(wxVERTICAL);
    thingsSizer->Add(dotextText, 0, wxALIGN_LEFT|wxSHAPED, 0);
    thingsSizer->Add(dotextCtrl, 0, wxALIGN_LEFT|wxSHAPED, 0);
    thingsSizer->Add(historyText, 0, wxALIGN_LEFT|wxSHAPED, 0);
    thingsSizer->Add(historyListBox, 0, wxALIGN_LEFT|wxSHAPED, 0);

    wxBoxSizer* rightUpSizer = new wxBoxSizer(wxVERTICAL);
    rightUpSizer->Add(resetButton, 0, wxALIGN_TOP|wxALIGN_RIGHT, 0);
    rightUpSizer->AddSpacer(75);
    rightUpSizer->Add(priorityChoice, 0, wxALIGN_BOTTOM|wxALIGN_LEFT, 0);
    rightUpSizer->AddSpacer(22);
    wxBoxSizer* rightButtomSizer = new wxBoxSizer(wxVERTICAL);
    rightButtomSizer->Add(endButton, 0, wxALIGN_TOP|wxALIGN_RIGHT, 0);

    wxBoxSizer* ctrlSizer = new wxBoxSizer(wxVERTICAL);
    ctrlSizer->Add(rightUpSizer, 0, wxALIGN_LEFT|wxSHAPED, 0);
    ctrlSizer->Add(rightButtomSizer, 0, wxALIGN_TOP|wxALIGN_LEFT|wxSHAPED, 0);

    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(objectSizer, 0, wxALIGN_LEFT|wxEXPAND|wxSHAPED, 0);
    topSizer->Add(thingsSizer, 0, wxALIGN_LEFT|wxEXPAND|wxSHAPED, 0);
    topSizer->Add(ctrlSizer, 0.8, wxALIGN_LEFT|wxEXPAND|wxSHAPED, 0);

    SetSizerAndFit(topSizer);
    /*SetSizerAndFit(objectSizer);
    SetSizerAndFit(thingsSizer);
    SetSizerAndFit(ctrlSizer);*/

    //不再需要按开始按钮，自动开始计时
    OnReset(*(new wxCommandEvent()));

    arrayStr = new wxArrayString();
    refreshHistoryListBox();
    refreshObjectText();
}

void MainFrame::OnReset(wxCommandEvent& event) {
    startTime = wxDateTime::Now();

    //this->resetButton->Disable();
    endButton->Enable();
    //wxMessageBox(startTime.Format("%H:%M:%S"));
}

void MainFrame::OnEnd(wxCommandEvent& event) {
    endTime = wxDateTime::Now();
    wxTimeSpan tmp = endTime - startTime;

    int lineNum = dotextCtrl->GetNumberOfLines();
    std::string text;

    for (int i = 0; i < lineNum; i++) {
        text += dotextCtrl->GetLineText(i);

        if (i < lineNum - 1) {
            text += "\n";
        }
    }

    if (text == "") {
        wxMessageBox("事项不能为空！");
        return;
    }

    string strPriority = priorityChoice->GetString(priorityChoice->GetSelection());//获取优先级

    gSqlite.open();
    gSqlite.setSql(gInsertSql);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    std::string tmpStartTime = startTime.Format("%Y-%m-%d %H:%M:%S");
    std::string tmpEndTime = endTime.Format("%Y-%m-%d %H:%M:%S");
    gSqlite.bindString(1, tmpStartTime.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindString(2, tmpEndTime.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindString(3, text.c_str(), -1, SQLITE_STATIC);
    gSqlite.bindString(4, strPriority.c_str(), -1, SQLITE_STATIC);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    gSqlite.finalize();//释放pstmt指向的对象，防止内存泄露和多次准备导致数据返还忙状态

    dotextCtrl->SetLabel("");

    //wxMessageBox(startTime.Format("%Y%m%d %H:%M:%S")+","+endTime.Format("%H:%M:%S")+", inter="+tmp.Format("%H:%M:%S")+", "+text);

    //resetButton->Enable();
    //endButton->Disable();
    //不再需要按开始按钮，自动开始计时
    OnReset(*(new wxCommandEvent()));

    refreshHistoryListBox();
    saveObjectDataToDb();
}

void MainFrame::OnReport(wxCommandEvent& event) {
    RptDialog* rptDialog = new RptDialog(this, wxID_ANY, "报表", wxPoint(0, 0), wxSize(800, 500)); //(800,500)

    rptDialog->Centre();
    rptDialog->Show(true);
    return;
}

//双击历史事项的列表后，在事项对话框显示当前选择内容
void MainFrame::OnSelectHistory(wxCommandEvent& event) {
    //wxMessageBox("MainFrame::OnSelectHistory");    
    dotextCtrl->SetLabel(historyListBox->GetString(historyListBox->GetSelection()).Trim());
    wxString selectedHistory = historyListBox->GetString(historyListBox->GetSelection());

    //重置优先级选择框
    for(int i = 0; i < m_vecHistoryInfo.size(); ++i) {
        if(selectedHistory == m_vecHistoryInfo[i].things) {
            int index = priorityChoice->FindString(m_vecHistoryInfo[i].priority.Trim());
            if( index != wxNOT_FOUND ) {
                priorityChoice->SetSelection(index);
                break;
            }
        }
    }
}

void MainFrame::refreshHistoryListBox() {
    gSqlite.open();
    gSqlite.setSql(gQryHistory);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    arrayStr->Clear();
    m_vecHistoryInfo.clear();

    for (int i = 0; 1 == gSqlite.step() && i < 10; ++i) {
        HistoryInfo tmpHisInfo;
        tmpHisInfo.things = gSqlite.getColumnString(0);
        tmpHisInfo.priority = gSqlite.getColumnString(1);
        m_vecHistoryInfo.push_back(tmpHisInfo);

        arrayStr->Add(tmpHisInfo.things);
    }

    gSqlite.finalize();//释放pstmt指向的对象，防止内存泄露和多次准备导致数据返还忙状态
    if(!arrayStr->IsEmpty()) {
        historyListBox->InsertItems(*arrayStr, 0);
    }
}

void MainFrame::refreshObjectText() {
    gSqlite.open();
    gSqlite.setSql(gGetObject);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    string objectStr;

    if (1 == gSqlite.step()) {
        objectStr = gSqlite.getColumnString(0);
    }
    
    char test[100];
    memset(test, 0, 100);
    memcpy(test, objectStr.c_str(), 100);
    //wxMessageBox(objectStr);
    gSqlite.finalize();//释放pstmt指向的对象，防止内存泄露和多次准备导致数据返还忙状态
    objectCtrl->SetLabelText(objectStr);
}


MainFrame::~MainFrame() {
    saveObjectDataToDb();
}

void MainFrame::saveObjectDataToDb(){
    int lineNum = objectCtrl->GetNumberOfLines();
    std::string text;

    for (int i = 0; i < lineNum; i++) {
        text += objectCtrl->GetLineText(i).Trim();

        if (i < lineNum - 1) {
            text += "\r\n"; //windows下的换行符
        }
    }

    gSqlite.open();
    gSqlite.setSql(gUpdateObject);

    if (gSqlite.prepare() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    }

    gSqlite.bindString(1, text.c_str(), -1, SQLITE_STATIC);

    if (gSqlite.step() < 0) {
        wxMessageBox(gSqlite.errString);
        return;
    };

    gSqlite.finalize();//释放pstmt指向的对象，防止内存泄露和多次准备导致数据返还忙状态
}
