#ifndef ASM_DOWNLOADNEWSDIALOG_H
#define ASM_DOWNLOADNEWSDIALOG_H

#include <wx/dialog.h>
#include "Runtime.h"

enum DOWNLOADNEWSDAILOGID{
    ID_BEGINTIME_CTRL,
    ID_ENDTIME_CTRL,
    ID_OK_BUTTON
};

class DownLoadNewsDialog : public wxDialog{
public:

    //构造函数
    DownLoadNewsDialog(wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        wxDateTime& begtime,
        wxDateTime& endtime,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr) : wxDialog(parent,id,title,pos,size,style,name),m_begtime(begtime),m_endtime(endtime){
            
            wxStaticText *beginTimeText = new wxStaticText(this, -1, "开始时间:",wxPoint(0, 0),wxSize(60, 20));
            beginTimeCtrl = new wxDatePickerCtrl(this,wxID_ANY,wxDefaultDateTime,
                wxPoint(0, 22),wxSize(120, 20), wxDP_DROPDOWN,wxDefaultValidator);

            wxStaticText *endTimeText = new wxStaticText(this, -1, "结束时间:",wxPoint(0, 60),wxSize(60, 20));
            endTimeCtrl = new wxDatePickerCtrl(this,wxID_ANY,wxDefaultDateTime,
                wxPoint(0, 82),wxSize(120, 20), wxDP_DROPDOWN,wxDefaultValidator);

            okbutton = new wxButton(this, ID_OK_BUTTON, "确定", wxPoint(0, 110), wxSize(60,30));


    }

    void OnOk(wxCommandEvent& event);

    wxDatePickerCtrl* beginTimeCtrl;
    wxDatePickerCtrl* endTimeCtrl;
    wxButton* okbutton;

    wxDateTime& m_begtime;
    wxDateTime& m_endtime;
    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(DownLoadNewsDialog, wxDialog)    
    EVT_BUTTON(ID_OK_BUTTON, OnOk)
END_EVENT_TABLE()

void DownLoadNewsDialog::OnOk(wxCommandEvent& event){
    m_begtime = beginTimeCtrl->GetValue();
    m_endtime = endTimeCtrl->GetValue();

    Destroy();
}

#endif //ASM_DOWNLOADNEWSDIALOG_H