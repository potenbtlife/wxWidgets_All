#include "tray.h"

BEGIN_EVENT_TABLE(TimeTray, wxTaskBarIcon) 
EVT_MENU(MENU_EXIT, TimeTray::OnMenuExit) 
EVT_TASKBAR_LEFT_DCLICK(TimeTray::OnLeftDoubleClick)
END_EVENT_TABLE() 

TimeTray::TimeTray(MainFrame* frame):m_pFrame(frame) { 
    m_pTIcon = new wxIcon(t_xpm); 
} 

void TimeTray::OnMenuExit(wxCommandEvent& event) { 
    //wxMessageBox("in TimeTray::OnMenuExit");
    RemoveIcon();
    delete m_pFrame;
    exit(0);
} 

void TimeTray::OnLeftDoubleClick(wxTaskBarIconEvent& event){
    m_pFrame->Show(TRUE);
}

wxMenu* TimeTray::CreatePopupMenu() { 
    //wxMessageBox("in TimeTray::CreatePopupMenu");
    wxMenu* menu = new wxMenu(); 

    menu->Append(MENU_EXIT, wxT("E&xit")); 

    return menu; 
}


