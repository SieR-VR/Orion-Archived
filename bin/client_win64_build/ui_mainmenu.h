/********************************************************************************
** Form generated from reading UI file 'mainmenu.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINMENU_H
#define UI_MAINMENU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mainMenu
{
public:
    QWidget *centralwidget;
    QLabel *lDevices;
    QComboBox *cbDevices;
    QPushButton *btnScan;
    QPushButton *btnConnect;
    QPushButton *btnDisconnect;
    QTextBrowser *textLog;
    QLabel *lServices;
    QComboBox *cbServices;
    QLabel *lCharacteristics;
    QComboBox *cbCharacteristics;
    QPushButton *btnNotify;
    QPushButton *btnRead;

    void setupUi(QMainWindow *mainMenu)
    {
        if (mainMenu->objectName().isEmpty())
            mainMenu->setObjectName(QString::fromUtf8("mainMenu"));
        mainMenu->resize(800, 300);
        centralwidget = new QWidget(mainMenu);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        lDevices = new QLabel(centralwidget);
        lDevices->setObjectName(QString::fromUtf8("lDevices"));
        lDevices->setGeometry(QRect(10, 10, 81, 21));
        cbDevices = new QComboBox(centralwidget);
        cbDevices->setObjectName(QString::fromUtf8("cbDevices"));
        cbDevices->setGeometry(QRect(100, 10, 451, 22));
        btnScan = new QPushButton(centralwidget);
        btnScan->setObjectName(QString::fromUtf8("btnScan"));
        btnScan->setGeometry(QRect(560, 10, 71, 23));
        btnConnect = new QPushButton(centralwidget);
        btnConnect->setObjectName(QString::fromUtf8("btnConnect"));
        btnConnect->setGeometry(QRect(640, 10, 71, 23));
        btnDisconnect = new QPushButton(centralwidget);
        btnDisconnect->setObjectName(QString::fromUtf8("btnDisconnect"));
        btnDisconnect->setGeometry(QRect(720, 10, 71, 23));
        textLog = new QTextBrowser(centralwidget);
        textLog->setObjectName(QString::fromUtf8("textLog"));
        textLog->setGeometry(QRect(10, 100, 781, 192));
        lServices = new QLabel(centralwidget);
        lServices->setObjectName(QString::fromUtf8("lServices"));
        lServices->setGeometry(QRect(10, 40, 81, 21));
        cbServices = new QComboBox(centralwidget);
        cbServices->setObjectName(QString::fromUtf8("cbServices"));
        cbServices->setGeometry(QRect(100, 40, 691, 21));
        lCharacteristics = new QLabel(centralwidget);
        lCharacteristics->setObjectName(QString::fromUtf8("lCharacteristics"));
        lCharacteristics->setGeometry(QRect(10, 70, 91, 21));
        cbCharacteristics = new QComboBox(centralwidget);
        cbCharacteristics->setObjectName(QString::fromUtf8("cbCharacteristics"));
        cbCharacteristics->setGeometry(QRect(100, 70, 531, 22));
        btnNotify = new QPushButton(centralwidget);
        btnNotify->setObjectName(QString::fromUtf8("btnNotify"));
        btnNotify->setGeometry(QRect(720, 70, 75, 23));
        btnRead = new QPushButton(centralwidget);
        btnRead->setObjectName(QString::fromUtf8("btnRead"));
        btnRead->setGeometry(QRect(640, 70, 75, 23));
        mainMenu->setCentralWidget(centralwidget);

        retranslateUi(mainMenu);

        QMetaObject::connectSlotsByName(mainMenu);
    } // setupUi

    void retranslateUi(QMainWindow *mainMenu)
    {
        mainMenu->setWindowTitle(QCoreApplication::translate("mainMenu", "mainMenu", nullptr));
        lDevices->setText(QCoreApplication::translate("mainMenu", "Devices", nullptr));
        btnScan->setText(QCoreApplication::translate("mainMenu", "Scan", nullptr));
        btnConnect->setText(QCoreApplication::translate("mainMenu", "Connect", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("mainMenu", "Disconnect", nullptr));
        lServices->setText(QCoreApplication::translate("mainMenu", "Services", nullptr));
        lCharacteristics->setText(QCoreApplication::translate("mainMenu", "Characteristics", nullptr));
        btnNotify->setText(QCoreApplication::translate("mainMenu", "Notify", nullptr));
        btnRead->setText(QCoreApplication::translate("mainMenu", "Read", nullptr));
    } // retranslateUi

};

namespace Ui {
    class mainMenu: public Ui_mainMenu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINMENU_H
