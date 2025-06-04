#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "AIThread.h"
#include <QMainWindow>
#include "MyAudio.h"
#include <QUdpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private:
    void initUdp();
    void closeEvent(QCloseEvent* e);
private slots:
    void on_pushButton_text_clicked();
    void onReadResponse(QString context, QString response);
    void onModelsLoadFinished();
    void on_pushButton_audio_pressed();
    void on_pushButton_audio_released();
    void onReadyRead();
private:
    Ui::MainWindow *ui;
    AIThread* thread;
    MyAudio myAudio;
    QUdpSocket* socket;
};
#endif // MAINWINDOW_H
