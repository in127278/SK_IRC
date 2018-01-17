/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Mainwindow
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEdit;
    QSpinBox *spinBox;
    QPushButton *pushButton;
    QTextEdit *textEdit;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_2;
    QLineEdit *lineEdit_3;
    QPushButton *pushButton_3;
    QSpinBox *spinBox_2;
    QLabel *label;
    QCheckBox *checkBox;
    QComboBox *comboBox;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Mainwindow)
    {
        if (Mainwindow->objectName().isEmpty())
            Mainwindow->setObjectName(QStringLiteral("Mainwindow"));
        Mainwindow->resize(400, 300);
        centralWidget = new QWidget(Mainwindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(20, 0, 161, 31));
        spinBox = new QSpinBox(centralWidget);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setGeometry(QRect(210, 0, 91, 31));
        spinBox->setMinimum(1000);
        spinBox->setMaximum(2000);
        spinBox->setValue(1234);
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(310, 0, 91, 41));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(20, 40, 281, 131));
        lineEdit_2 = new QLineEdit(centralWidget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(20, 180, 281, 31));
        pushButton_2 = new QPushButton(centralWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setGeometry(QRect(310, 180, 89, 25));
        lineEdit_3 = new QLineEdit(centralWidget);
        lineEdit_3->setObjectName(QStringLiteral("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(100, 130, 171, 31));
        pushButton_3 = new QPushButton(centralWidget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setGeometry(QRect(290, 40, 91, 131));
        spinBox_2 = new QSpinBox(centralWidget);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setGeometry(QRect(210, 50, 61, 31));
        spinBox_2->setMinimum(1200);
        spinBox_2->setMaximum(1250);
        spinBox_2->setValue(1234);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(60, 130, 41, 21));
        checkBox = new QCheckBox(centralWidget);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(60, 90, 151, 21));
        checkBox->setChecked(true);
        comboBox = new QComboBox(centralWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(60, 50, 141, 21));
        comboBox->setEditable(true);
        Mainwindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Mainwindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 22));
        Mainwindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Mainwindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Mainwindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Mainwindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Mainwindow->setStatusBar(statusBar);

        retranslateUi(Mainwindow);

        QMetaObject::connectSlotsByName(Mainwindow);
    } // setupUi

    void retranslateUi(QMainWindow *Mainwindow)
    {
        Mainwindow->setWindowTitle(QApplication::translate("Mainwindow", "IRC Client", nullptr));
        lineEdit->setText(QApplication::translate("Mainwindow", "localhost", nullptr));
        pushButton->setText(QApplication::translate("Mainwindow", "Connect", nullptr));
        pushButton_2->setText(QApplication::translate("Mainwindow", "Send", nullptr));
        pushButton_3->setText(QApplication::translate("Mainwindow", "Connect", nullptr));
        label->setText(QApplication::translate("Mainwindow", "Nick:", nullptr));
        checkBox->setText(QApplication::translate("Mainwindow", "use default port", nullptr));
        comboBox->setItemText(0, QApplication::translate("Mainwindow", "localhost", nullptr));
        comboBox->setItemText(1, QApplication::translate("Mainwindow", "192.168.0.12", nullptr));
        comboBox->setItemText(2, QApplication::translate("Mainwindow", "192.168.2.12", nullptr));
        comboBox->setItemText(3, QApplication::translate("Mainwindow", "127.0.0.1", nullptr));

    } // retranslateUi

};

namespace Ui {
    class Mainwindow: public Ui_Mainwindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
