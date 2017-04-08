#ifndef ASM_BUGORSELLFOUNDDIALOG_H
#define ASM_BUGORSELLFOUNDDIALOG_H

#include <wx/dialog.h>
#include "Runtime.h"

enum BUYORSELLFOUNDDIALOGID{
    ID_AMT_TEXTCTRL,
    ID_ADVICE_TEXTCTRL,
    ID_BUYORSELLFOUNDDIALOGID_OK_BUTTON
};

class BuyOrSellFoundDialog : public wxDialog{
public:

    //构造函数,type:0 申购；1：赎回
    BuyOrSellFoundDialog(int type, wxWindow *parent,
        wxWindowID id, 
        const wxString& title,
        const wxPoint&  pos = wxDefaultPosition, 
        const wxSize& size = wxDefaultSize,
        long  style = wxDEFAULT_DIALOG_STYLE,
        const wxString &  name = wxDialogNameStr) : wxDialog(parent,id,title,pos,size,style,name), m_type(type){

        if(m_type == buyFundType){
            new wxStaticText(this, -1, "申购金额:",wxPoint(10, 10),wxSize(60, 20));
        }else {
            new wxStaticText(this, -1, "赎回金额:",wxPoint(10, 10),wxSize(60, 20));
        }

        amtCtrl = new wxTextCtrl(this, ID_AMT_TEXTCTRL, "", wxPoint(70,10),wxSize(120,20));

        new wxStaticText(this, -1, "原因:",wxPoint(10, 40),wxSize(60, 20));
        adviceCtrl = new wxTextCtrl(this, ID_ADVICE_TEXTCTRL, "", wxPoint(70,40),wxSize(120,60), wxTE_MULTILINE);

        okbutton = new wxButton(this, ID_BUYORSELLFOUNDDIALOGID_OK_BUTTON, "确定", wxPoint(80, 120), wxSize(60,30));
    }


    void OnOk(wxCommandEvent& event){
        string amtStr = amtCtrl->GetValue();
        string reason = adviceCtrl->GetValue();
        double changeCash = atof(amtStr.c_str());
        
        if((changeCash > -0.005 && changeCash < 0.005) || reason == "") {
            wxMessageBox("输入错误，不能为空!");
            return;
        }

        double curCash=0, debet=0, curShare=0;
        qryCashAndShare(Runtime::getInstance()->CurComposeID, curCash, debet, curShare);

        string datetime,value_advice, detailInfo;
        double fundShare=0, fundValue=0,marketvalue=0,cash=0; //主要为了获取基金每份净值
        qryValueInfo(Runtime::getInstance()->CurComposeID, datetime, value_advice, detailInfo, fundShare, fundValue,marketvalue,cash);//获取value_info数据，主要是为了获取净值数据

        string reasonStr;
        double newShare=0, newCash=0;
        if (m_type == buyFundType){
            newCash = curCash + changeCash;
            double addShare = changeCash/fundValue;
            newShare = curShare + addShare;
            reasonStr = "申购：" + trim(reason);
        }else{
			changeCash = -1*changeCash;
            newCash = curCash + changeCash;
            double reduceShare = changeCash/fundValue;// 已为负数
            newShare = curShare + reduceShare;
            reasonStr = "赎回：" + trim(reason);
        }
        
        InsertCashRecord(Runtime::getInstance()->CurComposeID,m_type, changeCash, newCash, debet, newShare, reasonStr);

        Destroy();
    }

    wxTextCtrl* amtCtrl;
    wxTextCtrl* adviceCtrl;
    wxButton* okbutton;
    int m_type; //类型，0：申购； 1：赎回

    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(BuyOrSellFoundDialog, wxDialog)    
EVT_BUTTON(ID_OK_BUTTON, OnOk)
END_EVENT_TABLE()

#endif //ASM_BuyOrSellFoundDialog_H