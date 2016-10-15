#include "tray.h"

BEGIN_EVENT_TABLE(MyTray, wxTaskBarIcon) 
EVT_MENU(MENU_EXIT, MyTray::OnMenuExit) 
EVT_TASKBAR_LEFT_DCLICK(MyTray::OnLeftDoubleClick)
END_EVENT_TABLE() 

MyTray::MyTray():m_aIcon(a_xpm),m_nIcon() { 
    wxTaskBarIcon::SetIcon(m_aIcon, "ASTMgr");
} 

MyTray::MyTray(wxFrame* frame):m_pFrame(frame),m_aIcon(a_xpm),m_nIcon() { 
    wxTaskBarIcon::SetIcon(m_aIcon, "ASTMgr");
} 

void MyTray::OnMenuExit(wxCommandEvent& event) { 
    //wxMessageBox("in MyTray::OnMenuExit");
    delete m_pFrame;
    RemoveIcon();
    exit(0);
} 

void MyTray::OnLeftDoubleClick(wxTaskBarIconEvent& event){
    m_pFrame->Show(TRUE);
    
    //ShowBalloon(wxString("Oh"), wxString("SSSss"), 30000, NIIF_INFO);
}

wxMenu* MyTray::CreatePopupMenu() { 
    //wxMessageBox("in MyTray::CreatePopupMenu");
    wxMenu* menu = new wxMenu(); 

    menu->Append(MENU_EXIT, wxT("E&xit")); 

    return menu; 
}

bool MyTray::ShowBalloon(wxString title, wxString message, unsigned int timeout, int icon)
{
    if (!IsOk())
        return false;
    
    NOTIFYICONDATA notifyData;
    memset(&notifyData, 0, sizeof(notifyData));

    notifyData.cbSize = sizeof(notifyData);
    notifyData.hWnd = (HWND)((wxWindow*)m_win)->GetHWND();
    
    /*notifyData.uCallbackMessage = ::RegisterWindowMessage(wxT("wxTaskBarIconMessage"));
    notifyData.uFlags = NIF_MESSAGE;

    notifyData.uFlags |= NIF_INFO;*/
    
    notifyData.uFlags = NIF_INFO;

    wxStrncpy(notifyData.szInfo, message.c_str(), WXSIZEOF(notifyData.szInfo));
    wxStrncpy(notifyData.szInfoTitle, title.c_str(), WXSIZEOF(notifyData.szInfoTitle));
    notifyData.dwInfoFlags = icon;
    notifyData.uTimeout = timeout;

    notifyData.uID = 99;
    if (m_iconAdded){
        bool res = false;
        while(!res){
            wxMilliSleep(1000);
            res= Shell_NotifyIcon(NIM_MODIFY, &notifyData);
            DWORD test = GetLastError();
        }
        //wxMessageBox(L"Format message failed with 0x%x\n", GetLastError()); 
        return res;
    }
    else
        return false;
}
