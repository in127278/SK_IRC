#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
namespace Ui {
class Mainwindow;
}

class Mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainwindow(QWidget *parent = 0);
    ~Mainwindow();
    QTcpSocket *tcpSocket = nullptr;
    QDataStream in;
private slots:
void on_pushButton_pressed();
void connecttoserver();
void disconnectedfroms();
void readingdata();
void on_lineEdit_2_returnPressed();
void display();

void on_pushButton_2_clicked();

void on_pushButton_3_clicked();

void on_checkBox_toggled(bool checked);

private:
    Ui::Mainwindow *ui;
};

#endif // MAINWINDOW_H
