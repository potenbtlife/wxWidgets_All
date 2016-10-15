#ifndef DOWNLOADONETBL_H
#define DOWNLOADONETBL_H

using namespace std;


enum
{
	ID_DLOT_OK,
	ID_DLOT_CANCEL
};

//定义 BuyDialog 类相关的东西
class DownLoadOneTblDialog: public wxDialog
{
public:
	//构造函数
	DownLoadOneTblDialog(string& stockId,
        wxWindow *parent,
		wxWindowID id, 
		const wxString& title,
		const wxPoint&  pos = wxDefaultPosition, 
		const wxSize& size = wxDefaultSize,
		long  style = wxDEFAULT_DIALOG_STYLE,
		const wxString &  name = wxDialogNameStr);

    
	void OnOK(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

	DECLARE_EVENT_TABLE();

private:
    wxTextCtrl *_codeCtrl;
    string& _stockId;
};


#endif //DOWNLOADONETBL_H