
#ifndef TIMEMGR_TRAY_H 
#define TIMEMGR_TRAY_H

#include "wx/wx.h" 
#include "wx/taskbar.h" 
#include "wx/menu.h" 
#include "wx/icon.h" 
#include "t.xpm"
#include "timeMgr.h"

#define  MENU_EXIT 1001

class TimeTray : public wxTaskBarIcon { 
public: 
    TimeTray(MainFrame* frame); 
    void OnMenuExit(wxCommandEvent& event); 
    void OnLeftDoubleClick(wxTaskBarIconEvent& event);

    virtual wxMenu* CreatePopupMenu(); 

    wxIcon* m_pTIcon; 
    MainFrame* m_pFrame;

private: 
    DECLARE_EVENT_TABLE() 
}; 

#endif  //TIMEMGR_TRAY_H

