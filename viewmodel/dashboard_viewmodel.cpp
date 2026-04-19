#include "dashboard_viewmodel.h"

DashboardViewModel::DashboardViewModel(QObject *parent) : QObject(parent) {
    m_model = new SensorModel(this);

    connect(m_model, &SensorModel::sensorUpdated, this, &DashboardViewModel::onSensorUpdated);
    connect(m_model, &SensorModel::connected, [this]() {
        emit connectionChanged(true);
        emit logMessage("SYS", "Connection : Port Opened");
    });
    connect(m_model, &SensorModel::disconnected, [this]() {
        emit connectionChanged(false);
        emit logMessage("SYS", "Port Closed");
    });
    connect(m_model, &SensorModel::errorOccurred, this, &DashboardViewModel::onDeviceError);
    connect(m_model, &SensorModel::dataReceived, [this](const QString &packet) {
        if (m_isMonitoring) {
            emit logMessage("RX", "$" + packet + "#");
            emit rawDataReceived(packet);
        }
    });
}

void DashboardViewModel::connectDevice(const QString &portName, int baudRate) {
    m_model->open(portName, baudRate);
}

void DashboardViewModel::disconnectDevice() {
    m_model->close();
}

void DashboardViewModel::toggleLed() {
    if (m_model->isOpen()) {
        QString cmd = "led toggle 1000\r\n";
        m_model->write(cmd.toUtf8());
        m_model->flush();
        emit logMessage("TX", ">> " + cmd.trimmed());
    }
}

void DashboardViewModel::setMonitor(bool on) {
    if (m_model->isOpen()) {
        m_isMonitoring = on;
        QString cmd = on ? "monitor on 100\r\n" : "monitor off\r\n";
        m_model->write(cmd.toUtf8());
        m_model->flush();
        emit logMessage("TX", ">> " + cmd.trimmed());
    }
}

void DashboardViewModel::onSensorUpdated(SensorID id, const QString &value) {
    if (id == ID_ENV_TEMP) {
        m_temperature = value;
        emit temperatureChanged(m_temperature + " °C");
    } else if (id == ID_OUT_LED_STATE) {
        bool active = (value.toInt() != 0);
        if (m_ledActive != active) {
            m_ledActive = active;
            emit ledActiveChanged(m_ledActive);
        }
    }
}

void DashboardViewModel::onDeviceError(const QString &error) {
    emit logMessage("ERR", "Error: " + error);
}
