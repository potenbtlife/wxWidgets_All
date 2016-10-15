#include "common.h"
#include "DownLoadOneTbl.h"

//注册买入窗口的事件处理函数
BEGIN_EVENT_TABLE(DownLoadOneTblDialog, wxDialog)    
	EVT_BUTTON(ID_DLOT_OK,   DownLoadOneTblDialog::OnOK)
	EVT_BUTTON(ID_DLOT_CANCEL,  DownLoadOneTblDialog::OnCancel)
END_EVENT_TABLE()

//买入窗口构造函数
DownLoadOneTblDialog::DownLoadOneTblDialog(string& stockId,
                    wxWindow *parent,
					wxWindowID id, 
					const wxString& title,
					const wxPoint&  pos, 
					const wxSize& size,
					long  style,
					const wxString& name)
					:wxDialog(parent,id,title,pos,size,style,name),_stockId(stockId)
{
	wxStaticText *buyCodeText = new wxStaticText(this, -1, "标的代码:",wxPoint(10, 10),wxSize(60, 20));
	_codeCtrl = new wxTextCtrl(this, wxID_ANY, "0",wxPoint(70, 10),wxSize(80, 20));
	
	wxButton* okButton = new wxButton(this, ID_DLOT_OK,"确定", wxPoint(30,60), wxSize(60,30));
	wxButton* cancelButton = new wxButton(this, ID_DLOT_CANCEL,"取消", wxPoint(120,60), wxSize(60,30));
}

void DownLoadOneTblDialog::OnOK(wxCommandEvent& event)//release 模式下，买入操作获取不到控件的值
{
    _stockId = _codeCtrl->GetValue();
	Destroy();
	//Close(false);
}

void DownLoadOneTblDialog::OnCancel(wxCommandEvent& event)
{
	Destroy();
	//Close(false);
}

