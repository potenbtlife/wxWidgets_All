#include "FinanceFrame.h"
std::string chekcUniquFinanceIndexSql = "select 1 from finance_index where stock_id=? and report_time=?";
std::string insertFinanceIndexSql = "insert into finance_index(stock_id,report_time,report_type,cashdidept,maolilv,yingyelirunlv,jinglilv,roe,roa,yingshougrowthrate,yingshoulvrungrowthrate,zongzicangrowthrate,jingzicangrowthrate,yingshouzzl,cunhuozzl,gudingzicanzzl,zongzicanzzl,gangganlv,ziyouxijgz,guxi,shareValue) values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

std::string qryBalance = "select stock_id,report_time,report_type,static_asset,flow_asset,flow_debt,netflow_asset,static_debt,fewholder_asset,net_asset,guben,chubei,holder_asset,invisible_asset,building_device,childcort_asset,sibing_asset,other_invest,yingshou,cunhuo,cash,yingfu,flow_bankdept,static_bankdept,all_asset,all_dept,stock_share,bizhong,bond from balance_report where stock_id = ? and report_type = ? order by report_time desc";
std::string qrySunYi = "select stock_id,report_time,report_type,yingyee,untax_profit,tax,tax_profit,fewholder_profit,holder_profit,dividend,net_profit,basic_one_profit,tanxiao_one_profit,ont_dividend,cost,depreciation,sell_fee,admin_fee,lixi_fee,gross_profit,jingying_profit,othercorp_profit,bizhong from sunyi_report where stock_id =? and report_type = ? order by report_time desc";
std::string qryCashFlow = "select stock_id,report_time,report_type,jingying_net,invest_net,rongzi_net,cash_add,cash_begin,cash_end,waihui_profit,buy_device,bizhong from cashflow_report where stock_id=? and report_type = ? order by report_time desc";
//std::string qryFinanceIndex = "select stock_id,report_time,report_type,cashdidept,maolilv,yingyelirunlv,jinglilv,roe,roa,yingshougrowthrate,yingshoulvrungrowthrate,zongzicangrowthrate,jingzicangrowthrate,yingshouzzl,cunhuozzl,gudingzicanzzl,zongzicanzzl,gangganlv,ziyouxijgz from finance_index where stock_id = ? and report_type = ? order by report_time desc";
std::string qryFinanceIndex = "select a.stock_id,a.report_time,a.report_type,a.cashdidept,a.maolilv,a.yingyelirunlv,a.jinglilv,a.roe,a.roa,a.yingshougrowthrate,a.yingshoulvrungrowthrate,a.zongzicangrowthrate,a.jingzicangrowthrate,a.yingshouzzl,a.cunhuozzl,a.gudingzicanzzl,a.zongzicanzzl,a.gangganlv,a.ziyouxijgz, a.guxi, a.shareValue, b.curprice from finance_index a, stock_list b where a.stock_id = b.stock_id and a.stock_id = ? and a.report_type = ? order by a.report_time desc";

enum FinanceID {
    ID_STOCKIDNAME_COMBOBOX,
    ID_CONDITION_COMBOBOX,
    ID_BALANCE_GRID,
    ID_SUNYI_GRID,
    ID_CASHFLOW_GRID,
    ID_BALANCE_MENU,
    ID_SUNYI_MENU,
    ID_CASHFLOW_MENU,
    ID_THREETBL_MENU,
    ID_YEAR_TBL_BUTTON,
    ID_MID_TBL_BUTTON,
    ID_THREETBL_BUTTON,
    ID_BALANCETBL_BUTTON,
    ID_SUNYIBTL_BUTTON,
    ID_CASHFLOWBTL_BUTTON
};

wxString AllIndex[] = {
    "代码顺序",
    "现金/有息负债(>1)",
    "毛利率",
    "营业利润率",
    "净利率",
    "净资产收益率(ROE)",
    "总资产收益率(ROA)",
    "营业收入增长率",
    "营业利润增长率",
    "总资产增长率",
    "净资产增长率",
    "应收账款周转率(次/年)：",
    "存货周转率(次/年)",
    "固定资产周转率(次/年)",
    "总资产周转率(次/年)",
    "杠杆系数(<200%)",
    "每股估值(元)",
    "估值差"
};

BEGIN_EVENT_TABLE(FinanceFrame, wxFrame)
    EVT_COMBOBOX(ID_STOCKIDNAME_COMBOBOX, OnSelectedChange)
    EVT_TEXT_ENTER(ID_STOCKIDNAME_COMBOBOX, OnQueryCode)
    EVT_MENU(ID_BALANCE_MENU,   OnShowBalance)
    EVT_MENU(ID_SUNYI_MENU,   OnShowSunYi)
    EVT_MENU(ID_CASHFLOW_MENU,   OnShowCashFlow)
    EVT_BUTTON(ID_YEAR_TBL_BUTTON, Switch2Year)
    EVT_BUTTON(ID_MID_TBL_BUTTON, Switch2Mid)
    EVT_BUTTON(ID_THREETBL_BUTTON, ShowThreeTbl)
    EVT_BUTTON(ID_BALANCETBL_BUTTON, OnShowBalance)
    EVT_BUTTON(ID_SUNYIBTL_BUTTON, OnShowSunYi)
    EVT_BUTTON(ID_CASHFLOWBTL_BUTTON, OnShowCashFlow)
END_EVENT_TABLE()

wxArrayString FinanceFrame::m_arrayAllIDName;

FinanceFrame::~FinanceFrame() {
    m_auiMan->UnInit();
}

FinanceFrame::FinanceFrame(string stockId) : wxFrame(NULL, wxID_ANY, "Finance analyse index", wxDefaultPosition, wxSize(900, 670)),
    m_balanceFrame(NULL), m_sunYiFrame(NULL), m_cashFlowFrame(NULL), m_reportType("年报"), _stockId(stockId) {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_THREETBL_MENU, "&三表\tCtrl-T");
    menuFile->Append(ID_BALANCE_MENU, "&资产负债表\tCtrl-B");
    menuFile->Append(ID_SUNYI_MENU, "&综合损益表\tCtrl-S");
    menuFile->Append(ID_CASHFLOW_MENU, "&现金流量表\tCtrl-C");

    CreateStatusBar();
    SetStatusText("当前类型：" + m_reportType); //显示组合名称

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "操作菜单");
    SetMenuBar(menuBar);

    m_auiMan = new wxAuiManager(this);

    wxPanel* panel1 = new wxPanel(this, wxID_ANY);

    //排重插入搜索用的数组
    vector<string> vecAllIDName;
    getAllStockIdName(vecAllIDName);

	int initIndex = 0;
    for (int i = 0; i < vecAllIDName.size(); ++i) {
        m_arrayAllIDName.Add(vecAllIDName[i].c_str());
		
		if(_stockId != "" && vecAllIDName[i].find(_stockId.c_str()) != string::npos){
			initIndex = i;
		}
    }

    m_stockIdNameCB = new wxComboBox(panel1, ID_STOCKIDNAME_COMBOBOX, "", wxPoint(3, 3), wxSize(140, 28), m_arrayAllIDName, wxTE_PROCESS_ENTER);
	m_stockIdNameCB->SetSelection(initIndex);

    m_threeTblBtn = new wxButton(panel1, ID_YEAR_TBL_BUTTON, "年报", wxPoint(170, 00), wxSize(60, 28));
    m_threeTblBtn = new wxButton(panel1, ID_MID_TBL_BUTTON, "中报", wxPoint(240, 00), wxSize(60, 28));
    m_threeTblBtn = new wxButton(panel1, ID_THREETBL_BUTTON, "三表", wxPoint(310, 00), wxSize(60, 28));
    m_auiMan->AddPane(panel1, wxAuiPaneInfo().CloseButton(false).CaptionVisible(false).Top().BestSize(600, 30));

    financeIndexGrid = new wxGrid(this, ID_CASHFLOW_GRID, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, "财务指标");
    financeIndexGT = new wxGridStringTable(26, 1);
    financeIndexGT->SetRowLabelValue(0, "报告期");
    financeIndexGT->SetRowLabelValue(1, "安全性分析：");
    financeIndexGT->SetRowLabelValue(2, "现金/有息负债(>1)");
    financeIndexGT->SetRowLabelValue(3, "盈利能力分析:");
    financeIndexGT->SetRowLabelValue(4, "毛利率");
    financeIndexGT->SetRowLabelValue(5, "营业利润率");
    financeIndexGT->SetRowLabelValue(6, "净利率");
    financeIndexGT->SetRowLabelValue(7, "净资产收益率(ROE)");
    financeIndexGT->SetRowLabelValue(8, "总资产收益率(ROA)");
    financeIndexGT->SetRowLabelValue(9, "成长性分析：");
    financeIndexGT->SetRowLabelValue(10, "营业收入增长率");
    financeIndexGT->SetRowLabelValue(11, "营业利润增长率");
    financeIndexGT->SetRowLabelValue(12, "净资产增长率");
    financeIndexGT->SetRowLabelValue(13, "总资产增长率");
    financeIndexGT->SetRowLabelValue(14, "管理层能力分析：");
    financeIndexGT->SetRowLabelValue(15, "应收账款周转率(天)");
    financeIndexGT->SetRowLabelValue(16, "存货周转率(天)");
    financeIndexGT->SetRowLabelValue(17, "固定资产周转率(天)");
    financeIndexGT->SetRowLabelValue(18, "总资产周转率(天)");
    financeIndexGT->SetRowLabelValue(19, "总体分析：");
    financeIndexGT->SetRowLabelValue(20, "杠杆系数(<200%)");
    financeIndexGT->SetRowLabelValue(21, "估值分析");
    financeIndexGT->SetRowLabelValue(22, "股息率(%)");
    financeIndexGT->SetRowLabelValue(23, "市净率");
    financeIndexGT->SetRowLabelValue(24, "每股估值(元)");
    financeIndexGT->SetRowLabelValue(25, "现价(元)");

    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    vector<FinanceIndexData> vecFinanceIndexData;
    getOneFinanceIndex(stock_id, vecFinanceIndexData, m_reportType);
    RefreshFinanceIndexGT(vecFinanceIndexData);


    m_fontCell = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL);
    m_fontLabel = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_BOLD);

    financeIndexGrid->SetDefaultCellFont(m_fontCell);
    financeIndexGrid->SetLabelFont(m_fontLabel);
    financeIndexGrid->SetTable(financeIndexGT, true);
    financeIndexGrid->AutoSizeRows();
    financeIndexGrid->AutoSizeColumns();
    financeIndexGrid->SetRowLabelSize(160);
    //financeIndexGrid->UseNativeColHeader(false);
    //financeIndexGrid->SetRowSize(1,5);

    m_auiMan->AddPane(financeIndexGrid, wxAuiPaneInfo().CloseButton(false).CaptionVisible(false).Centre());

    m_auiMan->Update();
    Centre();
}

void FinanceFrame::Switch2Year(wxCommandEvent& event) {
    m_reportType = "年报";
    SetStatusText("当前类型：" + m_reportType);
    OnSelectedChange(event);
}

void FinanceFrame::Switch2Mid(wxCommandEvent& event) {
    m_reportType = "中报";
    SetStatusText("当前类型：" + m_reportType);
    OnSelectedChange(event);
}

void FinanceFrame::ShowThreeTbl(wxCommandEvent& event) {
    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    ReCreateThreeTbl(stock_id, idName);
}

void FinanceFrame::OnShowBalance(wxCommandEvent& event) {
    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    if (m_balanceFrame != NULL) {
        delete m_balanceFrame;
        m_balanceFrame = NULL;
    }

    m_balanceFrame = new BalanceFrame(stock_id, idName, m_reportType);
}

void FinanceFrame::OnShowSunYi(wxCommandEvent& event) {
    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    if (m_sunYiFrame != NULL) {
        delete m_sunYiFrame;
        m_sunYiFrame = NULL;
    }

    m_sunYiFrame = new SunYiFrame(stock_id, idName, m_reportType);
}

void FinanceFrame::OnShowCashFlow(wxCommandEvent& event) {
    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    if (m_cashFlowFrame != NULL) {
        delete m_cashFlowFrame;
        m_cashFlowFrame = NULL;
    }

    m_cashFlowFrame = new CashFlowFrame(stock_id, idName, m_reportType);
}

void FinanceFrame::OnSelectedChange(wxCommandEvent& event) {
    ///wxMessageBox(m_stockIdNameCB->GetStringSelection());
    string idName = m_stockIdNameCB->GetStringSelection();
    string stock_id = idName.substr(0, idName.find("_", 0));

    string findStr = m_stockIdNameCB->GetValue(); //for test

    vector<FinanceIndexData> vecFinanceIndexData;
    getOneFinanceIndex(stock_id, vecFinanceIndexData, m_reportType);
    RefreshFinanceIndexGT(vecFinanceIndexData);

    financeIndexGrid->AutoSizeRows();
    financeIndexGrid->AutoSizeColumns();

    if (m_balanceFrame != NULL) {
        delete m_balanceFrame;
        m_balanceFrame = NULL;
    }

    if (m_sunYiFrame != NULL) {
        delete m_sunYiFrame;
        m_sunYiFrame = NULL;
    }

    if (m_cashFlowFrame != NULL) {
        delete m_cashFlowFrame;
        m_cashFlowFrame = NULL;
    }
}

void FinanceFrame::OnQueryCode(wxCommandEvent& event) {
    string findStr = m_stockIdNameCB->GetValue();

    for (int i = 0; i < m_arrayAllIDName.GetCount(); ++i) {
        if (m_arrayAllIDName[i].Find(findStr.c_str()) != wxNOT_FOUND) {
            m_stockIdNameCB->SetSelection(i);
            break;
        }
    }

    OnSelectedChange(event);
}

int getOneBalance(string stock_id, vector<BalanceData>& vecBalanceData, string& reportType) {

    Runtime::getInstance()->sqlite.setSql(qryBalance);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindString(1, stock_id.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(2, reportType.c_str(), -1, SQLITE_STATIC);

    BalanceData tmpBalanceData;

    while (1 == Runtime::getInstance()->sqlite.step()) {
        tmpBalanceData.stock_id = Runtime::getInstance()->sqlite.getColumnString(0);
        tmpBalanceData.report_time = Runtime::getInstance()->sqlite.getColumnString(1);
        tmpBalanceData.report_type = Runtime::getInstance()->sqlite.getColumnString(2);
        tmpBalanceData.static_asset = Runtime::getInstance()->sqlite.getColumnDouble(3);
        tmpBalanceData.flow_asset = Runtime::getInstance()->sqlite.getColumnDouble(4);
        tmpBalanceData.flow_debt = Runtime::getInstance()->sqlite.getColumnDouble(5);
        tmpBalanceData.netflow_asset = Runtime::getInstance()->sqlite.getColumnDouble(6);
        tmpBalanceData.static_debt = Runtime::getInstance()->sqlite.getColumnDouble(7);
        tmpBalanceData.fewholder_asset = Runtime::getInstance()->sqlite.getColumnDouble(8);
        tmpBalanceData.net_asset = Runtime::getInstance()->sqlite.getColumnDouble(9);
        tmpBalanceData.guben = Runtime::getInstance()->sqlite.getColumnDouble(10);
        tmpBalanceData.chubei = Runtime::getInstance()->sqlite.getColumnDouble(11);
        tmpBalanceData.holder_asset = Runtime::getInstance()->sqlite.getColumnDouble(12);
        tmpBalanceData.invisible_asset = Runtime::getInstance()->sqlite.getColumnDouble(13);
        tmpBalanceData.building_device = Runtime::getInstance()->sqlite.getColumnDouble(14);
        tmpBalanceData.childcort_asset = Runtime::getInstance()->sqlite.getColumnDouble(15);
        tmpBalanceData.sibing_asset = Runtime::getInstance()->sqlite.getColumnDouble(16);
        tmpBalanceData.other_invest = Runtime::getInstance()->sqlite.getColumnDouble(17);
        tmpBalanceData.yingshou = Runtime::getInstance()->sqlite.getColumnDouble(18);
        tmpBalanceData.cunhuo = Runtime::getInstance()->sqlite.getColumnDouble(19);
        tmpBalanceData.cash = Runtime::getInstance()->sqlite.getColumnDouble(20);
        tmpBalanceData.yingfu = Runtime::getInstance()->sqlite.getColumnDouble(21);
        tmpBalanceData.flow_bankdept = Runtime::getInstance()->sqlite.getColumnDouble(22);
        tmpBalanceData.static_bankdept = Runtime::getInstance()->sqlite.getColumnDouble(23);
        tmpBalanceData.all_asset = Runtime::getInstance()->sqlite.getColumnDouble(24);
        tmpBalanceData.all_dept = Runtime::getInstance()->sqlite.getColumnDouble(25);
        tmpBalanceData.stock_share = Runtime::getInstance()->sqlite.getColumnDouble(26);
        tmpBalanceData.bizhong = Runtime::getInstance()->sqlite.getColumnString(27);

        vecBalanceData.push_back(tmpBalanceData);
    }

    return 1;
}

int getOneSunYi(string stock_id, vector<SunYiData>& vecSunYiData, string& reportType) {

    Runtime::getInstance()->sqlite.setSql(qrySunYi);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindString(1, stock_id.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(2, reportType.c_str(), -1, SQLITE_STATIC);

    SunYiData tmpSunYiData;

    while (1 == Runtime::getInstance()->sqlite.step()) {
        tmpSunYiData.stock_id = Runtime::getInstance()->sqlite.getColumnString(0);
        tmpSunYiData.report_time = Runtime::getInstance()->sqlite.getColumnString(1);
        tmpSunYiData.report_type = Runtime::getInstance()->sqlite.getColumnString(2);
        tmpSunYiData.yingyee = Runtime::getInstance()->sqlite.getColumnDouble(3);
        tmpSunYiData.untax_profit = Runtime::getInstance()->sqlite.getColumnDouble(4);
        tmpSunYiData.tax = Runtime::getInstance()->sqlite.getColumnDouble(5);
        tmpSunYiData.tax_profit = Runtime::getInstance()->sqlite.getColumnDouble(6);
        tmpSunYiData.fewholder_profit = Runtime::getInstance()->sqlite.getColumnDouble(7);
        tmpSunYiData.holder_profit = Runtime::getInstance()->sqlite.getColumnDouble(8);
        tmpSunYiData.dividend = Runtime::getInstance()->sqlite.getColumnDouble(9);
        tmpSunYiData.net_profit = Runtime::getInstance()->sqlite.getColumnDouble(10);
        tmpSunYiData.basic_one_profit = Runtime::getInstance()->sqlite.getColumnDouble(11);
        tmpSunYiData.tanxiao_one_profit = Runtime::getInstance()->sqlite.getColumnDouble(12);
        tmpSunYiData.ont_dividend = Runtime::getInstance()->sqlite.getColumnDouble(13);
        tmpSunYiData.cost = Runtime::getInstance()->sqlite.getColumnDouble(14);
        tmpSunYiData.depreciation = Runtime::getInstance()->sqlite.getColumnDouble(15);
        tmpSunYiData.sell_fee = Runtime::getInstance()->sqlite.getColumnDouble(16);
        tmpSunYiData.admin_fee = Runtime::getInstance()->sqlite.getColumnDouble(17);
        tmpSunYiData.lixi_fee = Runtime::getInstance()->sqlite.getColumnDouble(18);
        tmpSunYiData.gross_profit = Runtime::getInstance()->sqlite.getColumnDouble(19);
        tmpSunYiData.jingying_profit = Runtime::getInstance()->sqlite.getColumnDouble(20);
        tmpSunYiData.othercorp_profit = Runtime::getInstance()->sqlite.getColumnDouble(21);
        tmpSunYiData.bizhong = Runtime::getInstance()->sqlite.getColumnString(22);

        vecSunYiData.push_back(tmpSunYiData);
    }

    return 1;
}

int getOneCashFlow(string stock_id, vector<CashFlowData>& vecCashFlowData, string& reportType) {

    Runtime::getInstance()->sqlite.setSql(qryCashFlow);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindString(1, stock_id.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(2, reportType.c_str(), -1, SQLITE_STATIC);

    CashFlowData tmpCashFlow;

    while (1 == Runtime::getInstance()->sqlite.step()) {
        tmpCashFlow.stock_id = Runtime::getInstance()->sqlite.getColumnString(0);
        tmpCashFlow.report_time = Runtime::getInstance()->sqlite.getColumnString(1);
        tmpCashFlow.report_type = Runtime::getInstance()->sqlite.getColumnString(2);
        tmpCashFlow.jingying_net = Runtime::getInstance()->sqlite.getColumnDouble(3);
        tmpCashFlow.invest_net = Runtime::getInstance()->sqlite.getColumnDouble(4);
        tmpCashFlow.rongzi_net = Runtime::getInstance()->sqlite.getColumnDouble(5);
        tmpCashFlow.cash_add = Runtime::getInstance()->sqlite.getColumnDouble(6);
        tmpCashFlow.cash_begin = Runtime::getInstance()->sqlite.getColumnDouble(7);
        tmpCashFlow.cash_end = Runtime::getInstance()->sqlite.getColumnDouble(8);
        tmpCashFlow.waihui_profit = Runtime::getInstance()->sqlite.getColumnDouble(9);
        tmpCashFlow.buy_device = Runtime::getInstance()->sqlite.getColumnDouble(10);
        tmpCashFlow.bizhong = Runtime::getInstance()->sqlite.getColumnString(11);
        vecCashFlowData.push_back(tmpCashFlow);
    }

    return 1;
}

int getOneFinanceIndex(string stock_id, vector<FinanceIndexData>& vecFinanceIndexData, string& reportType) {

    Runtime::getInstance()->sqlite.setSql(qryFinanceIndex);

    if (Runtime::getInstance()->sqlite.prepare() < 0) {
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindString(1, stock_id.c_str(), -1, SQLITE_STATIC);
    Runtime::getInstance()->sqlite.bindString(2, reportType.c_str(), -1, SQLITE_STATIC);

    FinanceIndexData tmpFinanceIndex;

    while (1 == Runtime::getInstance()->sqlite.step()) {
        tmpFinanceIndex.stock_id = Runtime::getInstance()->sqlite.getColumnString(0);
        tmpFinanceIndex.report_time = Runtime::getInstance()->sqlite.getColumnString(1);
        tmpFinanceIndex.report_type = Runtime::getInstance()->sqlite.getColumnString(2);
        tmpFinanceIndex.cashdidept = Runtime::getInstance()->sqlite.getColumnDouble(3);
        tmpFinanceIndex.maolilv = Runtime::getInstance()->sqlite.getColumnDouble(4);
        tmpFinanceIndex.yingyelirunlv = Runtime::getInstance()->sqlite.getColumnDouble(5);
        tmpFinanceIndex.jinglilv = Runtime::getInstance()->sqlite.getColumnDouble(6);
        tmpFinanceIndex.roe = Runtime::getInstance()->sqlite.getColumnDouble(7);
        tmpFinanceIndex.roa = Runtime::getInstance()->sqlite.getColumnDouble(8);
        tmpFinanceIndex.yingshougrowthrate = Runtime::getInstance()->sqlite.getColumnDouble(9);
        tmpFinanceIndex.yingshoulvrungrowthrate = Runtime::getInstance()->sqlite.getColumnDouble(10);
        tmpFinanceIndex.zongzicangrowthrate = Runtime::getInstance()->sqlite.getColumnDouble(11);
        tmpFinanceIndex.jingzicangrowthrate = Runtime::getInstance()->sqlite.getColumnDouble(12);
        tmpFinanceIndex.yingshouzzl = Runtime::getInstance()->sqlite.getColumnDouble(13);
        tmpFinanceIndex.cunhuozzl = Runtime::getInstance()->sqlite.getColumnDouble(14);
        tmpFinanceIndex.gudingzicanzzl = Runtime::getInstance()->sqlite.getColumnDouble(15);
        tmpFinanceIndex.zongzicanzzl = Runtime::getInstance()->sqlite.getColumnDouble(16);
        tmpFinanceIndex.gangganlv = Runtime::getInstance()->sqlite.getColumnDouble(17);
        tmpFinanceIndex.ziyouxijgz = Runtime::getInstance()->sqlite.getColumnDouble(18);
        tmpFinanceIndex.guxi = Runtime::getInstance()->sqlite.getColumnDouble(19);
        double shareValue = Runtime::getInstance()->sqlite.getColumnDouble(20);
        tmpFinanceIndex.curPrice = Runtime::getInstance()->sqlite.getColumnDouble(21);
        tmpFinanceIndex.sjl = tmpFinanceIndex.curPrice / shareValue;

        vecFinanceIndexData.push_back(tmpFinanceIndex);
    }

    return 1;

}


void FinanceFrame::RefreshFinanceIndexGT(vector<FinanceIndexData>& vecFinanceIndexData) {
    financeIndexGT->Clear();
    int curColNum = financeIndexGT->GetNumberCols();

    if (curColNum > 1 && (vecFinanceIndexData.size() < curColNum)) { //加上>1预防，只有一行被删时，/行和列都会变成 0
        financeIndexGT->DeleteCols(0, curColNum - vecFinanceIndexData.size()); //行和列都会变成 0

    } else if (vecFinanceIndexData.size() > curColNum) {
        financeIndexGT->AppendCols(vecFinanceIndexData.size() - curColNum);
    }

    int daysInOnePeriod = 360; //周期天数

    for (int i = 0; i < vecFinanceIndexData.size(); ++i) {
        if (trim(vecFinanceIndexData[i].report_type) == "中报") {
            daysInOnePeriod = 180;
        }

        financeIndexGT->SetColLabelValue(i, vecFinanceIndexData[i].report_type);//报表类型放上面的lable区

        financeIndexGT->SetValue(0, i, vecFinanceIndexData[i].report_time);
        financeIndexGT->SetValue(2, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].cashdidept * 100));
        financeIndexGT->SetValue(4, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].maolilv * 100));
        financeIndexGT->SetValue(5, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].yingyelirunlv * 100));
        financeIndexGT->SetValue(6, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].jinglilv * 100));
        financeIndexGT->SetValue(7, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].roe * 100));
        financeIndexGT->SetValue(8, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].roa * 100));
        financeIndexGT->SetValue(10, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].yingshougrowthrate * 100));
        financeIndexGT->SetValue(11, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].yingshoulvrungrowthrate * 100));
        financeIndexGT->SetValue(12, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].jingzicangrowthrate * 100));
        financeIndexGT->SetValue(13, i, wxString::Format("%.2f%%", vecFinanceIndexData[i].zongzicangrowthrate * 100));
        financeIndexGT->SetValue(15, i, wxString::Format("%.2f", daysInOnePeriod / vecFinanceIndexData[i].yingshouzzl));
        financeIndexGT->SetValue(16, i, wxString::Format("%.2f", daysInOnePeriod / vecFinanceIndexData[i].cunhuozzl));
        financeIndexGT->SetValue(17, i, wxString::Format("%.2f", daysInOnePeriod / vecFinanceIndexData[i].gudingzicanzzl));
        financeIndexGT->SetValue(18, i, wxString::Format("%.2f", daysInOnePeriod / vecFinanceIndexData[i].zongzicanzzl));
        financeIndexGT->SetValue(20, i, wxString::Format("%.2f", vecFinanceIndexData[i].gangganlv * 100));
        financeIndexGT->SetValue(22, i, wxString::Format("%.2f%%", (vecFinanceIndexData[i].guxi / vecFinanceIndexData[i].curPrice))); //股息的单位为仙，除以100变元，再乘以100转换百分比，相互抵消。
        financeIndexGT->SetValue(23, i, wxString::Format("%.2f", vecFinanceIndexData[i].sjl));
        financeIndexGT->SetValue(24, i, wxString::Format("%.2f", vecFinanceIndexData[i].ziyouxijgz));
        financeIndexGT->SetValue(25, i, wxString::Format("%.2f", vecFinanceIndexData[i].curPrice));
    }
}

//计算所有指标，包括年报和中报
void FinanceFrame::CalcAllFinanceIndex(string reportType, string stockId) {
    wxLogDebug("begin CalcAllFinanceIndex");

    vector<string> vecAllIDName;

    if (stockId == "") {
        getAllStockIdName(vecAllIDName);

    } else {
        vecAllIDName.push_back(stockId);
    }

    vector<BalanceData> vecBalanceData;
    vector<SunYiData> vecSunYiData;
    vector<CashFlowData> vecCashFlowData;
    string stock_id;

    for (int pos = 0; pos < vecAllIDName.size(); ++pos) {
        wxLogDebug("cur pos[%d]", pos);

        int index = vecAllIDName[pos].find("_", 0);

        if (index == string::npos) {
            stock_id = vecAllIDName[pos];

        } else {

            stock_id = vecAllIDName[pos].substr(0, index).c_str();
        }

        //string stock_id = "02038"; //for test
        //reportType="年报"; //for test
        vecBalanceData.clear();
        vecSunYiData.clear();
        vecCashFlowData.clear();
        getOneBalance(stock_id, vecBalanceData, reportType);
        getOneSunYi(stock_id, vecSunYiData, reportType);
        getOneCashFlow(stock_id, vecCashFlowData, reportType);

        CalcOneFinanceIndexToDb(stock_id, vecBalanceData, vecSunYiData, vecCashFlowData);
    }

    wxLogDebug("end CalcAllFinanceIndex");
}
void FinanceFrame::CalcOneFinanceIndexToDb(string stock_id, vector<BalanceData>& vecOneAllBalance, vector<SunYiData>& vecOneAllSunYi, vector<CashFlowData> vecOneAllCashFlow) {

    //最多能计算指标的个数
    int maxCanCalcNum = vecOneAllBalance.size() < vecOneAllSunYi.size() ? vecOneAllBalance.size() : vecOneAllSunYi.size();

    for (int i = 0; i < maxCanCalcNum; ++i) {
        string stock_id = vecOneAllBalance[i].stock_id;
        string report_time = vecOneAllBalance[i].report_time;

        if (QryUniqueRptData(chekcUniquFinanceIndexSql, stock_id, report_time)) { //已有
            continue;
        }

        int sunyiPos = 0;
        bool isFound = false;

        for (; sunyiPos < vecOneAllSunYi.size(); ++sunyiPos) {
            if (vecOneAllSunYi[sunyiPos].stock_id == stock_id && vecOneAllSunYi[sunyiPos].report_time == report_time) {
                isFound = true;
                break;
            }
        }

        if (!isFound) { //损益表找不到，返回
            return;
        }

        double cashdidept = 0;

        if ((vecOneAllBalance[i].flow_bankdept + vecOneAllBalance[i].static_bankdept) > 0) {
            cashdidept = vecOneAllBalance[i].cash / (vecOneAllBalance[i].flow_bankdept + vecOneAllBalance[i].static_bankdept);
        }

        double maolilv = vecOneAllSunYi[sunyiPos].gross_profit / vecOneAllSunYi[sunyiPos].yingyee;
        double yingyelirunlv = (vecOneAllSunYi[sunyiPos].gross_profit - vecOneAllSunYi[sunyiPos].sell_fee - vecOneAllSunYi[sunyiPos].admin_fee - vecOneAllSunYi[sunyiPos].lixi_fee) / vecOneAllSunYi[sunyiPos].yingyee;
        double jinglilv = vecOneAllSunYi[sunyiPos].tax_profit / vecOneAllSunYi[sunyiPos].yingyee;

        double roe = 0, roa = 0, yingshougrowthrate = 0, yingshoulvrungrowthrate = 0, zongzicangrowthrate = 0, jingzicangrowthrate = 0;
        double yingshouzzl = 0, cunhuozzl = 0, gudingzicanzzl = 0, zongzicanzzl = 0, gangganlv = 0;

        if (i == maxCanCalcNum - 1) { //最后一个元素，不能用+1计算上一年的期末
            roe = vecOneAllSunYi[sunyiPos].tax_profit / vecOneAllBalance[i].all_asset;
            roa = vecOneAllSunYi[sunyiPos].tax_profit / (vecOneAllBalance[i].all_asset + vecOneAllBalance[i].all_dept);
            yingshougrowthrate = 0;
            yingshoulvrungrowthrate = 0;
            zongzicangrowthrate = 0;
            jingzicangrowthrate = 0;
            yingshouzzl = vecOneAllSunYi[sunyiPos].yingyee / vecOneAllBalance[i].yingshou;
            cunhuozzl = vecOneAllSunYi[sunyiPos].cost / vecOneAllBalance[i].cunhuo;
            gudingzicanzzl = vecOneAllSunYi[sunyiPos].yingyee / vecOneAllBalance[i].building_device;
            zongzicanzzl = vecOneAllSunYi[sunyiPos].yingyee / vecOneAllBalance[i].all_asset;
            gangganlv = vecOneAllBalance[i].all_asset / vecOneAllBalance[i].net_asset;

        } else {
            roe = vecOneAllSunYi[sunyiPos].tax_profit / (vecOneAllBalance[i].all_asset + vecOneAllBalance[i + 1].all_asset) * 2;
            roa = vecOneAllSunYi[sunyiPos].tax_profit / (vecOneAllBalance[i].all_asset + vecOneAllBalance[i].all_dept + vecOneAllBalance[i + 1].all_asset + vecOneAllBalance[i + 1].all_dept) * 2;

            if (sunyiPos == vecOneAllSunYi.size() - 1) {
                yingshougrowthrate = 0;
                yingshoulvrungrowthrate = 0;

            } else {
                yingshougrowthrate = (vecOneAllSunYi[sunyiPos].yingyee - vecOneAllSunYi[sunyiPos + 1].yingyee) / vecOneAllSunYi[sunyiPos + 1].yingyee;

                if (vecOneAllSunYi[sunyiPos + 1].jingying_profit > 0) { //上期营业利润大于0，计算营业利润率才有意义
                    yingshoulvrungrowthrate = (vecOneAllSunYi[sunyiPos].jingying_profit - vecOneAllSunYi[sunyiPos + 1].jingying_profit) / vecOneAllSunYi[sunyiPos + 1].jingying_profit;
                }

            }

            zongzicangrowthrate = (vecOneAllBalance[i].all_asset - vecOneAllBalance[i + 1].all_asset) / vecOneAllBalance[i + 1].all_asset;
            jingzicangrowthrate = (vecOneAllBalance[i].net_asset - vecOneAllBalance[i + 1].net_asset) / vecOneAllBalance[i + 1].net_asset;
            yingshouzzl = vecOneAllSunYi[sunyiPos].yingyee / (vecOneAllBalance[i].yingshou + vecOneAllBalance[i + 1].yingshou) * 2;
            cunhuozzl = vecOneAllSunYi[sunyiPos].cost / (vecOneAllBalance[i].cunhuo + vecOneAllBalance[i + 1].cunhuo) * 2;
            gudingzicanzzl = vecOneAllSunYi[sunyiPos].yingyee / (vecOneAllBalance[i].building_device + vecOneAllBalance[i + 1].building_device) * 2;
            zongzicanzzl = vecOneAllSunYi[sunyiPos].yingyee / (vecOneAllBalance[i].all_asset + vecOneAllBalance[i + 1].all_asset) * 2;
            gangganlv = ((vecOneAllBalance[i].all_asset + vecOneAllBalance[i + 1].all_asset) / 2) / vecOneAllBalance[i].net_asset;
        }

        /*if(stock_id.find("00296") != string::npos){
            int test =1;
        }*/
        //静态现金流估值
        double allFreeCash = 0;
        int addpos = -1;

        for (int cashFlowPos = 0; cashFlowPos < vecOneAllCashFlow.size(); ++cashFlowPos) {
            if (vecOneAllCashFlow[cashFlowPos].stock_id == stock_id && vecOneAllCashFlow[cashFlowPos].report_time == report_time) {
                addpos = cashFlowPos;
            }

            if (addpos > -1) { //只相加比当前报表期之前年份的数据
                allFreeCash += vecOneAllCashFlow[cashFlowPos].jingying_net + vecOneAllCashFlow[cashFlowPos].buy_device;
            }

        }

        double averageFreeCash = allFreeCash / (vecOneAllCashFlow.size() - addpos);
        double now2FiveGrowth = 0; //1-5年现金流增长率
        double afterFiveGrowth = 0; //5年后永续现金流增长率
        double zhexianlv = 0.08; //折现率

        double cash1 = averageFreeCash;
        double cash2 = cash1 * (1 + now2FiveGrowth);
        double cash3 = cash2 * (1 + now2FiveGrowth);
        double cash4 = cash3 * (1 + now2FiveGrowth);
        double cash5 = cash4 * (1 + now2FiveGrowth);
        double yongxu = cash5 * (1 + afterFiveGrowth) / (zhexianlv - afterFiveGrowth);

        double guzhi = cash1 / (1 + zhexianlv) + cash2 / pow((1 + zhexianlv), 2) + cash3 / pow((1 + zhexianlv), 3) + cash4 / pow((1 + zhexianlv), 4) + cash5 / pow((1 + zhexianlv), 5) + yongxu / pow((1 + zhexianlv), 6);
        double shareguzhi = (guzhi * 1000000) / vecOneAllBalance[i].stock_share; //单位为百万

        //计算平均年股息
        //double dividend = 0.0;
        //for(int i=0; i<vecOneAllSunYi.size(); ++i){
        //    dividend +=vecOneAllSunYi[i].ont_dividend;
        //}
        //dividend /= vecOneAllSunYi.size();

        //计算每股净值 begin
        double allShare = vecOneAllBalance[i].stock_share;

        if ((allShare < 0.01 && allShare > -0.01) && (i + 1 < maxCanCalcNum - 1)) {
            allShare = vecOneAllBalance[i + 1].stock_share;
        }

        double shareValue = vecOneAllBalance[i].holder_asset * 1000000 / allShare;

        if (vecOneAllBalance[i].bizhong.find("人民币") != string::npos) { //人民币，需要转换为港币
            double rate = atof(Runtime::getInstance()->configObj->find("RMB_HK_ExchangeRate").c_str());
            shareValue *= rate;
        }
        //计算每股净值 end

        //如果每股股息为空，从总股息中算出来
        if (vecOneAllSunYi[i].ont_dividend < 0.05 && vecOneAllSunYi[i].ont_dividend > -0.05) {
            vecOneAllSunYi[i].ont_dividend = ((vecOneAllSunYi[i].dividend*1000000)/vecOneAllBalance[i].stock_share)*100;//元转换为仙
        }
        Runtime::getInstance()->sqlite.setSql(insertFinanceIndexSql);

        if (Runtime::getInstance()->sqlite.prepare() < 0) {
            wxMessageBox(Runtime::getInstance()->sqlite.errString);
        }

        Runtime::getInstance()->sqlite.bindString(1,  trim(vecOneAllBalance[i].stock_id).c_str(), -1, SQLITE_STATIC);
        Runtime::getInstance()->sqlite.bindString(2,  trim(vecOneAllBalance[i].report_time).c_str(), -1, SQLITE_STATIC);
        Runtime::getInstance()->sqlite.bindString(3,  trim(vecOneAllBalance[i].report_type).c_str(), -1, SQLITE_STATIC);
        Runtime::getInstance()->sqlite.bindDouble(4,  cashdidept);
        Runtime::getInstance()->sqlite.bindDouble(5,  maolilv);
        Runtime::getInstance()->sqlite.bindDouble(6,  yingyelirunlv);
        Runtime::getInstance()->sqlite.bindDouble(7,  jinglilv);
        Runtime::getInstance()->sqlite.bindDouble(8,  roe);
        Runtime::getInstance()->sqlite.bindDouble(9,  roa);
        Runtime::getInstance()->sqlite.bindDouble(10,  yingshougrowthrate);
        Runtime::getInstance()->sqlite.bindDouble(11, yingshoulvrungrowthrate);
        Runtime::getInstance()->sqlite.bindDouble(12, zongzicangrowthrate);
        Runtime::getInstance()->sqlite.bindDouble(13, jingzicangrowthrate);
        Runtime::getInstance()->sqlite.bindDouble(14, yingshouzzl);
        Runtime::getInstance()->sqlite.bindDouble(15, cunhuozzl);
        Runtime::getInstance()->sqlite.bindDouble(16, gudingzicanzzl);
        Runtime::getInstance()->sqlite.bindDouble(17, zongzicanzzl);
        Runtime::getInstance()->sqlite.bindDouble(18, gangganlv);
        Runtime::getInstance()->sqlite.bindDouble(19, shareguzhi);
        Runtime::getInstance()->sqlite.bindDouble(20, vecOneAllSunYi[i].ont_dividend);
        Runtime::getInstance()->sqlite.bindDouble(21, shareValue);

        if (Runtime::getInstance()->sqlite.step() < 0) {
            wxMessageBox(Runtime::getInstance()->sqlite.errString + " [" + vecOneAllBalance[i].stock_id + "]");
            return;
        };

        Runtime::getInstance()->sqlite.finalize();
    }

}

void FinanceFrame::ReCreateThreeTbl(string stock_id, string idName) {
    if (m_balanceFrame != NULL) {
        delete m_balanceFrame;
        m_balanceFrame = NULL;
    }

    m_balanceFrame = new BalanceFrame(stock_id, idName, m_reportType);

    if (m_sunYiFrame != NULL) {
        delete m_sunYiFrame;
        m_sunYiFrame = NULL;
    }

    m_sunYiFrame = new SunYiFrame(stock_id, idName, m_reportType);

    if (m_cashFlowFrame != NULL) {
        delete m_cashFlowFrame;
        m_cashFlowFrame = NULL;
    }

    m_cashFlowFrame = new CashFlowFrame(stock_id, idName, m_reportType);

    m_balanceFrame->Move(wxPoint(0, 0));
    m_sunYiFrame->Move(wxPoint(300, 200));
    m_cashFlowFrame->Move(wxPoint(600, 400));
}


BEGIN_EVENT_TABLE(BalanceFrame, wxFrame)
    EVT_CLOSE(OnClose)
END_EVENT_TABLE()
BalanceFrame::BalanceFrame(string stockId, string caption, string reportType) : m_stockid(stockId) , m_reportType(reportType),
    wxFrame(NULL, wxID_ANY, "资产负债表_" + caption, wxDefaultPosition, wxSize(900, 620)) {

    balanceGrid = new wxGrid(this, ID_BALANCE_GRID, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, "资产负债表");

    vector<BalanceData> vecBalanceData;
    getOneBalance(stockId, vecBalanceData, m_reportType);
    balanceGT = new wxGridStringTable(26, vecBalanceData.size());
    balanceGT->SetRowLabelValue(0, "报告期");
    balanceGT->SetRowLabelValue(1, "非流动资产");
    balanceGT->SetRowLabelValue(2, "流动资产");
    balanceGT->SetRowLabelValue(3, "流动负债");
    balanceGT->SetRowLabelValue(4, "净流动资产/(负债)");
    balanceGT->SetRowLabelValue(5, "非流动负债");
    balanceGT->SetRowLabelValue(6, "少数股东权益");
    balanceGT->SetRowLabelValue(7, "净资产/(负债)");
    balanceGT->SetRowLabelValue(8, "已发行股本");
    balanceGT->SetRowLabelValue(9, "储备");
    balanceGT->SetRowLabelValue(10, "股东权益/(亏损)");
    balanceGT->SetRowLabelValue(11, "无形资产(非流动)");
    balanceGT->SetRowLabelValue(12, "物业厂房设备(非流动)");
    balanceGT->SetRowLabelValue(13, "附属公司权益(非流动)");
    balanceGT->SetRowLabelValue(14, "联营公司权益(非流动)");
    balanceGT->SetRowLabelValue(15, "其他投资(非流动资产)");
    balanceGT->SetRowLabelValue(16, "应收账款(流动资产)");
    balanceGT->SetRowLabelValue(17, "存货(流动资产)");
    balanceGT->SetRowLabelValue(18, "现金及银行结存(流动)");
    balanceGT->SetRowLabelValue(19, "应付帐款(流动负债)");
    balanceGT->SetRowLabelValue(20, "银行贷款(流动负债)");
    balanceGT->SetRowLabelValue(21, "非流动银行贷款");
    balanceGT->SetRowLabelValue(22, "总资产");
    balanceGT->SetRowLabelValue(23, "总负债");
    balanceGT->SetRowLabelValue(24, "股份数目");
    balanceGT->SetRowLabelValue(25, "币种");

    for (int i = 0; i < vecBalanceData.size(); ++i) {
        balanceGT->SetColLabelValue(i, vecBalanceData[i].report_type);//报表类型放上面的lable区

        balanceGT->SetValue(0, i, vecBalanceData[i].report_time);
        balanceGT->SetValue(1, i, wxString::Format("%g", vecBalanceData[i].static_asset));
        balanceGT->SetValue(2, i, wxString::Format("%g", vecBalanceData[i].flow_asset));
        balanceGT->SetValue(3, i, wxString::Format("%g", vecBalanceData[i].flow_debt));
        balanceGT->SetValue(4, i, wxString::Format("%g", vecBalanceData[i].netflow_asset));
        balanceGT->SetValue(5, i, wxString::Format("%g", vecBalanceData[i].static_debt));
        balanceGT->SetValue(6, i, wxString::Format("%g", vecBalanceData[i].fewholder_asset));
        balanceGT->SetValue(7, i, wxString::Format("%g", vecBalanceData[i].net_asset));
        balanceGT->SetValue(8, i, wxString::Format("%g", vecBalanceData[i].guben));
        balanceGT->SetValue(9, i, wxString::Format("%g", vecBalanceData[i].chubei));
        balanceGT->SetValue(10, i, wxString::Format("%g", vecBalanceData[i].holder_asset));
        balanceGT->SetValue(11, i, wxString::Format("%g", vecBalanceData[i].invisible_asset));
        balanceGT->SetValue(12, i, wxString::Format("%g", vecBalanceData[i].building_device));
        balanceGT->SetValue(13, i, wxString::Format("%g", vecBalanceData[i].childcort_asset));
        balanceGT->SetValue(14, i, wxString::Format("%g", vecBalanceData[i].sibing_asset));
        balanceGT->SetValue(15, i, wxString::Format("%g", vecBalanceData[i].other_invest));
        balanceGT->SetValue(16, i, wxString::Format("%g", vecBalanceData[i].yingshou));
        balanceGT->SetValue(17, i, wxString::Format("%g", vecBalanceData[i].cunhuo));
        balanceGT->SetValue(18, i, wxString::Format("%g", vecBalanceData[i].cash));
        balanceGT->SetValue(19, i, wxString::Format("%g", vecBalanceData[i].yingfu));
        balanceGT->SetValue(20, i, wxString::Format("%g", vecBalanceData[i].flow_bankdept));
        balanceGT->SetValue(21, i, wxString::Format("%g", vecBalanceData[i].static_bankdept));
        balanceGT->SetValue(22, i, wxString::Format("%g", vecBalanceData[i].all_asset));
        balanceGT->SetValue(23, i, wxString::Format("%g", vecBalanceData[i].all_dept));
        balanceGT->SetValue(24, i, wxString::Format("%.0f", vecBalanceData[i].stock_share));
        balanceGT->SetValue(25, i, vecBalanceData[i].bizhong);
    }

    wxFont fontCell = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL);
    wxFont fontLabel = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_BOLD);
    balanceGrid->SetDefaultCellFont(fontCell);
    balanceGrid->SetLabelFont(fontLabel);

    balanceGrid->SetTable(balanceGT, true);
    balanceGrid->AutoSizeRows();
    balanceGrid->AutoSizeColumns();
    balanceGrid->SetRowLabelSize(160);

    Centre();
    Show(true);
}
void BalanceFrame::OnClose(wxCloseEvent& event) {
    Show(false);
}


BEGIN_EVENT_TABLE(SunYiFrame, wxFrame)
    EVT_CLOSE(OnClose)
END_EVENT_TABLE()
SunYiFrame::SunYiFrame(string stockId, string caption, string reportType) : m_stockid(stockId), m_reportType(reportType), wxFrame(NULL, wxID_ANY, "综合损益表_" + caption, wxDefaultPosition, wxSize(900, 510)) {

    wxGrid* sunYiGrid = new wxGrid(this, ID_SUNYI_GRID, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, "损益表");
    vector<SunYiData> vecSunYiData;
    getOneSunYi(stockId, vecSunYiData, m_reportType);
    wxGridStringTable* SunYiGT = new wxGridStringTable(21, vecSunYiData.size());
    SunYiGT->SetRowLabelValue(0, "报告期");
    SunYiGT->SetRowLabelValue(1, "营业额");
    SunYiGT->SetRowLabelValue(2, "除税前盈利/(亏损)");
    SunYiGT->SetRowLabelValue(3, "税项");
    SunYiGT->SetRowLabelValue(4, "除税后盈利/(亏损)");
    SunYiGT->SetRowLabelValue(5, "少数股东权益");
    SunYiGT->SetRowLabelValue(6, "股东应占盈利/(亏损)");
    SunYiGT->SetRowLabelValue(7, "股息");
    SunYiGT->SetRowLabelValue(8, "除税及股息后盈利/(亏损)");
    SunYiGT->SetRowLabelValue(9, "基本每股盈利(仙)");
    SunYiGT->SetRowLabelValue(10, "摊薄每股盈利(仙)");
    SunYiGT->SetRowLabelValue(11, "每股股息(仙)");
    SunYiGT->SetRowLabelValue(12, "销售成本");
    SunYiGT->SetRowLabelValue(13, "折旧");
    SunYiGT->SetRowLabelValue(14, "销售及分销费用");
    SunYiGT->SetRowLabelValue(15, "一般及行政费用");
    SunYiGT->SetRowLabelValue(16, "财务费用");
    SunYiGT->SetRowLabelValue(17, "毛利");
    SunYiGT->SetRowLabelValue(18, "经营盈利");
    SunYiGT->SetRowLabelValue(19, "应占联营公司盈利");
    SunYiGT->SetRowLabelValue(20, "币种");

    for (int i = 0; i < vecSunYiData.size(); ++i) {
        SunYiGT->SetColLabelValue(i, vecSunYiData[i].report_type);//报表类型放上面的lable区

        SunYiGT->SetValue(0, i, vecSunYiData[i].report_time);
        SunYiGT->SetValue(1, i, wxString::Format("%g", vecSunYiData[i].yingyee));
        SunYiGT->SetValue(2, i, wxString::Format("%g", vecSunYiData[i].untax_profit));
        SunYiGT->SetValue(3, i, wxString::Format("%g", vecSunYiData[i].tax));
        SunYiGT->SetValue(4, i, wxString::Format("%g", vecSunYiData[i].tax_profit));
        SunYiGT->SetValue(5, i, wxString::Format("%g", vecSunYiData[i].fewholder_profit));
        SunYiGT->SetValue(6, i, wxString::Format("%g", vecSunYiData[i].holder_profit));
        SunYiGT->SetValue(7, i, wxString::Format("%g", vecSunYiData[i].dividend));
        SunYiGT->SetValue(8, i, wxString::Format("%g", vecSunYiData[i].net_profit));
        SunYiGT->SetValue(9, i, wxString::Format("%g", vecSunYiData[i].basic_one_profit));
        SunYiGT->SetValue(10, i, wxString::Format("%g", vecSunYiData[i].tanxiao_one_profit));
        SunYiGT->SetValue(11, i, wxString::Format("%g", vecSunYiData[i].ont_dividend));
        SunYiGT->SetValue(12, i, wxString::Format("%g", vecSunYiData[i].cost));
        SunYiGT->SetValue(13, i, wxString::Format("%g", vecSunYiData[i].depreciation));
        SunYiGT->SetValue(14, i, wxString::Format("%g", vecSunYiData[i].sell_fee));
        SunYiGT->SetValue(15, i, wxString::Format("%g", vecSunYiData[i].admin_fee));
        SunYiGT->SetValue(16, i, wxString::Format("%g", vecSunYiData[i].lixi_fee));
        SunYiGT->SetValue(17, i, wxString::Format("%g", vecSunYiData[i].gross_profit));
        SunYiGT->SetValue(18, i, wxString::Format("%g", vecSunYiData[i].jingying_profit));
        SunYiGT->SetValue(19, i, wxString::Format("%g", vecSunYiData[i].othercorp_profit));
        SunYiGT->SetValue(20, i, vecSunYiData[i].bizhong);
    }

    sunYiGrid->SetTable(SunYiGT, true);

    wxFont fontCell = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL);
    wxFont fontLabel = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_BOLD);
    sunYiGrid->SetDefaultCellFont(fontCell);
    sunYiGrid->SetLabelFont(fontLabel);

    sunYiGrid->AutoSizeRows();
    sunYiGrid->AutoSizeColumns();
    sunYiGrid->SetRowLabelSize(160);
    Centre();
    Show(true);
}
void SunYiFrame::OnClose(wxCloseEvent& event) {
    Show(false);
}

BEGIN_EVENT_TABLE(CashFlowFrame, wxFrame)
    EVT_CLOSE(OnClose)
END_EVENT_TABLE()
CashFlowFrame::CashFlowFrame(string stockId, string caption, string reportType) : m_stockid(stockId), m_reportType(reportType),
    wxFrame(NULL, wxID_ANY, "现金流量表_" + caption, wxDefaultPosition, wxSize(900, 320)) {

    //cash flow
    vector<CashFlowData> vecCashFlowData;
    getOneCashFlow(stockId, vecCashFlowData, m_reportType);
    wxGrid* cashFlowGrid = new wxGrid(this, ID_CASHFLOW_GRID, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, "现金流量表");
    wxGridStringTable* cashFlowGT = new wxGridStringTable(10, vecCashFlowData.size());
    cashFlowGT->SetRowLabelValue(0, "报告期");
    cashFlowGT->SetRowLabelValue(1, "经营业务所得之现金流入净额");
    cashFlowGT->SetRowLabelValue(2, "投资活动之现金流入净额");
    cashFlowGT->SetRowLabelValue(3, "融资活动之现金流入净额");
    cashFlowGT->SetRowLabelValue(4, "现金及现金等价物增加");
    cashFlowGT->SetRowLabelValue(5, "会计年初之现金及现金等价物");
    cashFlowGT->SetRowLabelValue(6, "会计年终之现金及现金等价物");
    cashFlowGT->SetRowLabelValue(7, "外汇兑换率变动之影响");
    cashFlowGT->SetRowLabelValue(8, "购置固定资产款项");
    cashFlowGT->SetRowLabelValue(9, "币种");

    for (int i = 0; i < vecCashFlowData.size(); ++i) {
        cashFlowGT->SetColLabelValue(i, vecCashFlowData[i].report_type);//报表类型放上面的lable区

        cashFlowGT->SetValue(0, i, vecCashFlowData[i].report_time);
        cashFlowGT->SetValue(1, i, wxString::Format("%g", vecCashFlowData[i].jingying_net));
        cashFlowGT->SetValue(2, i, wxString::Format("%g", vecCashFlowData[i].invest_net));
        cashFlowGT->SetValue(3, i, wxString::Format("%g", vecCashFlowData[i].rongzi_net));
        cashFlowGT->SetValue(4, i, wxString::Format("%g", vecCashFlowData[i].cash_add));
        cashFlowGT->SetValue(5, i, wxString::Format("%g", vecCashFlowData[i].cash_begin));
        cashFlowGT->SetValue(6, i, wxString::Format("%g", vecCashFlowData[i].cash_end));
        cashFlowGT->SetValue(7, i, wxString::Format("%g", vecCashFlowData[i].waihui_profit));
        cashFlowGT->SetValue(8, i, wxString::Format("%g", vecCashFlowData[i].buy_device));
        cashFlowGT->SetValue(9, i, vecCashFlowData[i].bizhong);
    }

    cashFlowGrid->SetTable(cashFlowGT, true);

    wxFont fontCell = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_NORMAL);
    wxFont fontLabel = wxFont(10, wxFONTFAMILY_DEFAULT, wxNORMAL, wxFONTWEIGHT_BOLD);
    cashFlowGrid->SetDefaultCellFont(fontCell);
    cashFlowGrid->SetLabelFont(fontLabel);

    cashFlowGrid->AutoSizeRows();
    cashFlowGrid->AutoSizeColumns();
    cashFlowGrid->SetRowLabelSize(200);

    Centre();
    Show(true);
}
void CashFlowFrame::OnClose(wxCloseEvent& event) {
    Show(false);
}