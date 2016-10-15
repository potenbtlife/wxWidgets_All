#include "ValueReportDialog.h"

#include <wx/xy/xyplot.h>
#include <wx/xy/xylinerenderer.h>
#include <wx/dataset.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/dateaxis.h>
#include <wx/chart.h>
#include <wx/aui/aui.h>

#include <wx/xy/xysimpledataset.h>


struct MYDATA{
    int recored_time;
    double value;
    double index;
};
int getReportDataFromDb(vector<MYDATA>& data);

/**
 * Time series demo dataset.
 */
/*class TimeSeriesDataset : public XYDataset, public DateTimeDataset
{
	DECLARE_CLASS(TimeSeriesDataset)
public:
	TimeSeriesDataset(vector<MYDATA>& data, int type);//type: 0：value；2：index
	virtual ~TimeSeriesDataset();

	virtual size_t GetSerieCount();

	virtual wxString GetSerieName(size_t serie);

	virtual size_t GetCount(size_t serie);

	virtual double GetX(size_t index, size_t serie);

	virtual double GetY(size_t index, size_t serie);

	virtual DateTimeDataset *AsDateTimeDataset();

	//
	// DateTimeDataset
	//
	virtual time_t GetDate(size_t index);

	virtual size_t GetCount();


private:
	vector<double> m_data;
	vector<time_t> m_times;
};

IMPLEMENT_CLASS(TimeSeriesDataset, XYDataset)

TimeSeriesDataset::TimeSeriesDataset(vector<MYDATA>& data, int type)
{
    wxDateTime dt;
    for (int i=0; i<data.size(); i++){
        dt.ParseFormat(data[i].recored_time.c_str(), wxT("%Y-%m-%d"));
        
        if(type == 0 ) {
            m_data.push_back(data[i].value);
        }else{
            m_data.push_back(data[i].index);
        }
        m_times.push_back(dt.GetTicks());
    }
    
}

TimeSeriesDataset::AddSerie(){
    
    double* dataArray = new double[m_data.size()];

    ::AddSerie((double *) values2, WXSIZEOF(values2));
}

TimeSeriesDataset::~TimeSeriesDataset()
{
}

size_t TimeSeriesDataset::GetSerieCount()
{
	return 1;
}

size_t TimeSeriesDataset::GetCount(size_t WXUNUSED(serie))
{
	return m_data.size();
}

size_t TimeSeriesDataset::GetCount()
{
	return m_data.size();
}

wxString TimeSeriesDataset::GetSerieName(size_t WXUNUSED(serie))
{
	return wxT("Time series");
}

double TimeSeriesDataset::GetX(size_t index, size_t WXUNUSED(serie))
{
	return index;
}

double TimeSeriesDataset::GetY(size_t index, size_t WXUNUSED(serie))
{
	return m_data[index];
}

time_t TimeSeriesDataset::GetDate(size_t index)
{
	return m_times[index];
}

DateTimeDataset *TimeSeriesDataset::AsDateTimeDataset()
{
    return this;
};*/


/**
 * Base class for chart demo.
 *
 */
class ChartDemo
{
public:
	ChartDemo(const wxChar *name)
	{
		m_name = name;
	}

	virtual ~ChartDemo()
	{
	}

	/**
	 * Called to create chart.
	 * @return created demo chart
	 */
	virtual Chart *Create() = 0;

	const wxString &GetName()
	{
		return m_name;
	}

private:
	wxString m_name;
};

/**
 *
 */
/*class TimeSeriesDemo1 : public ChartDemo
{
public:
	TimeSeriesDemo1()
	: ChartDemo(wxT("Time series Demo 1"))
	{
	}

    int getReportDataFromDb(vector<MYDATA>& data);//从数据库中获取绘图数据

	virtual Chart *Create()
	{
		// data
        vector<MYDATA> vecData;
        getReportDataFromDb(vecData);

		// first step: create plot
		XYPlot *plot = new XYPlot();

		TimeSeriesDataset *dataset_value = new TimeSeriesDataset(vecData,0);//净值数据源
        TimeSeriesDataset *dataset_index = new TimeSeriesDataset(vecData,1);//指数数据源
        
        dataset_value->AddS
        // colors for first and second datasets
        wxColour color1 = wxColour(255, 0, 0);
        wxColour color2 = wxColour(0, 0, 255);

        // create renderer for first dataset
        XYLineRenderer *renderer_value = new XYLineRenderer();
        renderer_value->SetSerieColour(0, &color1);
        dataset_value->SetRenderer(renderer_value);

        plot->AddDataset(dataset_value);

        // create renderer for second dataset
        XYLineRenderer *renderer_index = new XYLineRenderer();
        renderer_index->SetSerieColour(0, &color2);
        dataset_index->SetRenderer(renderer_index);

        plot->AddDataset(dataset_index);



		// add left axes for first dataset
		NumberAxis *leftAxis_value = new NumberAxis(AXIS_LEFT);
        leftAxis_value->SetLabelTextColour(color1);
		leftAxis_value->SetLabelPen(*wxThePenList->FindOrCreatePen(color1, 1, wxSOLID));
        plot->AddAxis(leftAxis_value);

        // create left axis for second dataset
        NumberAxis *leftAxis_index = new NumberAxis(AXIS_LEFT);
        // set label text colour same as lines
        leftAxis_index->SetLabelTextColour(color2);
        // set label lines colour same as lines
        leftAxis_index->SetLabelPen(*wxThePenList->FindOrCreatePen(color2, 1, wxSOLID));
        plot->AddAxis(leftAxis_index);

        //add bottom date axes
		DateAxis *bottomAxis = new DateAxis(AXIS_BOTTOM);

		// setup window
		//bottomAxis->SetWindow(0, 10);
		//bottomAxis->SetUseWindow(true);

		bottomAxis->SetVerticalLabelText(true);
		bottomAxis->SetDateFormat(wxT("%m-%d"));

		plot->AddAxis(bottomAxis);


        // link first dataset with first left axis
        plot->LinkDataVerticalAxis(0, 0);
        // link second dataset with second left axis
        plot->LinkDataVerticalAxis(0, 0);

        // link first and second datasets with bottom axis
        plot->LinkDataHorizontalAxis(0, 0);
        plot->LinkDataHorizontalAxis(0, 0);
		
        // link axes and dataset
        //plot->LinkDataVerticalAxis(0, 0);
		//plot->LinkDataHorizontalAxis(0, 0);

		// and finally create chart
		Chart *chart = new Chart(plot, GetName());

		//chart->SetScrolledAxis(bottomAxis);
		return chart;
	}
};

ChartDemo *timeSeriesDemos[] = {
	new TimeSeriesDemo1(),
};
int timeSeriesDemosCount = WXSIZEOF(timeSeriesDemos);*/


/**
 * Multiple axis demo.
 */
class MultipleAxisDemo1 : public ChartDemo
{
public:
	MultipleAxisDemo1()
	:ChartDemo(wxT("Multiple axis Demo 1"))
	{
	}
    
    double m_value1[36][2];
    double m_value2[36][2];
    void DataVector2Array(vector<MYDATA>& vecData){
        /*if (m_value1) {
            delete m_value1;
            m_value1 = NULL;
        }
        if (m_value2) {
            delete m_value2;
            m_value2 = NULL;
        }*/
        int testnum = vecData.size();
        //m_value1 = new double*[vecData.size()];
        //m_value2 = new double*[vecData.size()];

        for (int i=0; i<vecData.size(); ++i) {
            //m_value1[i] = new double[2];
            m_value1[i][0] = vecData[i].recored_time;
            m_value1[i][1] = vecData[i].value;

            //m_value2[i] = new double[2];
            m_value2[i][0] = vecData[i].recored_time;
            m_value2[i][1] = vecData[i].index;
        }
        
    };

	virtual Chart *Create()
	{
		// first dataset values
		/*double m_value1[][2] = {
				{ 1, 1 },
				{ 2, 3 },
				{ 5, 4 },
				{ 6, 3 },
				{ 7, 6 },
				{ 8, 6 },
				{ 9, 4 },
		};

		// second dataset values
		double m_value2[][2] = {
				{ 0, 0 },
				{ 2, -1 },
				{ 4, 6 },
				{ 5, 2 },
				{ 7, 8 },
				{ 8, 4 },
				{ 9, -2 },
		};*/

        vector<MYDATA> vecData;
        getReportDataFromDb(vecData);
        DataVector2Array(vecData);
		// colors for first and second datasets
		wxColour color1 = wxColour(255, 0, 0);
		wxColour color2 = wxColour(0, 0, 255);

		// create xy plot
		XYPlot *plot = new XYPlot();

		// create first dataset
		XYSimpleDataset *dataset1 = new XYSimpleDataset();
		// add serie to it
		dataset1->AddSerie((double *) m_value1, WXSIZEOF(m_value1));

		// create second dataset
		XYSimpleDataset *dataset2 = new XYSimpleDataset();
		// add serie to it
		dataset2->AddSerie((double *) m_value2, WXSIZEOF(m_value2));

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
		NumberAxis *bottomAxis = new NumberAxis(AXIS_BOTTOM);
		plot->AddAxis(bottomAxis);

		// link first dataset with first left axis
		plot->LinkDataVerticalAxis(0, 0);
		// link second dataset with second left axis
		plot->LinkDataVerticalAxis(1, 1);

		// link first and second datasets with bottom axis
		plot->LinkDataHorizontalAxis(0, 0);
		plot->LinkDataHorizontalAxis(1, 0);

		return new Chart(plot, GetName());
	}
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
MainFrame::MainFrame() : 
wxFrame(NULL, wxID_ANY, wxString::Format(wxT("wxFreeChart demo %s"), version), wxDefaultPosition, wxSize(800, 445))
{
    wxAuiManager *auiMan = new wxAuiManager(this);

    m_chartPanel = new wxChartPanel(this);
    auiMan->AddPane(m_chartPanel,
        wxAuiPaneInfo().Center().BestSize(400, 400).CloseButton(false).Caption(wxT("chart")));

    //m_chartPanel->SetChart(CreateChart());
	auiMan->Update();
    Centre();

    ChartDemo *demo = new MultipleAxisDemo1();
    m_chartPanel->SetChart(demo->Create());


    /*wxStaticText *sellNameText = new wxStaticText(this, -1, "近一天:",wxPoint(20, 10),wxSize(60, 20));
    sellNameCtrl = new wxTextCtrl(this,ID_SELLNANME_CTRL, "",wxPoint(90, 10),wxSize(80, 20));
    wxStaticText *sellCodeText = new wxStaticText(this, -1, "近一周:",wxPoint(200, 10),wxSize(30, 20));
    sellCodeCtrl = new wxTextCtrl(this, ID_SELLCODE_CTRL, "000",wxPoint(240, 10),wxSize(80, 20));

    wxStaticText *sellPriceText = new wxStaticText(this, -1, "当月:",wxPoint(20, 35),wxSize(60, 20));
    sellPriceCtrl = new wxTextCtrl(this,ID_SELLPRICE_CTRL, "0.0",wxPoint(90, 35),wxSize(80, 20));
    wxStaticText *sellNumText = new wxStaticText(this, -1, "当年",wxPoint(200, 35),wxSize(30, 20));
    sellNumCtrl = new wxTextCtrl(this, ID_SELLNUM_CTRL, "100",wxPoint(240, 35),wxSize(80, 20));*/

    //Show();
}

int getReportDataFromDb(vector<MYDATA>& data){
    Runtime::getInstance()->sqlite.setSql(QryReportDataSql);

    if( Runtime::getInstance()->sqlite.prepare() < 0 ){
        wxMessageBox(Runtime::getInstance()->sqlite.errString);
        return -1;
    }

    Runtime::getInstance()->sqlite.bindInt(1, Runtime::getInstance()->CurComposeID);

    MYDATA tmpData;
    while ( 1 == Runtime::getInstance()->sqlite.step() )
    {
        string strTime = Runtime::getInstance()->sqlite.getColumnString(0);
        wxDateTime dt;
        dt.ParseFormat(strTime.c_str(), wxT("%Y-%m-%d"));
        string test = dt.Format("%m%d");
        
        tmpData.recored_time = atoi(test.c_str());
        tmpData.value = Runtime::getInstance()->sqlite.getColumnDouble(1);
        tmpData.index = Runtime::getInstance()->sqlite.getColumnDouble(2);

        data.push_back(tmpData);
    }

    return 1;
}
