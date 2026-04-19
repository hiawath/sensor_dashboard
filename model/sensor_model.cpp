#include "sensor_model.h"

SensorModel::SensorModel(QObject *parent) : SerialDevice(parent) {
    connect(this, &SerialDevice::readyRead, this, &SensorModel::handleRawData);
}

void SensorModel::handleRawData(const QByteArray &data) {
    m_buffer.append(data);

    while (m_buffer.contains('$') && m_buffer.contains('#')) {
        int start = m_buffer.indexOf('$');
        int end = m_buffer.indexOf('#', start);

        if (end == -1) break;

        QByteArray packet = m_buffer.mid(start + 1, end - start - 1);
        m_buffer.remove(0, end + 1);

        if (!packet.isEmpty()) {
            QString packetStr = QString::fromUtf8(packet);
            emit dataReceived(packetStr);
            parsePacket(packetStr);
        }
    }
}

void SensorModel::parsePacket(const QString &data) {
    // Format: count,id:type:val,id:type:val
    QStringList parts = data.split(',');
    if (parts.size() < 2) return;
    
    // parts[0] is count, but we can just iterate
    for (int i = 1; i < parts.size(); ++i) {
        QStringList item = parts[i].split(':');
        if (item.size() < 3) continue;

        int sensorId = item[0].toInt();
        QString valueStr = item[2];
        
        emit sensorUpdated(static_cast<SensorID>(sensorId), valueStr);
    }
}
