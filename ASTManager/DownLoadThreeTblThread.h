//线程相关代码，配合其他cpp（main.cpp）使用，为了精简其他文件独立开来

#ifndef AST_DOWNLOADTHREETBLTHREAD_H
#define  AST_DOWNLOADTHREETBLTHREAD_H


#include "wx/thread.h"

class DownLoadThreeTblThread : public wxThread
{
public:
    DownLoadThreeTblThread(MyFrame *frame, int reportType): wxThread(),m_reportType(reportType) {
        m_frame = frame;
    };

    // thread execution starts here
    virtual void *Entry(){
        //Runtime::getInstance()->myTimer.Stop();//先停止轮询，否则会由于抢占操作数据库，导致出问题。
        
        wxLogDebug("begin DownLoadThreadTblThread::Entry");
        m_frame->WriteThreeRptToDb(m_reportType);
        wxLogDebug("end DownLoadThreadTblThread::Entry");
        wxMessageBox("下载报表结束！");

        //Runtime::getInstance()->myTimer.Start(1000*60);//1分钟触发一次

        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

public:
    MyFrame *m_frame;
    int m_reportType;
};


#endif