#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <Windows.h>
#include <QTimer>
#include "blehandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainMenu; }
QT_END_NAMESPACE

class mainMenu : public QMainWindow
{
    Q_OBJECT

public:
    mainMenu(QWidget *parent = nullptr);
    ~mainMenu();

private slots:
    void on_btnScan_clicked();

    void updateDeviceList();
    void updateServiceList();
    void updateCharacteristicList();

    void dataUpdate();

    void writeText(const QString &text);

    void on_btnConnect_clicked();

    void on_cbServices_currentIndexChanged(int index);

    void on_btnNotify_clicked();

    void on_btnDisconnect_clicked();

    void on_btnRead_clicked();

private:
    Ui::mainMenu *ui;
    BLEHandler *handler;
    QTimer *timer;
};
#endif // MAINMENU_H
