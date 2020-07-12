#ifndef BLEHANDLER_H
#define BLEHANDLER_H

#include <QObject>
#include <QDebug>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServiceDiscoveryAgent>
#include <QLowEnergyController>

class BLEHandler : public QObject
{
    Q_OBJECT
public:
    BLEHandler(QObject *parent = nullptr);
    ~BLEHandler();

    //agent (devices);
    void scanDevice();
    QStringList getDeviceListString();

    //controller (services)
    void connectToDevice(const int &index);
    void disconnectFromDevice();
    QStringList getServiceListString();

    //service (characteristics)
    QStringList getCharacteristicListString(const int &serviceIndex);
    QByteArray getData(const int &serviceIndex, const int &characteristicIndex);
    bool subscribeData(const int &serviceIndex, const int &characteristicIndex);

signals:
    //devices
    void deviceListUpdated();

    //services
    void serviceListUpdated();

    //characteristics
    void characteristicListUpdated();
    void valueChanged(const QByteArray &data, const QString &uuid);

    //log
    void textRecived(const QString &text);

private slots:
    //agent (devices)
    void addDevice(const QBluetoothDeviceInfo &info);
    void updateDevice();
    void agentError(QBluetoothDeviceDiscoveryAgent::Error error);

    //controller (services)
    void deviceConnected();
    void deviceDisconnected();
    void addService(const QBluetoothUuid &newService);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error error);

    //service (characteristics)
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void serviceError(QLowEnergyService::ServiceError error);
    void updateValue(QLowEnergyCharacteristic characteristic, QByteArray data);

private:
    QBluetoothDeviceDiscoveryAgent *m_agent;
    QLowEnergyController *m_controller;

    QList<QBluetoothDeviceInfo *> m_devices;
    QList<QLowEnergyService *> m_services;
};

#endif // BLEHANDLER_H
