//线程相关代码，配合其他cpp（main.cpp）使用，为了精简其他文件独立开来

#ifndef AST_MYTHREAD_H
#define  AST_MYTHREAD_H


#include "wx/thread.h"

class CalcFinanceIndexThread : public wxThread
{
public:
    CalcFinanceIndexThread(): wxThread() {
    };

    // thread execution starts here
    virtual void *Entry(){
        wxLogDebug("begin CalcFinanceIndexThread::Entry");

        DeleteFinanceIndexFromDb("年报");
        FinanceFrame::CalcAllFinanceIndex("年报");
        DeleteFinanceIndexFromDb("中报");
        FinanceFrame::CalcAllFinanceIndex("中报");
        //wxThread::Sleep(2000);

        wxLogDebug("end CalcFinanceIndexThread::Entry");
        wxMessageBox("计算指标结束！");
        return NULL;
    };

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit(){};

};


#endif