#include "mainmenu.h"
#include "ui_mainmenu.h"

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
    connect(handler, &BLEHandler::textRecived, this, &mainMenu::writeText);
    connect(handler, &BLEHandler::deviceListUpdated, this, &mainMenu::updateDeviceList);
    connect(handler, &BLEHandler::serviceListUpdated, this, &mainMenu::updateServiceList);
    connect(handler, &BLEHandler::characteristicListUpdated, this, &mainMenu::updateCharacteristicList);
    connect(handler, &BLEHandler::valueChanged, this, &mainMenu::dataNotifyUpdate);

    if(!shereMemHandler.createShereMem())
        shereMemHandler.connectToShereMem();
}

mainMenu::~mainMenu() {
    delete handler;
    delete ui;
}

void mainMenu::writeText(const QString &text) {
    if(!text.size())
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
    writeText(data);
}

void mainMenu::dataNotifyUpdate(const QByteArray &data, const QString &uuid) {
    float qw, qx, qy, qz;
    memcpy_s(&qw, sizeof(float), &data.data()[0], sizeof(float));
    memcpy_s(&qx, sizeof(float), &data.data()[4], sizeof(float));
    memcpy_s(&qy, sizeof(float), &data.data()[8], sizeof(float));
    memcpy_s(&qz, sizeof(float), &data.data()[12], sizeof(float));
    ui->lQuat->setText(QString::number(qw) + " " + QString::number(qx) + " " + QString::number(qy) + " " + QString::number(qz));

    OrionSensorData sensorData = {qw, qx, qy, qz, true};
    shereMemHandler.dataUpdate(sensorData);
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

void mainMenu::on_cbServices_currentIndexChanged(int index) {
    updateCharacteristicList();
}

void mainMenu::on_btnNotify_clicked() {
    handler->subscribeData(ui->cbServices->currentIndex(), ui->cbCharacteristics->currentIndex());
}

void mainMenu::on_btnRead_clicked() {
    dataUpdate();
}
