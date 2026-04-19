#ifndef SENSOR_MODEL_H
#define SENSOR_MODEL_H

#include "serial_device.h"
#include "serial_protocol.h"

class SensorModel : public SerialDevice {
    Q_OBJECT
public:
    explicit SensorModel(QObject *parent = nullptr);

signals:
    void dataReceived(const QString &rawPacket);
    void sensorUpdated(SensorID id, const QString &value);

private slots:
    void handleRawData(const QByteArray &data);

private:
    void parsePacket(const QString &packet);
    QByteArray m_buffer;
};

#endif // SENSOR_MODEL_H
