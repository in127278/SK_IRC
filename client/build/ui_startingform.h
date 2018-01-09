/********************************************************************************
** Form generated from reading UI file 'startingform.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STARTINGFORM_H
#define UI_STARTINGFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StartingForm
{
public:
    QComboBox *comboBox;
    QPushButton *pushButton;
    QLineEdit *lineEdit;

    void setupUi(QWidget *StartingForm)
    {
        if (StartingForm->objectName().isEmpty())
            StartingForm->setObjectName(QStringLiteral("StartingForm"));
        StartingForm->resize(400, 300);
        comboBox = new QComboBox(StartingForm);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(70, 70, 191, 31));
        pushButton = new QPushButton(StartingForm);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(270, 70, 91, 111));
        lineEdit = new QLineEdit(StartingForm);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(70, 150, 191, 31));

        retranslateUi(StartingForm);

        QMetaObject::connectSlotsByName(StartingForm);
    } // setupUi

    void retranslateUi(QWidget *StartingForm)
    {
        StartingForm->setWindowTitle(QApplication::translate("StartingForm", "Form", nullptr));
        pushButton->setText(QApplication::translate("StartingForm", "Connect", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StartingForm: public Ui_StartingForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STARTINGFORM_H
