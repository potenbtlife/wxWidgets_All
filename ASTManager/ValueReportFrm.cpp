#include "ValueReportFrm.h"

#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/dataset.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/dateaxis.h>
#include <wx/chart.h>
#include <wx/xy/xysimpledataset.h>

#include <algorithm>

/**
 * Holds data for one XY serie.
 */
class WXDLLIMPEXP_FREECHART MYXYSerie // : public XYSerie
{
public:
	/**
	 * Constructs new xy serie.
	 * @param data double [x, y] array
	 * @param count point count in data array
	 */
    MYXYSerie(vector<double>& data, vector<time_t>& times ,std::string name);

	virtual ~MYXYSerie();

	double GetX(size_t index);

	double GetY(size_t index);

    time_t GetDate(size_t index);

	size_t GetCount();

	const wxString &GetName();

	void SetName(const wxString &name);

private:
	//double *m_data;
    vector<double> m_data;
    vector<time_t> m_times;
	wxString m_name;
};

MYXYSerie::MYXYSerie(vector<double>& data, vector<time_t>& times, std::string name){
    m_data = data;
    m_times = times;
    m_name = name;
}

MYXYSerie::~MYXYSerie()
{
    //wxDELETEA(m_data);
}

double MYXYSerie::GetX(size_t index)
{
    wxCHECK_MSG(index < m_data.size(), 0, wxT("MYXYSerie::GetX"));
    return m_times[index];
}

double MYXYSerie::GetY(size_t index)
{
    wxCHECK_MSG(index < m_data.size(), 0, wxT("MYXYSerie::GetY"));
    return m_data[index];
}

time_t MYXYSerie::GetDate(size_t index){
    return m_times[index];
}

size_t MYXYSerie::GetCount()
{
    return m_data.size();
}

const wxString &MYXYSerie::GetName()
{
    return m_name;
}

void MYXYSerie::SetName(const wxString &name)
{
    m_name = name;
}


//////////////////////////////////////////////////////////////////
/**
 * Time series demo dataset.
 */
class TimeSeriesDataset : public XYDataset, public DateTimeDataset
{
	DECLARE_CLASS(TimeSeriesDataset)
public:
	TimeSeriesDataset(vector<MYDATA>& data, int type, int cycle);//type: 0：value；1：index
    virtual ~TimeSeriesDataset();

    /**
	 * Adds new xy serie.
	 * @param [data] y array
	 * @param [times] x array
	 */
	void AddSerie(vector<double>& data, vector<time_t>& times, string& name);

	/**
	 * Constructs new xy serie.
	 * @param serie new serie
	 */
	void AddSerie(MYXYSerie *serie);

	virtual size_t GetSerieCount();

	virtual wxString GetSerieName(size_t serie);

	virtual size_t GetCount(size_t serie);

	virtual double GetX(size_t index, size_t serie);

	virtual double GetY(size_t index, size_t serie);

	virtual DateTimeDataset *AsDateTimeDataset();

	// DateTimeDataset inherit function
	virtual time_t GetDate(size_t index);

	virtual size_t GetCount();

private:
    vector<MYXYSerie> m_series;
};

IMPLEMENT_CLASS(TimeSeriesDataset, XYDataset)

void TimeSeriesDataset::AddSerie(vector<double>& data, vector<time_t>& times, string& name)
{
    AddSerie(new MYXYSerie(data, times, name));
}

void TimeSeriesDataset::AddSerie(MYXYSerie *serie)
{
    m_series.push_back(*serie);
    DatasetChanged();
}

//从原始数据，拆分为两组xy坐标数据，
//param[in]type（数据类型）：0 净值；1：指数
//param[in]cycle（周期类型）：0：展示所有， 1:一月内，2：一年内
TimeSeriesDataset::TimeSeriesDataset(vector<MYDATA>& data, int type, int cycle = 0)
{
    wxDateTime dt;
    vector<double> vecData;
    vector<time_t> vecTimes;
    std::string name;

    //从原始数据，拆分为两组xy坐标数据
    for (int i = data.size()-1; i>=0; --i){

        if( cycle == 1 && data.size() > (i+1) ){
            if(data[i].recored_time.GetMonth() != data[i+1].recored_time.GetMonth()){
                break;
            }
        }else if ( cycle == 2 && data.size() > (i+1) ) {
            if(data[i].recored_time.GetYear() != data[i+1].recored_time.GetYear()){
                break;
            }
        }

        if(type == 0){
            name = "value";
            vecData.push_back(data[i].value);
        }else {
            vecData.push_back(data[i].index);
            name = "index";
        }
        
        vecTimes.push_back(data[i].recored_time.GetTicks());
    }
    
    std::reverse(vecData.begin(), vecData.end());
    std::reverse(vecTimes.begin(), vecTimes.end());
    AddSerie(vecData, vecTimes, name);
}

TimeSeriesDataset::~TimeSeriesDataset() {
}

size_t TimeSeriesDataset::GetSerieCount() {
	return m_series.size();
}

size_t TimeSeriesDataset::GetCount(size_t serie) {
	return m_series[serie].GetCount();
}

size_t TimeSeriesDataset::GetCount() {
	return m_series[0].GetCount();
}

wxString TimeSeriesDataset::GetSerieName(size_t serie) {
	return wxT("Time series");
}

double TimeSeriesDataset::GetX(size_t index, size_t serie) {
	return index;
}

double TimeSeriesDataset::GetY(size_t index, size_t serie) {
	return m_series[serie].GetY(index);
}

time_t TimeSeriesDataset::GetDate(size_t index) {

    if (m_series[0].GetCount() <= 30) {
        return m_series[0].GetDate(index);
    }

    if (index == 0 || index == (m_series[0].GetCount() -1)) { //恒显示第一个和最后一个
        return m_series[0].GetDate(index);
    }
    
    float divisor = (float)m_series[0].GetCount()/30; //显示30个日期
    int circle = (int)(index-1)/divisor;

    if ( (index - circle*divisor) / divisor > 1 ) {
        return m_series[0].GetDate(index);

    }else{
        return NULL;
    }
    
}

DateTimeDataset *TimeSeriesDataset::AsDateTimeDataset() {
    return this;
};
    
static wxChar *version = wxT("1.6");
/*ValueReportDialog::ValueReportDialog(wxWindow *parent,
                       wxWindowID id, 
                       const wxString& title,
                       const wxPoint&  pos, 
                       const wxSize& size,
                       long  style,
                       const wxString& name)
                       :wxDialog(parent,id,title,pos,size,style,name)
{*/

BEGIN_EVENT_TABLE(ValueReportFrm, wxFrame)    
    EVT_BUTTON(ID_SHOWMONTH_BUTTON, OnShowMonth)
    EVT_BUTTON(ID_SHOWYEAR_BUTTON, OnShowYear)
    EVT_BUTTON(ID_SHOWALL_BUTTON, OnShowAll)
END_EVENT_TABLE()

ValueReportFrm::~ValueReportFrm(){
    m_auiMan->UnInit();
};

ValueReportFrm::ValueReportFrm() : 
wxFrame(NULL, wxID_ANY, wxString::Format(wxT("report: VALUE VS INDEX, %s"), version), wxDefaultPosition, wxSize(900, 550))
{
    m_auiMan = new wxAuiManager(this);

    m_chartPanel = new wxChartPanel(this, wxID_ANY, NULL, wxDefaultPosition, wxSize(1, 1));

    getReportDataFromDb(m_vecData);
    m_chartPanel->SetChart(CreateChart(2)); //默认仅显示今年的走势图
    //m_auiMan->AddPane(m_chartPanel, wxAuiPaneInfo().Center().BestSize(400, 400).CloseButton(false).Caption(wxT("report: VALUE VS INDEX")));
    m_auiMan->AddPane(m_chartPanel, wxAuiPaneInfo().Center().BestSize(600, 400).CloseButton(false).CaptionVisible(false));

 //   wxPanel* valuePanel = new wxPanel(this, ID_DATA_PANEL);
 //   wxStaticText *valueText0 = new wxStaticText(valuePanel, wxID_ANY, "展示区间：", wxPoint(0,0),wxSize(60,20));

    char showAllValueStr[256] = {0}, showAllIndexStr[256] = {0};
    sprintf(showAllValueStr, "净值变动  近一天:%s  近一月:%s  近一年:%s", getLastDayChange(0).c_str(), getRecMonthChange(0).c_str(),getRecYearChange(0).c_str());
    sprintf(showAllIndexStr, "指数变动  近一天:%s  近一月:%s  近一年:%s", getLastDayChange(1).c_str(), getRecMonthChange(1).c_str(),getRecYearChange(1).c_str());
    
    CreateStatusBar(2);
    SetStatusText(showAllValueStr, 0);
    SetStatusText(showAllIndexStr, 1);

    wxPanel* buttonPanel = new wxPanel(this, wxID_ANY);

    wxStaticText *recShowText = new wxStaticText(buttonPanel, wxID_ANY, "展示区间：");
    wxButton* receMonButtom = new wxButton(buttonPanel, ID_SHOWMONTH_BUTTON, "近一月");
    wxButton* receYearButtom = new wxButton(buttonPanel, ID_SHOWYEAR_BUTTON, "近一年");
    wxButton* receAllButtom = new wxButton(buttonPanel, ID_SHOWALL_BUTTON, "所有");

    wxBoxSizer *sizer1 =new wxBoxSizer(wxHORIZONTAL);
    //sizer1->SetMinSize(wxSize(600,30));
    sizer1->Add(recShowText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER);
    sizer1->AddSpacer(8);
    sizer1->Add(receMonButtom, 0, wxALIGN_CENTER);
    sizer1->AddSpacer(2);
    sizer1->Add(receYearButtom, 0, wxALIGN_CENTER);
    sizer1->AddSpacer(2);
    sizer1->Add(receAllButtom, 0, wxALIGN_CENTER);
    buttonPanel->SetSizerAndFit(sizer1);
    
    m_auiMan->AddPane(buttonPanel, wxAuiPaneInfo().Bottom().CloseButton(false).CaptionVisible(false));

    m_auiMan->Update();
    Centre();

    //Show();
}

//param[in]cycle（周期类型）：0：展示所有， 1:一月内，2：一年内
Chart* ValueReportFrm::CreateChart(int cycle) {
    //DataVector2Array(vecData);
    // colors for first and second datasets
    wxColour color1 = wxColour(255, 0, 0);
    wxColour color2 = wxColour(0, 0, 255);

    // create xy plot
    XYPlot *plot = new XYPlot();

    // create first and second dataset
    TimeSeriesDataset* dataset1= new TimeSeriesDataset(m_vecData, 0, cycle);
    TimeSeriesDataset* dataset2 = new TimeSeriesDataset(m_vecData, 1, cycle);
    
    // create renderer for first dataset
    XYLineRenderer *renderer1 = new XYLineRenderer();
    renderer1->SetSerieColour(0, &color1);

    // add first dataset to plot
    plot->AddDataset(dataset1);

    // set it to first dataset
    dataset1->SetRenderer(renderer1);

    // create renderer for second dataset
    XYLineRenderer *renderer2 = new XYLineRenderer();
    renderer2->SetSerieColour(0, &color2);

    // set it to second dataset
    dataset2->SetRenderer(renderer2);

    // add second dataset to plot
    plot->AddDataset(dataset2);

    // create left axis for first dataset
    NumberAxis *leftAxis1 = new NumberAxis(AXIS_LEFT);
    // set label text colour same as lines
    leftAxis1->SetLabelTextColour(color1);
    // set label lines colour same as lines
    leftAxis1->SetLabelPen(*wxThePenList->FindOrCreatePen(color1, 1, wxSOLID));
    plot->AddAxis(leftAxis1);

    // create left axis for second dataset
    NumberAxis *leftAxis2 = new NumberAxis(AXIS_LEFT);
    // set label text colour same as lines
    leftAxis2->SetLabelTextColour(color2);
    // set label lines colour same as lines
    leftAxis2->SetLabelPen(*wxThePenList->FindOrCreatePen(color2, 1, wxSOLID));
    plot->AddAxis(leftAxis2);

    // create bottom axis for first and second dataset
    DateAxis *bottomAxis = new DateAxis(AXIS_BOTTOM);
    bottomAxis->SetVerticalLabelText(true);
    bottomAxis->SetDateFormat(wxT("%y-%m-%d"));
    plot->AddAxis(bottomAxis);

    // link first dataset with first left axis
    plot->LinkDataVerticalAxis(0, 0);
    // link second dataset with second left axis
    plot->LinkDataVerticalAxis(1, 1);

    // link first and second datasets with bottom axis
    plot->LinkDataHorizontalAxis(0, 0);
    plot->LinkDataHorizontalAxis(1, 0);

    //return new Chart(plot, "report: Value VS Index");
    return new Chart(plot);
};



int ValueReportFrm::getReportDataFromDb(vector<MYDATA>& data){
    Runtime::getInstance()->sqlite.setSql(QryReportDataSql);

    if( Runtime::getInstance()->sqlite.prepare() < 0 ){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindInt(1, Runtime::getInstance()->CurComposeID);

    MYDATA tmpData;
    while ( 1 == Runtime::getInstance()->sqlite.step() )
    {
        tmpData.recored_time.ParseFormat(Runtime::getInstance()->sqlite.getColumnString(0).c_str(), wxT("%Y-%m-%d"));
        tmpData.value = Runtime::getInstance()->sqlite.getColumnDouble(1);
        tmpData.index = Runtime::getInstance()->sqlite.getColumnDouble(2);

        data.push_back(tmpData);
    }

    return 1;
}

//get change of recent one day
//param[in] type 0: value change; 1: index change
std::string ValueReportFrm::getLastDayChange(int type){

    if(m_vecData.size()==0){
        return "";
    }

    double result = 0;
    if (type == 0 && m_vecData.size() >= 2) {
        double last = m_vecData[m_vecData.size()-1].value;
        double lastlast = m_vecData[m_vecData.size()-2].value;
        result = (last - lastlast)/lastlast;

    } else if(m_vecData.size() >= 2) {
        double last = m_vecData[m_vecData.size()-1].index;
        double lastlast = m_vecData[m_vecData.size()-2].index;
        result = (last - lastlast)/lastlast;
    }

    char outStr[32] = {0};
    sprintf(outStr,"%.2f%%", result * 100);
    return outStr;
} 


//get change of recent one month
//param[in] type 0: value change; 1: index change
std::string ValueReportFrm::getRecMonthChange(int type){
    
    if(m_vecData.size()==0){
        return "";
    }

    size_t monthFristDay = 0;
    for (int i=m_vecData.size()-1; i>0 ; --i) {
        if (m_vecData[i].recored_time.GetMonth() !=  m_vecData[i-1].recored_time.GetMonth()) {
            monthFristDay = i - 1; //起始点为上个月最后一天
            break;
        }
    }

    double result = 0;
    if (type == 0) {
        double recentDay = m_vecData[m_vecData.size()-1].value;
        double lastlast = m_vecData[monthFristDay].value;
        result = (recentDay - lastlast)/lastlast;

    } else {
        double last = m_vecData[m_vecData.size()-1].index;
        double lastlast = m_vecData[monthFristDay].index;
        result = (last - lastlast)/lastlast;
    }

    char outStr[32] = {0};
    sprintf(outStr,"%.2f%%", result * 100);
    return outStr;
} 


//get change of recent one year
//param[in] type 0: value change; 1: index change
std::string ValueReportFrm::getRecYearChange(int type){

    if(m_vecData.size()==0){
        return "";
    }

    size_t yearFristDay = 0;
    for (int i=m_vecData.size()-1; i>0 ; --i) {
        if (m_vecData[i].recored_time.GetYear() !=  m_vecData[i-1].recored_time.GetYear()) {
            yearFristDay = i - 1; //起始点为上一年的最后一天
            break;
        }
    }

    double result = 0;
    if (type == 0) {
        double recentDay = m_vecData[m_vecData.size()-1].value;
        double lastlast = m_vecData[yearFristDay].value;
        result = (recentDay - lastlast)/lastlast;

    } else {
        double last = m_vecData[m_vecData.size()-1].index;
        double lastlast = m_vecData[yearFristDay].index;
        result = (last - lastlast)/lastlast;
    }

    char outStr[32] = {0};
    sprintf(outStr,"%.2f%%", result * 100);
    return outStr;
} 

void ValueReportFrm::OnShowMonth(wxCommandEvent& event){
    m_chartPanel->SetChart(CreateChart(1));
}

void ValueReportFrm::OnShowYear(wxCommandEvent& event){
    m_chartPanel->SetChart(CreateChart(2));
}

void ValueReportFrm::OnShowAll(wxCommandEvent& event){
    m_chartPanel->SetChart(CreateChart(0));
}
