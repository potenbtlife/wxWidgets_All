#include "AdjustCashDialog.h"
#include "main.h"

//注册买入窗口的事件处理函数
BEGIN_EVENT_TABLE(AdjustCashDialog, wxDialog)    
EVT_BUTTON(ID_ADJUSTCASHCTRL_OK, AdjustCashDialog::OnOK)
EVT_BUTTON(ID_ADJUSTCASHCTRL_CANCEL, AdjustCashDialog::OnCancel)
END_EVENT_TABLE()

//买入窗口构造函数
AdjustCashDialog::AdjustCashDialog(wxWindow *parent,
                     wxWindowID id, 
                     const wxString& title,
                     const wxPoint&  pos, 
                     const wxSize& size,
                     long  style,
                     const wxString& name)
                     :wxDialog(parent,id,title,pos,size,style,name), m_curCashVaule(0), m_curShare(0)
{
    wxStaticText *buyNameText = new wxStaticText(this, -1, "现有现金余额:",wxPoint(20, 10),wxSize(100, 20));
    wxTextCtrl* curCashCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 10),wxSize(100, 20), wxTE_READONLY);
    
    wxStaticText *buyPriceText = new wxStaticText(this, -1, "输入调整后现金:",wxPoint(20, 35),wxSize(100, 20));
    m_nowCashCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 35),wxSize(100, 20));

    wxStaticText *fundShareText = new wxStaticText(this, -1, "调整后基金份额:",wxPoint(20, 60),wxSize(100, 20));
    m_nowShareCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 60),wxSize(100, 20));
    
	new wxStaticText(this, -1, "调整后债务:",wxPoint(20, 85),wxSize(100, 20));
	m_debetCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 85),wxSize(100, 20));

    new wxStaticText(this, -1, "调整原因:",wxPoint(20, 110),wxSize(100, 20));
    m_reasonCtrl = new wxTextCtrl(this,wxID_ANY, "",wxPoint(120, 110),wxSize(120, 60), wxTE_MULTILINE);

    wxButton* okButton = new wxButton(this, ID_ADJUSTCASHCTRL_OK,"确定", wxPoint(70,190), wxSize(60,30));
    wxButton* cancelButton = new wxButton(this, ID_ADJUSTCASHCTRL_CANCEL,"取消", wxPoint(160,190), wxSize(60,30));

    qryCashAndShare(Runtime::getInstance()->CurComposeID, m_curCashVaule, m_debet, m_curShare);
    
    char ctmp[128] = {0};
    sprintf(ctmp, "%.2lf", m_curCashVaule);
    curCashCtrl->SetValue(ctmp); //现金控件

	memset(ctmp,0,sizeof(ctmp));
    sprintf(ctmp, "%.2lf", m_curShare);
    m_nowShareCtrl->SetValue(ctmp); //基金份额控件
	
	memset(ctmp,0,sizeof(ctmp));
	sprintf(ctmp, "%.2lf", m_debet);
	m_debetCtrl->SetValue(ctmp); //债务控件
}

void AdjustCashDialog::OnOK(wxCommandEvent& event)
{
    string modifyCashStr = m_nowCashCtrl->GetValue();
    string modifyShareStr = m_nowShareCtrl->GetValue();
    string reasonStr = m_reasonCtrl->GetValue();
	string strDebet = m_debetCtrl->GetValue();

    double modifyCash = 0, modifyShare = 0, modifyDebet=0;
    sscanf(modifyCashStr.c_str(), "%lf", &modifyCash);
    sscanf(modifyShareStr.c_str(), "%lf", &modifyShare);
	sscanf(strDebet.c_str(), "%lf", &modifyDebet);

    if( (modifyCash<0.005 && modifyCash>-0.005) || "" == reasonStr || (modifyShare < 0.05 && modifyShare > -0.05)){
        wxMessageBox( "所有选项不能为空，请重新输入！","输入错误！", wxOK | wxICON_ERROR );
        return;
    }

    double diffShare = modifyShare - m_curShare;
    if (diffShare < 0.5 && diffShare > -0.5) { //如果由于转换导致份额的精度丢失，使用原来的份额值以防止精度丢失。
        modifyShare = m_curShare;
    }
    InsertCashRecord(Runtime::getInstance()->CurComposeID, AdjustBalance, (modifyCash-m_curCashVaule), modifyCash,modifyDebet, modifyShare, reasonStr);//变更金额先置为0，以后再补充

    MyFrame* myFrame = (MyFrame*)wxWindowBase::FindWindowById(ID_MAIN_FRAME);
    myFrame->RefreshGoodsAndValue();

    Destroy();
}

void AdjustCashDialog::OnCancel(wxCommandEvent& event)
{
    Destroy();
    //Close(false);
}

//更新临时现金份额表
int AdjustCashDialog::BuyUpdateCash(double changeCash){
/*
    //更新数据库，以数据库的为最终标准
    gSqlite.setSql(BuyUpdateTmpSql);
    gSqlite.prepare();
    gSqlite.bindDouble(1, changeCash);
    gSqlite.bindInt(2, Runtime::getInstance()->CurComposeID);

    if(gSqlite.step() < 0 && gSqlite.finalize()){ //写失败！
        wxMessageBox(gSqlite.errString);
        return -1;
    };
    gSqlite.finalize();

    //更新界面显示
    wxTextCtrl* cashCtrl = (wxTextCtrl*)wxWindowBase::FindWindowById(ID_CASH_TEXTCTRL);
    string sCash = cashCtrl->GetValue();
    double fCash = atof(sCash.c_str()); //现金
    double allCash = fCash - changeCash;
    char cAllCash[16] = {0};
    sprintf(cAllCash, "%.2f", allCash);
    cashCtrl->SetValue(cAllCash);
*/
    return 1;
}

