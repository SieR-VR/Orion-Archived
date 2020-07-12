#include "blehandler.h"

BLEHandler::BLEHandler(QObject *parent) : QObject(parent),
    m_agent(0)
{
    m_agent = new QBluetoothDeviceDiscoveryAgent();
    m_agent->setLowEnergyDiscoveryTimeout(3000);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BLEHandler::addDevice);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BLEHandler::updateDevice);
    connect(m_agent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &BLEHandler::agentError);
}

BLEHandler::~BLEHandler() {
    delete m_agent;
    qDeleteAll(m_devices);
    m_devices.clear();
}
void BLEHandler::scanDevice() {
    if(m_agent->isActive()) {
        m_agent->stop();
    }

    qDeleteAll(m_devices);
    m_devices.clear();
    emit deviceListUpdated();
    emit characteristicListUpdated();

    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BLEHandler::addDevice(const QBluetoothDeviceInfo &info) {
    emit textRecived("Device found - " + info.name() + " : " + info.address().toString());
}

void BLEHandler::updateDevice() {
    auto foundDevices = m_agent->discoveredDevices();
    for(auto device : foundDevices) {
        m_devices.append(new QBluetoothDeviceInfo(device));
    }

    if(m_devices.empty()) {
        emit textRecived("No devices found..");
    }
    else {
        emit textRecived("Devices found!");
    }

    emit deviceListUpdated();
}

void BLEHandler::connectToDevice(const int &index) {
    if(m_devices.empty()) {
        emit textRecived("Device list is empty");
        return;
    }

    m_controller = QLowEnergyController::createCentral(*m_devices.at(index));
    connect(m_controller, &QLowEnergyController::connected, this, &BLEHandler::deviceConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BLEHandler::deviceDisconnected);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BLEHandler::addService);
    connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BLEHandler::serviceScanDone);
    connect(m_controller, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &BLEHandler::controllerError);

    emit textRecived("Connect to device..");
    m_controller->connectToDevice();
}

void BLEHandler::disconnectFromDevice() {
    m_controller->disconnectFromDevice();
    deviceDisconnected();
}

void BLEHandler::deviceConnected() {
    emit textRecived("Device Connected!");
    emit textRecived("Discover services..");
    m_controller->discoverServices();
}

void BLEHandler::deviceDisconnected() {
    emit textRecived("Device Disconnected");

    qDeleteAll(m_services);
    m_services.clear();
    emit serviceListUpdated();
    emit characteristicListUpdated();
}

void BLEHandler::addService(const QBluetoothUuid &newService) {
    emit textRecived("Service found - "+ newService.toString());
}

void BLEHandler::serviceScanDone() {
    auto serviceUuids = m_controller->services();
    if(serviceUuids.empty()) {
        emit textRecived("No services found");
        return;
    }

    for(auto serviceUuid : serviceUuids) {
        QLowEnergyService *service = m_controller->createServiceObject(serviceUuid);
        if(!service) {
            emit textRecived("Can't create service for this UUID : " + serviceUuid.toString());
            continue;
        }
        m_services.append(service);
        connect(service, &QLowEnergyService::stateChanged, this, &BLEHandler::serviceStateChanged);
        connect(service, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &BLEHandler::serviceError);
        connect(service, &QLowEnergyService::characteristicChanged, this, &BLEHandler::updateValue);
        emit textRecived("Discovering details for this device - " + service->serviceName() + " : " + serviceUuid.toString());
        service->discoverDetails();
    }
    emit serviceListUpdated();
    emit characteristicListUpdated();
}

void BLEHandler::serviceStateChanged(QLowEnergyService::ServiceState newState) {
    auto service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    if(newState == QLowEnergyService::InvalidService) {
        emit textRecived("Invalid service : " + service->serviceName());
        return;
    }
    else if(newState == QLowEnergyService::DiscoveryRequired) {
        emit textRecived("Discovery Required : " + service->serviceName());
        return;
    }
    else if(newState == QLowEnergyService::DiscoveringServices) {
        emit textRecived("Discovering Services : " + service->serviceName());
        return;
    }
    else if(newState == QLowEnergyService::LocalService) {
        emit textRecived("Local Service : " + service->serviceName());
    }
    else {
        emit textRecived("Service Discovered : " + service->serviceName());
    }
    emit characteristicListUpdated();
}

void BLEHandler::updateValue(QLowEnergyCharacteristic characteristic, QByteArray data) {
    emit valueChanged(data, characteristic.uuid().toString());
}

void BLEHandler::agentError(QBluetoothDeviceDiscoveryAgent::Error error) {
    emit textRecived("Agent Error : " + QVariant::fromValue(error).toString());
}

void BLEHandler::controllerError(QLowEnergyController::Error error) {
    emit textRecived("Controller Error : " + QVariant::fromValue(error).toString());
}

void BLEHandler::serviceError(QLowEnergyService::ServiceError error) {
    emit textRecived("Service Error : " + QVariant::fromValue(error).toString());
}

bool BLEHandler::subscribeData(const int &serviceIndex, const int &characteristicIndex) {
    auto service = m_services.at(serviceIndex);
    if(!service)
        return false;

    auto characteristic = service->characteristics().at(characteristicIndex);
    if(!characteristic.isValid())
        return false;

    auto descriptor = characteristic.descriptor(QBluetoothUuid(QBluetoothUuid::ClientCharacteristicConfiguration));
    if(!descriptor.isValid())
        return false;

    service->writeDescriptor(descriptor, QByteArray::fromHex("0100"));
    emit textRecived("Notify enabled");
    return true;
}

QByteArray BLEHandler::getData(const int &serviceIndex, const int &characteristicIndex) {
    auto characteristic = m_services.at(serviceIndex)->characteristics().at(characteristicIndex);
    m_services.at(serviceIndex)->readCharacteristic(characteristic);
    return m_services.at(serviceIndex)->characteristics().at(characteristicIndex).value();
}

QStringList BLEHandler::getCharacteristicListString(const int &serviceIndex) {
    QStringList res;
    if(!m_services.empty()) {
        if(m_services.at(serviceIndex)->state() == QLowEnergyService::ServiceDiscovered) {
            auto characteristics = m_services.at(serviceIndex)->characteristics();
            for(auto characteristic : characteristics) {
                res.append(characteristic.name() + " : " + characteristic.uuid().toString());
            }
        }
    }
    return res;
}

QStringList BLEHandler::getServiceListString() {
    QStringList res;
    for(auto service : m_services) {
        res.append(service->serviceName() + " : " + service->serviceUuid().toString());
    }
    return res;
}

QStringList BLEHandler::getDeviceListString() {
    QStringList res;
    for(auto device : m_devices) {
        res.append(device->name() + " : " + device->address().toString());
    }
    return res;
}
