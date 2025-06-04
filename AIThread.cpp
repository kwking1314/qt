#include "AIThread.h"
#include "PathDef.h"
#include <QDebug>

#define MODULE_NAME "forcpp"

void AIThread::stop()
{
    isRun = false;
}

void AIThread::setMsg(MsgInfo msg)
{
    mutex.lock();
    msgQue.push(msg);
    mutex.unlock();
}

void AIThread::run()
{
    initPyApi();
    
    while(isRun)
    {
        if(msgQue.empty())//没有消息
        {
            usleep(100);//延时
            continue;
        }
        mutex.lock();
        MsgInfo msg = msgQue.front();
        msgQue.pop();
        mutex.unlock();
        switch(msg.msgType)
        {
        case text:
            {
            //调用py脚本 doText函数
                PyObject* args = PyTuple_New(1);
                char *info = msg.context.toUtf8().data();
                PyTuple_SetItem(args,0,Py_BuildValue("s",info));
                PyObject* ret = PyObject_CallObject(doTextFun,args);
                PyArg_Parse(ret, "s", &textResultStr);
                qDebug() << textResultStr ;
                QString respone(textResultStr);
                emit readResponse(msg.context, respone); 
                break;
            }
        case audio:
            {
                //先调用py脚本doAudio函数
                PyObject* args = PyTuple_New(1);
                PyTuple_SetItem(args,0,Py_BuildValue("s",MODULE_PATH));
                PyObject* audioRet = PyObject_CallObject(doAutioFun,args);
                PyArg_Parse(audioRet, "s",&audioResultStr);
                qDebug() << "audio" << audioResultStr ;
                
                //再调用py脚本doText函数
                PyObject* args2 = PyTuple_New(1);
                PyTuple_SetItem(args2,0,Py_BuildValue("s",audioResultStr));
                PyObject* textRet = PyObject_CallObject(doTextFun,args2);
                PyArg_Parse(textRet, "s", &textResultStr);
            
                qDebug() << "llm" << textResultStr ;
                QString respone(textResultStr);
                emit readResponse(QString(audioResultStr), respone); 
                break;
            }
        }  
    }
    qDebug() << "end...";
    Py_Finalize();
}

void AIThread::initPyApi()
{
    // 开始环境
    Py_Initialize();
    if(!Py_IsInitialized())
    {
        qDebug() << "error";
        return;

    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('" MODULE_PATH "')");
    PyRun_SimpleString("sys.path.append('" MODULE_PATH "lora_output')");
    moudle = PyImport_ImportModule(MODULE_NAME);
    if(moudle == nullptr)
    {
        qDebug() << "moudle error";
        Py_Finalize();
        return;
    }
    //加载模型函数
    loadModelFun = PyObject_GetAttrString(moudle,"loadModel");
    if(!loadModelFun || !PyCallable_Check(loadModelFun))
    {
        qDebug() << "doText fun error";
        Py_Finalize();
    }
    // 访问大模型函数
    doTextFun = PyObject_GetAttrString(moudle,"doText");
    if(!doTextFun || !PyCallable_Check(doTextFun))
    {
        qDebug() << "doText fun error";
        Py_Finalize();
    }
    // 访问语音识别函数
    doAutioFun = PyObject_GetAttrString(moudle,"doAudio");
    if(!doAutioFun || !PyCallable_Check(doAutioFun))
    {
        qDebug() << "doAudio fun error";
        Py_Finalize();
    }
    
    //加载大模型
    PyObject* args = PyTuple_New(1);
    PyTuple_SetItem(args,0,Py_BuildValue("s",MODULE_PATH));
    PyObject_CallObject(loadModelFun,args);
    emit modelsLoadFinished();
}
