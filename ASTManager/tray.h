
#ifndef ASTMANAGER_TRAY_H 
#define ASTMANAGER_TRAY_H

#include "wx/wx.h" 
#include "wx/taskbar.h" 
#include "wx/menu.h" 
#include "wx/icon.h" 
#include "a.xpm"
#include <shellapi.h>

#define  MENU_EXIT 1001

class MyTray : public wxTaskBarIcon { 
public: 
    MyTray();
    
    void setFrame(wxFrame* frame){
        m_pFrame = frame;
    }

    MyTray(wxFrame* frame); 
    
    bool ShowBalloon(wxString title, wxString message, unsigned int timeout = 1000000, int icon = NIIF_INFO);

private: 
    virtual wxMenu* CreatePopupMenu(); 
    void OnMenuExit(wxCommandEvent& event); 
    void OnLeftDoubleClick(wxTaskBarIconEvent& event);

    wxIcon m_aIcon; //AÍ¼±ê
    wxIcon m_nIcon; //¿Õ°×Í¼±ê
    wxFrame* m_pFrame;
    bool isShine;

    DECLARE_EVENT_TABLE() 
}; 

#endif  //ASTMANAGER_TRAY_H

