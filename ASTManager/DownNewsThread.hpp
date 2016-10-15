//线程相关代码，配合其他cpp（main.cpp）使用，为了精简其他文件独立开来

#ifndef AST_DOWNNEWSTHREAD_H
#define  AST_DOWNNEWSTHREAD_H

#include "CRTData.h"
#include "wx/thread.h"

class LoadNewsThread : public wxThread
{
public:
    LoadNewsThread(wxDateTime& beginDate, wxDateTime& endDate, string& dir, int beginNum=0, int endNum=999): wxThread() {
        m_beginDate = beginDate;
        m_endDate = endDate;
        m_dir = dir;
        m_beginNum = beginNum;
        m_endNum = endNum;
    };

    // thread execution starts here
    virtual void *Entry(){
        wxLogDebug("begin LoadNewsThread::Entry");

        //Runtime::getInstance()->myTimer.Stop();//先停止轮询，否则会由于抢占操作数据库，导致出问题。

        CRTData* tmpRtData;
        createRtDataObj(&tmpRtData);

        wxDateSpan dateSpan(0,0,0,1);//减1天

        for(; m_endDate>=m_beginDate; m_endDate.Subtract(dateSpan)){
            tmpRtData->LoadOneDayNews( m_endDate, m_dir, m_beginNum, m_endNum);
        }

        delete tmpRtData;

        //Runtime::getInstance()->myTimer.Start(1000*60);//1分钟触发一次
        wxLogDebug("end LoadNewsThread::Entry");
        wxMessageBox("下载news结束.");
        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

public:
    wxDateTime m_beginDate;
    wxDateTime m_endDate;
    string m_dir;
    int m_beginNum; //开始序号，为了支持多进程同时下载
    int m_endNum;  //结束序号，为了支持多进程同时下载
};


#endif //AST_DOWNNEWSTHREAD_H