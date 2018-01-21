#include "mainwindow.h"
#include "ui_mainwindow.h"
Mainwindow::Mainwindow(QWidget *parent) :
    QMainWindow(parent),
    tcpSocket(new QTcpSocket(this)),
    ui(new Ui::Mainwindow)
{
    ui->setupUi(this);
    ui->lineEdit->hide();
    ui->lineEdit_2->hide();
    ui->pushButton->hide();
    ui->pushButton_2->hide();
    ui->pushButton_4->hide();
    ui->spinBox->hide();
    ui->textEdit->hide();
    ui->spinBox_2->setEnabled(false);
    connect(tcpSocket,&QTcpSocket::connected,this,&Mainwindow::connecttoserver);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&Mainwindow::disconnectedfroms);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Mainwindow::display);
}

Mainwindow::~Mainwindow()
{
    delete ui;
}

void Mainwindow::on_pushButton_pressed()
{   tcpSocket->disconnectFromHost();
    tcpSocket->connectToHost(ui->lineEdit->text(),ui->spinBox->value());
}

void Mainwindow::connecttoserver()
{

    ui->lineEdit_2->show();

    ui->pushButton_2->show();
    ui->pushButton_4->show();

    ui->textEdit->show();
    ui->comboBox->hide();
    ui->spinBox_2->hide();
    ui->pushButton_3->hide();
    ui->lineEdit_3->hide();
    ui->checkBox->hide();
    ui->label->hide();
    QByteArray msg ="/nick " + ui->lineEdit_3->text().toUtf8();
    tcpSocket->write(msg);
}
void Mainwindow::disconnectedfroms()
{
    ui->textEdit->append("Disconnected");
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
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

void Mainwindow::on_pushButton_3_clicked()
{
    QString info = ui->comboBox->currentText();
    if(ui->lineEdit_3->text().toUtf8().size() > 0 ){

        tcpSocket->connectToHost(info,ui->spinBox_2->value());

    }
}

void Mainwindow::on_checkBox_toggled(bool checked)
{
    ui->spinBox_2->setEnabled(not(checked));
}

void Mainwindow::on_pushButton_4_clicked()
{   if(ui->pushButton_4->isChecked() == true){
        ui->lineEdit->show();
        ui->spinBox->show();
        ui->pushButton->show();
    }
    else{
        ui->lineEdit->hide();
        ui->spinBox->hide();
        ui->pushButton->hide();
    }
}
