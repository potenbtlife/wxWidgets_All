#ifndef AST_FINDSHOTFRAME_H
#define AST_FINDSHOTFRAME_H
#include "common.h"
#include <wx/aui/aui.h>

struct GuZhiData{
    string stockId;
    string stockName;
    double price; //现价
    double standPrice; //自由现金流估价
    double diGuRate; //低估率
};

class FindShotFrame : public wxFrame{
public:
    FindShotFrame();
    virtual ~FindShotFrame();

private:
    void OnOk(wxCommandEvent& event);
    void GetGuZhiData(vector<GuZhiData> &vecGuZhiData, double diguRate, string year);
    void RefreshGuZhiGrid(vector<GuZhiData> &vecGuZhiData);

    wxAuiManager* m_auiMan;
    wxComboBox* m_diGuLvComboBox;
    wxButton* m_okButton;
    wxTextCtrl* yearTextCtrl;
    wxGrid* DataGrid;
    wxGridStringTable *DataGT;

    enum FINDASHOT_ID{
        ID_DIGULV_COMBOBOX,
        ID_OK_BUTTON,
        ID_YEAR_TEXTCTRL,
        ID_FIND_RESULE_GRID
    };
    
    DECLARE_EVENT_TABLE();
};

#endif