#include "common.h"

vector<DataContainer> gVecAllData; //保存所有的明细数据
const unsigned int lineNumEachPage = 12; //每页的行数
unsigned int curPage = 1; //当前页码

void fillPageFromVector2GridTable(vector<DataContainer>& vSrc, wxGridStringTable& gridStrTable, unsigned int numInOnePage, unsigned int pageNum) {
    
    if( vSrc.size() < numInOnePage * (pageNum - 1) ){ //记录数小于上一页的总行数，直接返回
        return;
    }

    gridStrTable.Clear();
    //规整gridStringTable中的行数为numInOnePage
    if(gridStrTable.GetNumberRows() > getRowNumInOnePage(vSrc, numInOnePage, pageNum)){
        gridStrTable.DeleteRows(0, gridStrTable.GetNumberRows() - getRowNumInOnePage(vSrc, numInOnePage, pageNum));
    }else{
        gridStrTable.AppendRows(getRowNumInOnePage(vSrc, numInOnePage, pageNum) - gridStrTable.GetNumberRows());
    }

    vector<DataContainer>::iterator iter = vSrc.begin();
    iter += numInOnePage * (pageNum - 1);// 跳转到pageNum页的记录
    for(int i = 0; i < getRowNumInOnePage(vSrc, numInOnePage, pageNum) && iter != vSrc.end(); ++i, ++iter){

        char cWeekOfYear[16];
        memset(cWeekOfYear,'0',sizeof(cWeekOfYear));
        sprintf(cWeekOfYear,"第%d周",iter->iWeekofYear);
        gridStrTable.SetValue(i,0,cWeekOfYear);
        gridStrTable.SetValue(i,1,GetDayOfWeek(iter->idayOfWeek));
        gridStrTable.SetValue(i,2,iter->startTime);
        gridStrTable.SetValue(i,3,iter->endTime);
        gridStrTable.SetValue(i,4,timeConvert(iter->usedTimeSec));
        gridStrTable.SetValue(i,5,iter->doTings);
        gridStrTable.SetValue(i,6,iter->priority);
    }
}
