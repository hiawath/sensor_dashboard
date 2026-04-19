#ifndef DASHBOARD_VIEWMODEL_H
#define DASHBOARD_VIEWMODEL_H

#include <QObject>
#include "sensor_model.h"

class DashboardViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString temperature READ temperature NOTIFY temperatureChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    Q_PROPERTY(bool ledActive READ ledActive NOTIFY ledActiveChanged)

public:
    explicit DashboardViewModel(QObject *parent = nullptr);

    QString temperature() const { return m_temperature; }
    bool isConnected() const { return m_model->isOpen(); }
    bool ledActive() const { return m_ledActive; }

    void connectDevice(const QString &portName, int baudRate);
    void disconnectDevice();
    void toggleLed();
    void setMonitor(bool on);

signals:
    void temperatureChanged(const QString &temp);
    void connectionChanged(bool connected);
    void ledActiveChanged(bool active);
    void logMessage(const QString &type, const QString &msg);
    void rawDataReceived(const QString &packet);

private slots:
    void onSensorUpdated(SensorID id, const QString &value);
    void onDeviceError(const QString &error);

private:
    SensorModel *m_model;
    QString m_temperature;
    bool m_ledActive = false;
    bool m_isMonitoring = false;
};

#endif // DASHBOARD_VIEWMODEL_H
