#include "mainwindow.h"
#include "ui_mainwindow.h"
Mainwindow::Mainwindow(QWidget *parent) :
    QMainWindow(parent),
    tcpSocket(new QTcpSocket(this)),
    ui(new Ui::Mainwindow)
{
    ui->setupUi(this);
    connect(tcpSocket,&QTcpSocket::connected,this,&Mainwindow::connecttoserver);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&Mainwindow::disconnectedfroms);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Mainwindow::display);
}

Mainwindow::~Mainwindow()
{
    delete ui;
}

void Mainwindow::on_pushButton_pressed()
{
    tcpSocket->connectToHost(ui->lineEdit->text(),ui->spinBox->value());
}

void Mainwindow::connecttoserver()
{
    //ui->textEdit->append("Connected");

}
void Mainwindow::disconnectedfroms()
{
    ui->textEdit->append("Disconnected");
}
void Mainwindow::readingdata()
{
       if(tcpSocket->state() == QAbstractSocket::ConnectedState)
       {

            ui->textEdit->append(tcpSocket->readAll());
       }
}

void Mainwindow::on_lineEdit_2_returnPressed()
{
    if(tcpSocket->state() == QAbstractSocket::ConnectedState)
    {   QByteArray msg = ui->lineEdit_2->text().toUtf8();
        tcpSocket->write(msg);
        ui->lineEdit_2->clear();
    }

}
void Mainwindow::display(){

    QByteArray buffer = tcpSocket->readAll();
    ui->textEdit->append(buffer);
}

void Mainwindow::on_pushButton_2_clicked()
{
    if(tcpSocket->state() == QAbstractSocket::ConnectedState)
    {   QByteArray msg = ui->lineEdit_2->text().toUtf8();
        tcpSocket->write(msg);
        ui->lineEdit_2->clear();
    }
}
