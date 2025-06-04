#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    thread = new AIThread();
    connect(thread, &AIThread::finished, thread, &AIThread::deleteLater);
    connect(thread, &AIThread::modelsLoadFinished, this, &MainWindow::onModelsLoadFinished);
    connect(thread, &AIThread::readResponse, this, &MainWindow::onReadResponse);
    thread->start();
    initUdp();
    ui->textBrowser->append("开始加载大模型");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUdp()
{
    socket = new QUdpSocket(this);
    connect(socket, &QUdpSocket::readyRead, this, &MainWindow::onReadyRead);
    int port = ui->lineEdit_port->text().toInt();
    socket->bind(QHostAddress::Any, port);
    
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for(int i = 0;i < addresses.size();i++)
    {
        if(addresses[i].protocol() == QAbstractSocket::IPv4Protocol)
        {
            ui->listWidget_ip->addItem(addresses[i].toString());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    thread->stop();
}

void MainWindow::on_pushButton_text_clicked()
{
    QString data = ui->lineEdit->text();
    MsgInfo msg;
    msg.msgType = text;
    msg.context = data;
    thread->setMsg(msg);
    ui->pushButton_text->setEnabled(false);
}

void MainWindow::onReadResponse(QString context, QString response)
{
    QString ip = ui->lineEdit_ip->text();
    ui->textBrowser->append(context + "  " + response);
    ui->pushButton_audio->setEnabled(true);
    ui->pushButton_text->setEnabled(true);
    if(response == "led on")
    {
        QString send("led on");
        socket->writeDatagram(send.toLocal8Bit(), QHostAddress(ip), 8081);
    }
    else if(response == "led off")
    {
        QString send("led off");
        socket->writeDatagram(send.toLocal8Bit(), QHostAddress(ip), 8081);
    }
}

void MainWindow::onModelsLoadFinished()
{
    ui->textBrowser->append("大模型加载完毕");
    ui->pushButton_text->setEnabled(true);
    ui->pushButton_audio->setEnabled(true);
}

void MainWindow::on_pushButton_audio_pressed()
{
    myAudio.startAudio();
}

void MainWindow::on_pushButton_audio_released()
{
    myAudio.stopAudio();
    MsgInfo msg;
    msg.msgType = audio;
    thread->setMsg(msg);
    ui->pushButton_audio->setEnabled(false);
}

void MainWindow::onReadyRead()
{
    while(socket->hasPendingDatagrams())
    {
        QHostAddress from;
        QByteArray dataByte;
        dataByte.resize(socket->pendingDatagramSize());
        socket->readDatagram(dataByte.data(), dataByte.size(), &from);
        qDebug()<<QString::fromUtf8(dataByte);
    }
}
