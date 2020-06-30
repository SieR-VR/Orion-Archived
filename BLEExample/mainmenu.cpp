#include "mainmenu.h"
#include "ui_mainmenu.h"

struct OverlayShereMem {
    float q1, q2, q3, q4;
};

HANDLE hMaapping = CreateFileMapping(
    INVALID_HANDLE_VALUE,
    NULL,
    PAGE_READWRITE,
    0,
    sizeof(OverlayShereMem),
    L"SH_MEM"
);

OverlayShereMem* buuf = (OverlayShereMem*)MapViewOfFile(
    hMaapping,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    0
);

mainMenu::mainMenu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainMenu)
{
    ui->setupUi(this);

    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    ui->btnRead->setEnabled(false);
    ui->btnNotify->setEnabled(false);
    ui->cbDevices->setEnabled(false);
    ui->cbServices->setEnabled(false);
    ui->cbCharacteristics->setEnabled(false);

    handler = new BLEHandler();
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &mainMenu::dataUpdate);

    connect(handler, &BLEHandler::textRecived, this, &mainMenu::writeText);
    connect(handler, &BLEHandler::deviceListUpdated, this, &mainMenu::updateDeviceList);
    connect(handler, &BLEHandler::serviceListUpdated, this, &mainMenu::updateServiceList);
    connect(handler, &BLEHandler::characteristicListUpdated, this, &mainMenu::updateCharacteristicList);
}

mainMenu::~mainMenu() {
    delete ui;
    UnmapViewOfFile(buuf);
    CloseHandle(hMaapping);
}

void mainMenu::writeText(const QString &text) {
    QString compare;
    if(text == compare)
        return;
    ui->textLog->insertPlainText(text+"\n");
}

void mainMenu::updateDeviceList() {
    ui->cbDevices->clear();
    ui->cbDevices->addItems(handler->getDeviceListString());
    if(ui->cbDevices->count() == 0) {
        ui->cbDevices->setEnabled(false);
        ui->btnConnect->setEnabled(false);
    }
    else {
        ui->cbDevices->setEnabled(true);
        ui->btnConnect->setEnabled(true);
    }
}

void mainMenu::updateServiceList() {
    ui->cbServices->clear();
    ui->cbServices->addItems(handler->getServiceListString());
    if(ui->cbServices->count() == 0) {
        ui->cbServices->setEnabled(false);
        ui->cbCharacteristics->setEnabled(false);
    }
    else {
        ui->cbServices->setEnabled(true);
        ui->cbCharacteristics->setEnabled(true);
    }
    ui->btnDisconnect->setEnabled(true);
}

void mainMenu::updateCharacteristicList() {
    ui->cbCharacteristics->clear();
    ui->cbCharacteristics->addItems(handler->getCharacteristicListString(ui->cbServices->currentIndex()));
    if(ui->cbCharacteristics->count() == 0) {
        ui->cbCharacteristics->setEnabled(false);
        ui->btnRead->setEnabled(false);
        ui->btnNotify->setEnabled(false);
    }
    else {
        ui->cbCharacteristics->setEnabled(true);
        ui->btnRead->setEnabled(true);
        ui->btnNotify->setEnabled(true);
    }
}

void mainMenu::dataUpdate() {
    auto data = handler->getData(ui->cbServices->currentIndex(), ui->cbCharacteristics->currentIndex());
    if(ui->cbServices->currentIndex() == 2 && ui->cbCharacteristics->currentIndex() == 0) {
        short q1 = data.at(0) << 8 | data.at(1); short q2 = data.at(2) << 8 | data.at(3); short q3 = data.at(4) << 8 | data.at(5); short q4 = data.at(6) << 8 | data.at(7);
        buuf->q1 = q1 / 16384.0; buuf->q2 = q2 / 16384.0; buuf->q3 = q3 / 16384.0; buuf->q4 = q4 / 16384.0;
        writeText(QString::number(q1 / 16384.0) + " " + QString::number(q2 / 16384.0) + " " + QString::number(q3 / 16384.0) + " " + QString::number(q4 / 16384.0));
    }
    else {
        writeText(data);
    }
}

void mainMenu::on_btnScan_clicked() {
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    ui->btnRead->setEnabled(false);
    ui->btnNotify->setEnabled(false);
    ui->cbDevices->setEnabled(false);
    ui->cbServices->setEnabled(false);
    ui->cbCharacteristics->setEnabled(false);
    handler->scanDevice();
}

void mainMenu::on_btnConnect_clicked() {
    handler->connectToDevice(ui->cbDevices->currentIndex());
}

void mainMenu::on_btnDisconnect_clicked(){
    ui->btnConnect->setEnabled(false);
    ui->btnDisconnect->setEnabled(false);
    ui->btnRead->setEnabled(false);
    ui->btnNotify->setEnabled(false);
    ui->cbDevices->setEnabled(false);
    ui->cbServices->setEnabled(false);
    ui->cbCharacteristics->setEnabled(false);
    handler->disconnectFromDevice();
}

void mainMenu::on_cbServices_currentIndexChanged(int index)
{
    updateCharacteristicList();
}

void mainMenu::on_btnNotify_clicked()
{
    timer->start(100);
}

void mainMenu::on_btnRead_clicked()
{
    dataUpdate();
}
