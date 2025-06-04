#ifndef AITHREAD_H
#define AITHREAD_H

#include <Python.h>
#include <QThread>
#include <QMutex>
#include <queue>


using namespace std;

enum MsgType
{
    text,
    audio
};

struct MsgInfo
{
    MsgType msgType;
    QString context;
};

class AIThread :public QThread
{
    Q_OBJECT
public:
    void stop();//结束线程
    void setMsg(MsgInfo msg);//加入消息

signals:
    void modelsLoadFinished();
    void readResponse(QString context, QString response);//读取响应消息

private:
    void run();//线程逻辑
    void initPyApi();//初始化python api

private:
    bool isRun = true;//线程运行标识
    QMutex mutex;//互斥所
    queue<MsgInfo> msgQue;//消息队列
    PyObject* moudle;// 调用的模块
    PyObject* loadModelFun;// 加载大模型
    PyObject* doTextFun;// 调用的大模型函数
    PyObject* doAutioFun;// 调用的语音识别函数
    char* textResultStr;// 接收大模型返回值的参数
    char* audioResultStr;//接收语音大模型返回值

    
};

#endif // AITHREAD_H
