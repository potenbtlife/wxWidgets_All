#ifndef ASTMANAGER_TIMER_H
#define ASTMANAGER_TIMER_H

#include <wx/timer.h>
#include "CRTData.h"

const std::string m_insertSql = "insert into value_info(compose_id,recordtime,marketvalue,cash,stock_ratio,\
                                value_advice,detail_info,fund_share,fund_value,reference_index) \
                                values(?,?,?,?,?,?,?,?,?,?)";
const std::string m_qryUnique = "select 1 from value_info where recordtime=?";
const std::string m_qryComposefirst = "select 1 from value_info where compose_id=?";

const std::string updatePrice = "update stock_list set curprice=? where stock_id=? and type=?";

class Timer : public wxTimer{
public:

    void Notify();
    bool UpdateAllPrice();

private:
    int RegValueInfo(int composeID,string recordtime,double marketvalue, string advice,string detailInfo, double ref_index);
    int QryUnique(string recordtime);
    bool QryAllUnique(int composeID);
};



#endif //ASTMANAGER_TIMER_H