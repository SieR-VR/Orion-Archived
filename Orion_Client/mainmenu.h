#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMainWindow>
#include <QTimer>
#include <Orion.h>
#include <Windows.h>
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
    void updateDeviceList();
    void updateServiceList();
    void updateCharacteristicList();
    void dataUpdate();

    void writeText(const QString &text);

    void on_btnScan_clicked();
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnNotify_clicked();
    void on_btnRead_clicked();
    void on_cbServices_currentIndexChanged(int index);

    bool driverMemoryCreate();
    void driverDataUpdate(const orion::trackerData &data);

private:
    Ui::mainMenu *ui;
    BLEHandler *handler;
    QTimer *timer;

    HANDLE driverDataHandle;
    char *driverData;
    bool isSharedMemCreated;
};
#endif // MAINMENU_H
